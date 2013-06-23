#include "../lib/util.hpp"
#include <string>
#include <cassert>

void test_query_serialize();
void test_query_response_serialize();

int main(int argc, char** argv)
{
    test_query_serialize();
    test_query_response_serialize();
    return 0;
}

void test_query_response_serialize()
{
    QueryResponse* qr1 = QueryResponse::noise_query_response(1,20);
    QueryResponse* qr2 = QueryResponse::noise_query_response(13,120);

    
    std::string serialized,other;
    qr1->serialize(serialized);
    qr2->serialize(other);
    serialized += other;

    QueryResponse dqr1,dqr2,dqr3;

    if (! QueryResponse::deserialize(dqr1,serialized))
        assert(false);
    if (! QueryResponse::deserialize(dqr2,serialized))
        assert(false);

    if (dqr1.bmp_dat != qr1->bmp_dat)
        assert(false);
    if (dqr2.bmp_dat != qr2->bmp_dat)
        assert(false);
    
    if (QueryResponse::deserialize(dqr3,serialized))
        assert(false);
}

void test_query_serialize()
{
    Query q1;
    q1.x = 302;
    q1.y = 39;
    q1.z = 313;
    q1.query_key = 1;
    std::string serialized;
    q1.serialize(serialized);

    Query q2;
    q2.x = 302.39392;
    q2.y = 39.1;
    q2.z = 3.13;
    q2.query_key = 3;
    std::string other_serialized;
    q2.serialize(other_serialized);
    serialized += other_serialized;
    
    
    Query to_deserialize_to1, to_deserialize_to2, to_deserialize_to3;
    if (! Query::deserialize(to_deserialize_to1,serialized))
        assert(false);

    
    if (! Query::deserialize(to_deserialize_to2,serialized))
        assert(false);
    
    // should not be able to deserialize any more
    if (Query::deserialize(to_deserialize_to3,serialized))
        assert(false);

    if (q1 != to_deserialize_to1)
        assert(false);
    if (q2 != to_deserialize_to2)
        assert(false);
}
