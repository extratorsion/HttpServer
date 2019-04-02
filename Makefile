CFLAG=-pthread -std=c++1z

server: server.cpp cgihandler.cpp request.cpp response.cpp threadpool.cpp mstring.cpp
	g++ -o $@ ${CFLAG} $^

%: %.cpp
	g++ -o $@ ${CFLAG} $<


