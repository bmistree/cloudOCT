#ifndef __QUERY_CONNECTION_HPP__
#define __QUERY_CONNECTION_HPP__

#define READ_BUFFER_SIZE 100

#include "../lib/util.hpp"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <vector>
using boost::asio::ip::tcp;

typedef boost::function<void (QueryResponse*)> QueryCallback;

class QueryConnection
{
public:
    QueryConnection(
        boost::asio::io_service* io_service, boost::asio::ip::address, PortNum);
    void issue_query(const Query& query,QueryCallback query_cb);
    ~QueryConnection();
private:
    void connection_listen();
    std::map<QueryCounter,QueryCallback> callback_map;
    tcp::socket* socket;
    boost::thread* listening_thread;
    boost::mutex callback_map_mutex;
};

typedef std::vector<QueryConnection*> ConnVec;
typedef ConnVec::iterator ConnVecIter;

#endif 
