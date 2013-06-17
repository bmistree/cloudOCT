#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <sstream>
#include <boost/regex.hpp>
#include <string>
#include <iomanip>

#define FLOAT_WIDTH_PRECISION 20
#define QUERY_DATA_SIZE 5

// how long each message from volume servers is
#define QUERY_RESPONSE_MESSAGE_LENGTH  FLOAT_WIDTH_PRECISION + QUERY_DATA_SIZE + 1

typedef uint PortNum;
typedef uint QueryKey;
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
        std::ostringstream str_stream;
        to_serialize_to = "";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << query_counter << " ";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << x << " ";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << y << " ";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << z << " ";

        to_serialize_to = str_stream.str();
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
    QueryResponse()
    {}
    ~QueryResponse()
    {}

    void serialize(std::string& to_serialize_to)
    {
        std::ostringstream str_stream;
        to_serialize_to = "";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << query_counter << " ";        
        to_serialize_to = str_stream.str();
        to_serialize_to.append(query_data, QUERY_DATA_SIZE);
    }

    static bool deserialize(QueryResponse& q, std::string& msg)
    {
        if (msg.size() < QUERY_RESPONSE_MESSAGE_LENGTH)
            return false;

        q.query_counter = atof(msg.substr(0,FLOAT_WIDTH_PRECISION).c_str());
        memcpy(
            q.query_data,
            msg.substr(FLOAT_WIDTH_PRECISION +1,QUERY_DATA_SIZE).c_str(),
            QUERY_DATA_SIZE);

        msg = msg.substr(QUERY_RESPONSE_MESSAGE_LENGTH );
        return true;
    }

    void merge_query_data_into_me(const QueryResponse& to_merge_with)
    {
        // FIXME: should actually do useful work.
        for (int index = 0; index < QUERY_DATA_SIZE; ++index)
            query_data[index] += to_merge_with.query_data[index];
    }
    
    QueryCounter query_counter;
    char query_data [QUERY_DATA_SIZE];
};


#endif
