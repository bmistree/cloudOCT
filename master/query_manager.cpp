#include "query_manager.hpp"

QueryManager::QueryManager(
    boost::asio::io_service* io_service,AllConnInfoMap all_conn_info_map)
{
    // Create a query group map
    for (AllConnInfoMapIter iter = all_conn_info_map.begin();
         iter != all_conn_info_map.end(); ++iter)
    {
        QueryKey query_key = iter->first;
        ConnInfoVec civ = iter->second;
        QueryGroup* query_group = new QueryGroup(io_service,query_key,civ);
        query_group_map[query_key] = query_group;
    }
}

QueryManager::~QueryManager()
{
    // FIXME: not performing any clean up
}


bool QueryManager::issue_query(const Query& q,QueryCompleteCallback cb)
{
    QueryGroupMapIter iter = query_group_map.find(q.query_key);
    if (iter == query_group_map.end())
        return false;
    QueryGroup* qg = iter->second;
    qg->submit_query(q, cb);
    return true;
}
