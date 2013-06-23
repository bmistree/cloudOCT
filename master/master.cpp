#include "master.hpp"

using boost::asio::ip::tcp;
Master::Master(
    boost::asio::io_service* _io_service,PortNum query_port,QueryManager* q_manager)
{
    query_manager = q_manager;
    io_service = _io_service;
    connection_port_num = query_port;

    boost::thread query_listening_thread = 
        boost::thread(
            boost::bind(&Master::listen_for_new_connections,this));
    query_listening_thread.detach();
}


void Master::listen_for_new_connections()
{
    ConnectionId conn_id = 0;
    
    tcp::acceptor acceptor(
        *io_service, tcp::endpoint(tcp::v4(), connection_port_num));
    
    while (true)
    {
        ++conn_id;
        tcp::socket* socket = new tcp::socket(*io_service);
        acceptor.accept(*socket);
        {
            boost::mutex::scoped_lock lock(conn_map_mutex);
            Connection* external_connection = new Connection(socket,query_manager,conn_id);
            connection_map[conn_id] = external_connection;
        }
    }
}

Master::~Master()
{
    // FIXME: need to actually clean up connections.
}

