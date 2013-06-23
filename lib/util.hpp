#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <sstream>
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <string>
#include <iomanip>
#include <vector>

#include <stdlib.h>
#include <time.h> 

#include "../deps/lodepng/lodepng.h"

#define FLOAT_WIDTH_PRECISION 20
#define IMAGE_WIDTH 200
#define IMAGE_HEIGHT 200
#define RGBA_PIXEL_BYTES 4
// The total amount of data that we're passing each way should be the number of
// pixels in the image (image_width*image_height), multiplied by the amount of
// data required for each pixel.
#define QUERY_DATA_SIZE IMAGE_WIDTH*IMAGE_HEIGHT*RGBA_PIXEL_BYTES



typedef uint PortNum;
typedef uint QueryKey;
typedef uint QueryCounter;

typedef uint ConnectionId;


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

    // used to distinguish what data set are for
    QueryKey query_key;
    // used to distinguish which query replying to.
    QueryCounter query_counter;
    // position of the camera.
    float x,y,z;


    bool operator== (const Query& q)
    {
        return ((query_counter == q.query_counter) &&
            (query_key == q.query_key));
    }

    bool operator!= (const Query& q)
    {
        return ((query_counter != q.query_counter) ||
            (query_key != q.query_key));
    }
    
    
    void serialize(std::string& to_serialize_to) const
    {
        std::ostringstream str_stream;
        to_serialize_to = "";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << query_key << " ";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << query_counter << " ";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << x << " ";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << y << " ";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << z << " ";

        to_serialize_to = str_stream.str();
    }
    
    static bool deserialize(Query& q, std::string& msg)
    {
        boost::regex regex(
            "(\\d+)\\s+(\\d+)\\s+(\\d+[.]*\\d*)\\s+(\\d+[.]*\\d*)\\s+(\\d+[.]*\\d*)");

        std::string::iterator to_search_from, end;
        to_search_from = msg.begin();
        end = msg.end();
        boost::match_results<std::string::iterator> matches;

        if (boost::regex_search(to_search_from,end,matches,regex))
        {
            q.query_key = atoi(
                std::string(matches[1].first,matches[1].second).c_str());
            q.query_counter = atoi(
                std::string(matches[2].first,matches[2].second).c_str());
            q.x = atof(
                std::string(matches[3].first,matches[3].second).c_str());
            q.y = atof(
                std::string(matches[4].first,matches[4].second).c_str());
            q.z = atof(
                std::string(matches[5].first,matches[5].second).c_str());

            msg = std::string(matches[5].second,msg.end());
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


    // Provides a randomly-generated result image for debugging.
    static QueryResponse* noise_query_response(
        QueryKey query_key, QueryCounter query_counter)
    {
        QueryResponse * to_return = new QueryResponse();
        to_return->query_key = query_key;
        to_return->query_counter = query_counter;

        for (int query_data_index = 0; query_data_index < QUERY_DATA_SIZE;
             ++query_data_index)
        {
            to_return->bmp_dat.push_back(rand() % 256);
        }
        return to_return;
    }
    
    void serialize(std::string& to_serialize_to) const
    {
        std::ostringstream str_stream;
        to_serialize_to = "";

        std::vector<unsigned char> png_dat;
        unsigned error = lodepng::encode(png_dat, bmp_dat, IMAGE_WIDTH, IMAGE_HEIGHT);
        if (error)
            assert(false);

        int msg_size = (3 * FLOAT_WIDTH_PRECISION) + 3 + png_dat.size();
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << msg_size <<" ";
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << query_key << " ";        
        str_stream << std::setw(FLOAT_WIDTH_PRECISION) << query_counter << " ";
        to_serialize_to = str_stream.str();
        to_serialize_to.append(png_dat.begin(),png_dat.end());
    }

    static bool deserialize(QueryResponse& q, std::string& msg)
    {
        if (msg.size() < FLOAT_WIDTH_PRECISION)
            return false;

        uint msg_size = atoi(msg.substr(0,FLOAT_WIDTH_PRECISION).c_str());
        if (msg_size > msg.size())
            return false;

        q.query_key = atoi(
            msg.substr(FLOAT_WIDTH_PRECISION + 1, 2*FLOAT_WIDTH_PRECISION).c_str());
        q.query_counter = atoi(
            msg.substr(2*(FLOAT_WIDTH_PRECISION + 1), 3*FLOAT_WIDTH_PRECISION).c_str());

        uint png_size = msg_size - 3*(FLOAT_WIDTH_PRECISION+1);
        std::vector<uint8_t> png_dat;
        uint base_start = 3*(FLOAT_WIDTH_PRECISION + 1);
        for (uint index= base_start; index < (base_start + png_size); ++index)
            png_dat.push_back(msg[index]);

        uint garbage_width, garbage_height;
        unsigned error = lodepng::decode(q.bmp_dat, garbage_width,garbage_height, png_dat);
        
        if (error)
            assert(false);

        msg = msg.substr( msg_size );
        return true;
    }

    void merge_query_data_into_me(const QueryResponse& to_merge_with)
    {
        // only concurrent operations are merging data into self
        boost::mutex::scoped_lock lock (merge_mutex);
        
        // FIXME: should actually do useful work.
        if (bmp_dat.size() == 0)
        {
            for (uint index = 0; index < QUERY_DATA_SIZE; ++index)
                bmp_dat.push_back(to_merge_with.bmp_dat[index]);
        }
        else
        {
            for (uint index = 0; index < bmp_dat.size(); ++index)
                bmp_dat[index] += to_merge_with.bmp_dat[index];
        }
    }

    // Returns true if successfully decoded, false otherwise.
    bool to_png(std::vector<unsigned char>& png_dat)
    {
        unsigned error = lodepng::encode(png_dat, bmp_dat, IMAGE_WIDTH, IMAGE_HEIGHT);
        if (error)
            assert(false);
        
        return ! error;
    }

    QueryKey query_key;
    QueryCounter query_counter;
    std::vector<uint8_t>bmp_dat;
    boost::mutex merge_mutex;
};


#endif
