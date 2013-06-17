#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

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
    
    tcp::socket* socket;
    Master* master;
    boost::thread* listening_thread;
    ConnectionId conn_id;
};

typedef std::map<ConnectionId,Connection*> ConnMap;
typedef ConnMap::iterator ConnMapIter;


#endif
