COMPILER= g++
FLAGS= -Wall -g -std=c++0x 
#LIBS= -I/usr/local/include/
LIBS= -lpthread -lboost_system -lboost_thread
DEPS= ../lib/util.hpp
PROGRAMS= 
SOS=/usr/lib/libboost_regex.so

serialize_tests: ../test/serialize_tests.cpp ../lib/util.hpp $(SOS)
	$(COMPILER) -o serialize_tests ../test/serialize_tests.cpp $(SOS)

server_connection_test: ../master/master.cpp ../master/connection.cpp ../test/server_connection_test.cpp
	$(COMPILER) -o server_connection_test ../test/server_connection_test.cpp ../master/master.cpp ../master/connection.cpp $(LIBS) 