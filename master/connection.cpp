#include "connection.hpp"
#include "master.hpp"
#include <iostream>

using boost::asio::ip::tcp;
Connection::Connection (tcp::socket* _socket, Master* _master,ConnectionId _conn_id)
{
    socket = _socket;
    master = _master;
    conn_id = _conn_id;
    
    listening_thread = 
        new boost::thread(
            boost::bind(&Connection::listen_for_queries,this));
}

Connection::~Connection()
{
    // FIXME: need to free socket and listening thread here
}

void Connection::listen_for_queries()
{
    std::string msg_result ("Result");
    std::string read_msg;
    while (true)
    {
        char msg [CONNECTION_READ_BUFFER_SIZE];
        int read_len = socket->read_some(
            boost::asio::buffer(msg, CONNECTION_READ_BUFFER_SIZE));
        read_msg.append(msg,read_len);
        
        boost::system::error_code ignored_error;
        boost::asio::write(
            *socket,
            boost::asio::buffer(msg_result.c_str()),
            boost::asio::transfer_all(), ignored_error);
    }
}

