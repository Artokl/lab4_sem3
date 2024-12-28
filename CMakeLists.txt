cmake_minimum_required(VERSION 3.28)
project(lab_4)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(lab_4 main.cpp
        Iterator.h
        DynamicArray.h
        Graph.h
        Path.h
        Tests.h
        Vertex.h
        Edge.h
        Graph.cpp
        Graph.h

)

target_link_libraries(lab_4 PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)