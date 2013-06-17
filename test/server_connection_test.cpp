#include "../master/master.hpp"
#include <boost/asio.hpp>

const PortNum CONNECTION_PORT = 5020;


int main (int argc, char** argv)
{
    boost::asio::io_service io_service;
    Master master(&io_service,CONNECTION_PORT,NULL);

    while(true){}
    
    return 0;
}
