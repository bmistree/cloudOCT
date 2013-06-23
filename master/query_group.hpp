#ifndef __QUERY_GROUP_HPP__
#define __QUERY_GROUP_HPP__

#include "volume_server_conn_info.hpp"
#include <map>
#include "../lib/util.hpp"
#include "query_connection.hpp"
#include <boost/function.hpp>
#include <boost/thread.hpp>

/**
   Each QueryGroup contains connections to a collection of servers that will
   respond to queries for a particular volume.  Ie, they all hold pieces of data
   for the same kidney, bladder, etc.
 */

typedef boost::function < void (QueryResponse*) > QueryCompleteCallback;

class Connection;

struct OutstandingQuery
{
    OutstandingQuery(const Query& query, QueryCompleteCallback _cb)
    {
        num_responded = 0;
        qr = new QueryResponse();
        qr->query_counter = query.query_counter;
        cb = _cb;
    }
    ~OutstandingQuery()
    {
        // NOTE: do not free qr, because this gets passed back in callback.
    }
    
    uint num_responded;
    QueryCompleteCallback cb;
    QueryResponse* qr;
};
typedef std::map<QueryCounter,OutstandingQuery*> OutstandingQueryMap;
typedef OutstandingQueryMap::iterator OutstandingQueryMapIter;


class QueryGroup
{
public:
    QueryGroup(
        boost::asio::io_service* io_service, QueryKey qk, ConnInfoVec conn_info_vec);

    ~QueryGroup();
    void submit_query(const Query& query, QueryCompleteCallback cb);
    
private:
    void query_response(QueryResponse* query_response);
    
    QueryKey query_key;
    std::vector< QueryConnection*> all_connections;
    // ConnMap all_connections;

    boost::mutex outstanding_queries_mutex;
    OutstandingQueryMap outstanding_queries;
};

typedef std::map<QueryKey,QueryGroup*> QueryGroupMap;
typedef QueryGroupMap::iterator QueryGroupMapIter;


#endif
