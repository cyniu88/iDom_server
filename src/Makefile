iDom_server:iDom_server.o
	$(CXX) $(CXXFLAGS) iDom_server.o -o iDom_server $(LIBS) -lpthread parser/parser.cpp serialib/serialib.cpp   parser/parser.hpp c_connection/c_connection.cpp c_connection/c_connection2.cpp c_connection/c_connection_node.cpp c_connection/c_connection.h wiadomosc/wiadomosc.h functions/functions.h functions/functions.cpp logger/logger.hpp  logger/logger.cc
iDom_server.o:iDom_server.cpp
	$(CXX) $(CXXFLAGS) -c iDom_server.cpp    parser/parser.cpp  parser/parser.hpp c_connection/c_connection.cpp functions/functions.cpp
clean:
	rm *.o iDom_server
