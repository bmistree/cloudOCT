#include "query_group.hpp"
#include "query_connection.hpp"

QueryGroup::QueryGroup(
    boost::asio::io_service* io_service, QueryKey qk, ConnInfoVec conn_info_vec)
{
    query_key = qk;

    for (ConnInfoVecIter conn_iter = conn_info_vec.begin();
         conn_iter != conn_info_vec.end(); ++conn_iter)
    {
        QueryConnection* qc =
            new QueryConnection(
                io_service, conn_iter->server_ip_address,
                conn_iter->server_port);

        all_connections.push_back(qc);
    }
}

QueryGroup::~QueryGroup()
{
    // FIXME: Not performing any clean up
}

void QueryGroup::query_response(QueryResponse* query_response)
{
    QueryCounter counter = query_response->query_counter;

    OutstandingQuery* outstanding_query;
    bool issue_callback = false;
    // grab the associated element from the map of outstanding queries
    {
        boost::mutex::scoped_lock lock (outstanding_queries_mutex);
        outstanding_query = outstanding_queries[counter];

        outstanding_query->num_responded ++;

        // check if query is finished and need to remove from map
        if (outstanding_query->num_responded == all_connections.size())
        {
            issue_callback = true;
            outstanding_queries.erase(counter);
        }
    }

    // incorporate response into full response
    outstanding_query->qr->merge_query_data_into_me(*query_response);

    // free existing query response
    delete query_response;

    // issue callback if necessary
    if (issue_callback)
    {
        outstanding_query->cb(outstanding_query->qr);
        delete outstanding_query;
    }
}

void QueryGroup::submit_query(
    const Query& query, QueryCompleteCallback cb)
{
    // Register this query in our map of outstanding queries
    OutstandingQuery* outstanding_query = new OutstandingQuery(query,cb);
    {
        boost::mutex::scoped_lock lock (outstanding_queries_mutex);
        outstanding_queries[query.query_counter] = outstanding_query;
    }

    // Create a new callback for each query connection to execute when its query
    // has completed
    QueryCallback query_cb = boost::bind(&QueryGroup::query_response,this,_1);


    // actually issue the query
    for (ConnVecIter iter = all_connections.begin();
         iter != all_connections.end(); ++iter)
    {
        (*iter)->issue_query(query,query_cb);
    }
}

