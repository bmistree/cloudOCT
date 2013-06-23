#include "query_connection.hpp"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <cassert>

using boost::asio::ip::tcp;

QueryConnection::QueryConnection(
    boost::asio::io_service* io_service,boost::asio::ip::address ip_addr, PortNum pnum)
{
    socket = new tcp::socket(*io_service);
    tcp::resolver resolver(*io_service);
    tcp::resolver::query query(
        ip_addr.to_string(),
        boost::lexical_cast< std::string >( pnum ));

    tcp::resolver::iterator iterator = resolver.resolve(query);
    boost::system::error_code error = boost::asio::error::host_not_found;
    socket->connect(*iterator,error);
    
    listening_thread = new boost::thread(
        boost::bind(&QueryConnection::connection_listen,this));
}

void QueryConnection::connection_listen()
{
    std::string read_msg;
    QueryResponse* qr = new QueryResponse();
    while (true)
    {
        char msg [READ_BUFFER_SIZE];
        int read_len = socket->read_some(boost::asio::buffer(msg, READ_BUFFER_SIZE));
        read_msg.append(msg,read_len);

        // try to deserialize the message into a query response
        if (QueryResponse::deserialize(*qr,read_msg))
        {
            boost::mutex::scoped_lock lock(callback_map_mutex);

            std::map<QueryCounter,QueryCallback>::iterator iter =
                callback_map.find(qr->query_counter);
            if (iter == callback_map.end())
                assert(false);

            QueryCallback query_cb = iter->second;
            query_cb(qr);
            callback_map.erase(iter);
            qr = new QueryResponse();
        }
    }
}

QueryConnection::~QueryConnection()
{
    // FIXME: handle shut down
}


void QueryConnection::issue_query(const Query& query,QueryCallback query_cb)
{
    {
        // add callback to map
        boost::mutex::scoped_lock lock (callback_map_mutex);
        callback_map[query.query_counter] = query_cb;
    }

    // serialize query
    std::string serialized_query;
    query.serialize(serialized_query);

    // actually write query to wire
    boost::system::error_code ignored_error;
    boost::asio::write(*socket, boost::asio::buffer(serialized_query.c_str()),
        boost::asio::transfer_all(), ignored_error);
}

