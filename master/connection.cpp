#include "connection.hpp"
#include "master.hpp"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include "../lib/util.hpp"

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

// returns true if could match.  false otherwise
bool Connection::match_parts(
    std::string & msg,QueryKey& query_key, float& camera_x, float & camera_y, float& camera_z)
{
    // FIXME: should ensure that handling malformed requests
    
    boost::regex regex(
        "GET\\s+/(\\d+)/(\\d+[.]?\\d*)/(\\d+[.]?\\d*)/(\\d+[.]?\\d*)\\s+HTTP");

    std::string::iterator to_search_from, end;
    to_search_from = msg.begin();
    end = msg.end();
    boost::match_results<std::string::iterator> matches;
    
    if (boost::regex_search(to_search_from,end,matches,regex))
    {
        query_key = atoi(
            std::string(matches[1].first,matches[1].second).c_str());
        camera_x = atof(
            std::string(matches[2].first,matches[2].second).c_str());
        camera_y = atof(
            std::string(matches[3].first,matches[3].second).c_str());
        camera_z = atof(
            std::string(matches[4].first,matches[4].second).c_str());
        
        msg = std::string(matches[4].second,msg.end());
        return true;
    }

    return false;
}


void Connection::listen_for_queries()
{
    std::string msg_result (
        std::string("HTTP/1.1 200 OK \r\n") +
        std::string("Content-Type: text/html \r\n") +
        std::string("Content-Length: 5 \r\n\r\n") + 
        std::string("Hello"));
    
    while (true)
    {
        boost::asio::streambuf request;
        boost::asio::read_until(*socket, request, "\r\n\r\n");
        
        std::istream is(&request);
        std::string http_request_msg;
        std::getline(is, http_request_msg);

        
        QueryKey query_key;
        float camera_x, camera_y, camera_z;
        
        if (match_parts(
                http_request_msg,query_key,camera_x,camera_y,camera_z))
        {
            // garbage response for now
            boost::system::error_code ignored_error;
            boost::asio::write(
                *socket,
                boost::asio::buffer(msg_result.c_str()),
                boost::asio::transfer_all(), ignored_error);
        }
        else
        {
            // FIXME: actually should handle malformed requests
            std::cout<<"\nIncorrect http request!\n\n";
            std::cout<<http_request_msg<<"\n\n";
        }
    }
}

