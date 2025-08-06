CXX = g++
CXXFLAGS = -Wall -std=c++11

CLIENT_SRC = client/client.cpp
SERVER_SRC = server/server.cpp

CLIENT_BIN = client/client
SERVER_BIN = server/server

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)
