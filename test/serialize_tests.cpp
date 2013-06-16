#include "../lib/util.hpp"
#include <string>
#include <cassert>

int main(int argc, char** argv)
{
    Query q1;
    q1.x = 302;
    q1.y = 39;
    q1.z = 313;
    std::string serialized;
    q1.serialize(serialized);

    Query q2;
    q2.x = 302.39392;
    q2.y = 39.1;
    q2.z = 3.13;
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

    return 0;
}
