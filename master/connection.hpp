#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../lib/util.hpp"
#include "query_manager.hpp"

#define CONNECTION_READ_BUFFER_SIZE 100

class Master;


using boost::asio::ip::tcp;
class Connection
{
public:
    Connection (tcp::socket* socket, QueryManager* query_manager, ConnectionId);
    ~Connection();
private:
    void query_response(QueryResponse* query_response);
    void listen_for_queries();
    bool match_parts(
        std::string & to_read,QueryKey& query_key, float& camera_x,
        float & camera_y, float& camera_z);
    
    tcp::socket* socket;
    QueryManager* query_manager;
    boost::thread* listening_thread;
    ConnectionId conn_id;
};


typedef std::map<ConnectionId,Connection*> ConnMap;
typedef ConnMap::iterator ConnMapIter;


#endif
