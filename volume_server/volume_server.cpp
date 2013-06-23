#include "volume_server.hpp"
#include "../lib/util.hpp"
#include <boost/thread.hpp>
using boost::asio::ip::tcp;

VolumeServer::VolumeServer(
    boost::asio::io_service* _io_service,
    PortNum _listen_on_port_num)
{
    io_service = _io_service;
    listen_on_port_num = _listen_on_port_num;
    listen_for_new_connections();
}


void VolumeServer::listen_for_new_connections()
{
    // create a socket 
    tcp::acceptor acceptor(
        *io_service, tcp::endpoint(tcp::v4(), listen_on_port_num));

    socket = new tcp::socket(*io_service);
    acceptor.accept(*socket);

    // listen on that socket for queries
    Query* query = new Query();
    std::string read_msg;
    while (true)
    {
        char msg [READ_BUFFER_SIZE];
        int read_len = socket->read_some(boost::asio::buffer(msg, READ_BUFFER_SIZE));
        read_msg.append(msg,read_len);
        // try to deserialize received message
        if (Query::deserialize(*query, read_msg))
        {
            boost::thread query_handling_thread (
                boost::bind(&VolumeServer::handle_query,this,query));
            query_handling_thread.detach();
            query = new Query();
        }
    }
}

void VolumeServer::start_server_detached_thread(
    boost::asio::io_service* io_service, PortNum listen_on_port_num)
{
    boost::thread new_server_thread (
        boost::bind(VolumeServer::_start_server_detached_thread,
            io_service,listen_on_port_num));
    new_server_thread.detach();
}

void VolumeServer::_start_server_detached_thread(
    boost::asio::io_service* io_service,PortNum listen_on_port_num)
{
    new VolumeServer(io_service,listen_on_port_num);
}


void VolumeServer::write_response(QueryResponse* query_response)
{
    std::string str_to_write;
    query_response->serialize(str_to_write);
    boost::system::error_code ignored_error;
    boost::asio::write(*socket,
        boost::asio::buffer(str_to_write.c_str(), str_to_write.size()),
        boost::asio::transfer_all(), ignored_error);
    delete query_response;
}

void VolumeServer::handle_query(Query* query)
{
    QueryResponse* query_response = QueryResponse::noise_query_response(
        query->query_key,query->query_counter);
    
    delete query;
    write_response(query_response);
}

    
VolumeServer::~VolumeServer()
{
    // FIXME: actually perform cleanup
}
