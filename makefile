
GTKLINKFLAGS=$(shell pkg-config --libs gtkmm-3.0)
GTKCOMPILEFLAGS=$(shell pkg-config --cflags gtkmm-3.0)

CXXFLAGS+= -DASIO_STANDALONE -Wall -O0 -g -std=c++11
CPPFLAGS+= ${GTKCOMPILEFLAGS} -I./include -I./asio-1.12.2/include -I./src
LDLIBS+= -lpthread ${GTKLINKFLAGS}


TARGETS=asio-1.12.2 chat_client chat_server

all:${TARGETS}

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

chat_client:./src/chat_client.cpp  ./src/player.cpp
	${CXX} -o $@ $^  ${CXXFLAGS} ${CPPFLAGS} ${GTKCOMPILEFLAGS} ${LDLIBS} ${GTKLINKFLAGS}

chat_server:./src/chat_server.cpp  ./src/player.cpp
	${CXX} ${CXXFLAGS} ${CPPFLAGS} -o $@ $< -lpthread

clean:
	-rm -rf asio-1.12.2
	-rm -f chat_client
	-rm -f chat_server
