#ifndef __VOLUME_SERVER_CONN_INFO_HPP__
#define __VOLUME_SERVER_CONN_INFO_HPP__

#include "../lib/util.hpp"
#include <boost/asio.hpp>


/**
   Contains all info necessray to create a connection to a volume server.
 */
struct VolumeServerConnInfo
{
    VolumeServerConnInfo(
        QueryKey _query_key,PortNum _server_port,
        boost::asio::ip::address _server_ip_address)
     : query_key(_query_key),
       server_port(_server_port),
       server_ip_address(_server_ip_address)
    {}
    
    QueryKey query_key;
    PortNum server_port;
    boost::asio::ip::address server_ip_address;
};

typedef std::vector<VolumeServerConnInfo> ConnInfoVec;
typedef ConnInfoVec::iterator ConnInfoVecIter;
typedef std::map<QueryKey,ConnInfoVec> AllConnInfoMap;
typedef AllConnInfoMap::iterator AllConnInfoMapIter;

#endif
