cmake_minimum_required(VERSION 3.12)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled main.cpp main.h util.cpp util.h model/Message.cpp model/Message.h App.cpp App.h model/Stack.cpp model/Stack.h mcast_windows.cpp stdafx.cpp stdafx.h stdafx.h targetver.h stdafx.cpp stdafx.h targetver.h)