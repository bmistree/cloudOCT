#ifndef __QUERY_MANAGER_HPP__
#define __QUERY_MANAGER_HPP__

#include "volume_server_conn_info.hpp"
#include "query_group.hpp"

/**
   Takes any requested query and directs it to the appropriate query group which
   then processes it.  
 */

class QueryManager
{
public:
    QueryManager(
        boost::asio::io_service* io_service,AllConnInfoMap all_conn_info_map);
    ~QueryManager();

    // Returns true if the query key exists.  false if it does not.
    bool issue_query(const Query& q,QueryCompleteCallback cb);
private:
    QueryGroupMap query_group_map;
};


#endif
