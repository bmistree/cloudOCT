#include "../master/master.hpp"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../master/volume_server_conn_info.hpp"
#include "../volume_server/volume_server.hpp"
#include <stdlib.h>
#include <time.h> 


const PortNum MASTER_CONNECTION_PORT = 5020;
const QueryKey QUERY_KEY = 1;

const PortNum VOLUME_SERVER_PORT = 4390;

void setup_volume_servers(boost::asio::io_service* io_service,AllConnInfoMap);
void setup_master_server(boost::asio::io_service* io_service,AllConnInfoMap);
void setup_dummy_conn_info(AllConnInfoMap& all_conn_info);


int main (int argc, char** argv)
{
    srand(time(NULL));
    boost::asio::io_service io_service;

    AllConnInfoMap all_conn_info;
    setup_dummy_conn_info(all_conn_info);
    
    setup_volume_servers(&io_service,all_conn_info);

    // pause for the volume servers to get setup
    boost::this_thread::sleep( boost::posix_time::milliseconds(1000) );

    // setup master server
    setup_master_server(&io_service,all_conn_info);

    // run forever
    while(true){}
    return 0;
}

void setup_dummy_conn_info(AllConnInfoMap& all_conn_info_map)
{
    // Just create a single query key

    all_conn_info_map[QUERY_KEY].push_back(
        VolumeServerConnInfo(
            QUERY_KEY,
            VOLUME_SERVER_PORT,
            boost::asio::ip::address::from_string("127.0.0.1")));
}


void setup_volume_servers(
    boost::asio::io_service* io_service,AllConnInfoMap all_conn_info_map)
{
    for (AllConnInfoMapIter iter = all_conn_info_map.begin();
         iter != all_conn_info_map.end(); ++iter)
    {
        for(ConnInfoVecIter vec_iter = iter->second.begin();
            vec_iter != iter->second.end(); ++vec_iter)
        {
            VolumeServer::start_server_detached_thread(
                io_service,vec_iter->server_port);
        }
    }
}

void setup_master_server(
    boost::asio::io_service* io_service,AllConnInfoMap all_conn_info_map)
{
    // note: never actually freeing memory
    QueryManager* query_manager =
        new QueryManager(io_service,all_conn_info_map);
    Master* master = new Master (io_service,MASTER_CONNECTION_PORT,query_manager);
}
