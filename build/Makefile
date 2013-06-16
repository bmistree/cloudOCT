COMPILER= g++
FLAGS= -Wall -g -std=c++0x 
LIBS= -I/usr/local/include/
DEPS= ../lib/util.hpp
PROGRAMS= 
SOS=/usr/lib/libboost_regex.so

serialize_tests: ../test/serialize_tests.cpp ../lib/util.hpp $(SOS)
	$(COMPILER) -o serialize_tests ../test/serialize_tests.cpp $(SOS)
