#ifndef __MASTER_HPP__
#define __MASTER_HPP__

#include "../lib/util.hpp"
#include "connection.hpp"
#include <boost/thread.hpp>
#include "query_manager.hpp"


class Master
{
public:
    Master(
        boost::asio::io_service* _io_service,PortNum query_port,
        QueryManager* q_manager);
    ~Master();

    
private:
    void listen_for_new_connections();

    /** Listens for queries on this port.  A query should just contain a
     * camera's position as well as an identifier for the data set to apply the
     * query to.*/
    QueryManager* query_manager;

    PortNum connection_port_num;
    boost::asio::io_service* io_service;

    ConnMap connection_map;
    boost::mutex conn_map_mutex;
};


#endif
