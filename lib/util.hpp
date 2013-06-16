#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <sstream>
#include <boost/regex.hpp>
#include <string>
#include <iomanip>

#define MESSAGE_SIZE_LEN_BYTES 20

typedef uint PortNum;
typedef uint QueryKey;

typedef uint QueryData;
typedef uint QueryCounter;

/**
 * Query should just contain the camera's x,y,and z positions.
 */
struct Query
{
    Query()
    {
        static QueryCounter q_count = 0;
        query_counter = q_count ++;
    }
    ~Query() {}
    
    // used to distinguish which query replying to.
    QueryCounter query_counter;
    // position of the camera.
    float x,y,z;


    bool operator== (const Query& q)
    {
        return (query_counter == q.query_counter);
    }

    bool operator!= (const Query& q)
    {
        return (query_counter != q.query_counter);
    }
    
    
    void serialize(std::string& to_serialize_to)
    {
        int width = 30;
        std::ostringstream str_stream;
        to_serialize_to = "";
        str_stream << std::setw(width) << query_counter << " ";
        str_stream << std::setw(width) << x << " " << std::setw(width) << y << " ";
        str_stream << std::setw(width) << z << " ";

        to_serialize_to = str_stream.str();

        // ostringstream size_header_stream;
        // size_header_stream << setw(MESSAGE_SIZE_LEN_BYTES) << to_serialize_to.size();
        // size_header_stream <<" ";
    }
    
    static bool deserialize(Query& q, std::string& msg)
    {
        boost::regex regex(
            "(\\d+)\\s+(\\d+[.]*\\d+)\\s+(\\d+[.]*\\d+)\\s+(\\d+[.]*\\d+)");

        std::string::iterator to_search_from, end;
        to_search_from = msg.begin();
        end = msg.end();
        boost::match_results<std::string::iterator> matches;

        if (boost::regex_search(to_search_from,end,matches,regex))
        {
            q.query_counter = atoi(
                std::string(matches[1].first,matches[1].second).c_str());
            q.x = atof(
                std::string(matches[2].first,matches[2].second).c_str());
            q.y = atof(
                std::string(matches[3].first,matches[3].second).c_str());
            q.z = atof(
                std::string(matches[4].first,matches[4].second).c_str());

            msg = std::string(matches[4].second,msg.end());
            return true;
        }
        return false;
    }
};


struct QueryResponse
{
    QueryCounter query_counter;
    QueryData query_data;
};


#endif
