COMPILER= g++
FLAGS= -Wall -g -std=c++0x 
LIBS= -lpthread -lboost_system -lboost_thread
DEPS= ../lib/util.hpp
PROGRAMS= serialize_tests server_connection_test
SOS=/usr/lib/libboost_regex.so

PNG_DEP_FOLDER= ../deps/lodepng

# SOS=/usr/lib/libboost_regex.so.1.46.1

# for each .cpp file (or any change to DEPS), create a .o
# file -c: generate the object file.  -o: put the object file in the
# file named $@, which is the file on the left hand side of :
%.o: ../lib/%.cpp $(DEPS)
	$(COMPILER) -c -o $@ $< $(FLAGS) $(LIBS) 
%.o: ../master/%.cpp $(DEPS)
	$(COMPILER) -c -o $@ $< $(FLAGS) $(LIBS) 
%.o: ../test/%.cpp $(DEPS)
	$(COMPILER) -c -o $@ $< $(FLAGS) $(LIBS) 
%.o: ../volume_server/%.cpp $(DEPS)
	$(COMPILER) -c -o $@ $< $(FLAGS) $(LIBS) 

$(PNG_DEP_FOLDER)/%.o: $(PNG_DEP_FOLDER)/%.cpp
	$(COMPILER) -c -o $@ $< $(FLAGS) $(LIBS) 


serialize_tests: serialize_tests.o $(PNG_DEP_FOLDER)/lodepng.o $(DEPS)
	$(COMPILER) -o $@ $^ $(FLAGS) $(LIBS) $(SOS)

server_connection_test: master.o connection.o server_connection_test.o volume_server.o connection.o query_group.o query_manager.o query_connection.o $(PNG_DEP_FOLDER)/lodepng.o $(DEPS)
	$(COMPILER) -o $@ $^ $(FLAGS) $(LIBS) $(SOS)

.PHONY : clean
clean :
	rm -f *.o $(PROGRAMS)