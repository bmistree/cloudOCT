#ifndef __VOLUME_SERVER_HPP__
#define __VOLUME_SERVER_HPP__

#include <boost/asio.hpp>
#include "../lib/util.hpp"

#define READ_BUFFER_SIZE 100

using boost::asio::ip::tcp;

class VolumeServer
{
public:
    VolumeServer(
        boost::asio::io_service* io_service,
        PortNum listen_on_port_num);
    
    ~VolumeServer();

    static void start_server_detached_thread(
        boost::asio::io_service* io_service,
        PortNum listen_on_port_num);
    static void _start_server_detached_thread(
        boost::asio::io_service* io_service,
        PortNum listen_on_port_num);
    
    
private:
    void listen_for_new_connections();

    // Runs through all data held by volume server in a separate thread, then
    // adds response to output socket.  This method should free query.
    virtual void handle_query(Query* query);

    // query_response is handed over to this method, which then frees it
    void write_response(QueryResponse* query_response);

    
    boost::asio::io_service* io_service;
    tcp::socket* socket;
    PortNum listen_on_port_num;
};

#endif
