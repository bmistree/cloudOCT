#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../lib/util.hpp"
#define CONNECTION_READ_BUFFER_SIZE 100

typedef uint ConnectionId;
class Master;


using boost::asio::ip::tcp;
class Connection
{
public:
    Connection (tcp::socket* socket, Master* master, ConnectionId);
    ~Connection();
private:

    void listen_for_queries();
    bool match_parts(
        std::string & to_read,QueryKey& query_key, float& camera_x,
        float & camera_y, float& camera_z);
    
    tcp::socket* socket;
    Master* master;
    boost::thread* listening_thread;
    ConnectionId conn_id;
};

typedef std::map<ConnectionId,Connection*> ConnMap;
typedef ConnMap::iterator ConnMapIter;


#endif
