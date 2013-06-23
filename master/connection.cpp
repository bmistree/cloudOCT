#include "connection.hpp"
#include "master.hpp"
#include "query_manager.hpp"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "../lib/util.hpp"
#include <sstream>

using boost::asio::ip::tcp;
Connection::Connection (tcp::socket* _socket, QueryManager* _query_manager,ConnectionId _conn_id)
{
    socket = _socket;
    query_manager = _query_manager;
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

void Connection::query_response(QueryResponse* query_response)
{
    std::vector<unsigned char> png_dat;
    if (! query_response->to_png(png_dat))
        assert(false);

    std::string msg_result (
        "HTTP/1.1 200 OK \r\n"
        "Content-Type: image/png \r\n"
        "Content-Length: ");
    msg_result += boost::lexical_cast<std::string>(png_dat.size());
    msg_result += " \r\n\r\n";
    msg_result.append(png_dat.begin(),png_dat.end());

    boost::system::error_code ignored_error;
    boost::asio::write(
        *socket,
        boost::asio::buffer(msg_result.c_str(),msg_result.size()),
        boost::asio::transfer_all(), ignored_error);
    
    delete query_response;
}

void Connection::listen_for_queries()
{
    std::string four_oh_four_payload (
        "<html>"
        "<head><title>Not Found</title></head>"
        "<body><h1>"
        "404 No resource matching query key.  Try a different URL"
        "</h1></body>"
        "</html>");
    
    std::string four_oh_four_msg(
        "HTTP/1.1 404 Not Found \r\n"
        "Content-Type: text/html \r\n"
        "Content-Length: ");
    four_oh_four_msg += boost::lexical_cast<std::string>(
        four_oh_four_payload.size());
    four_oh_four_msg += "\r\n\r\n";
    four_oh_four_msg += four_oh_four_payload;

    QueryCompleteCallback complete_callback =
        boost::bind(&Connection::query_response,this, _1);
    
    while (true)
    {
        boost::asio::streambuf request;
        // read until the end of an http request
        boost::asio::read_until(*socket, request, "\r\n\r\n");

        // parse query request message 
        std::istream is(&request);
        std::string http_request_msg;
        std::getline(is, http_request_msg);

        QueryKey query_key;
        float camera_x, camera_y, camera_z;
        
        if (match_parts(
                http_request_msg,query_key,camera_x,camera_y,camera_z))
        {
            // request the master to process the query.  wait on result of query
            // and return response.
            Query query;
            query.query_key = query_key;
            query.x = camera_x;
            query.y = camera_y;
            query.z = camera_z;

            // submit the query to the backend volume servers
            if (! query_manager->issue_query(query,complete_callback))
            {
                // we did not have the query_key.  Display 404 error
                boost::system::error_code ignored_error;
                boost::asio::write(
                    *socket,
                    boost::asio::buffer(four_oh_four_msg.c_str(),four_oh_four_msg.size()),
                    boost::asio::transfer_all(), ignored_error);
            }
        }
        else
        {
            // FIXME: actually should handle malformed requests
            std::cout<<"\nIncorrect http request!\n\n";
            std::cout<<http_request_msg<<"\n\n";
        }
    }
}

