cmake_minimum_required (VERSION 3.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wno-write-strings -I/usr/include/mysql -lmysqlclient -I/mnt/hgfs/WSProject/WSSockServer/protobuf-3.3.0/src")
INCLUDE_DIRECTORIES(/mnt/hgfs/WSProject/WSSockServer/protobuf-3.3.0/cmake/Build_Return_linux)
LINK_DIRECTORIES(/mnt/hgfs/WSProject/WSSockServer/protobuf-3.3.0/cmake/Build_Return_linux)
project(WSSockServer)
ADD_EXECUTABLE(WSSockServer LogManager.cpp FileManager.cpp SqlManager.cpp EPOLL.cpp stdafx.cpp WSSockServer.cpp PacketManager.cpp InGame.cpp Player.cpp WSSockServer.pb.cc)
TARGET_LINK_LIBRARIES(WSSockServer libgmock_main.a libprotobuf.a libprotobuf-lite.a libprotoc.a)
