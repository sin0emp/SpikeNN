cmake_minimum_required(VERSION 2.8)

project (SpikeNN)

if(WIN32)
   set(Boost_USE_STATIC_LIBS ON)
endif(WIN32)

find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${Boost_INCLUDE_DIR})

add_library(SpikeNN SHARED src/Synapse.cpp
                           src/Neuron.cpp
                           src/Network.cpp
                           src/VisualNetwork.cpp
                           src/Logger.cpp
                           src/Layer.cpp
                           src/IzhikevichNeuron.cpp
                           src/GlobalVars.cpp
                           src/EasyBMP.cpp
                           src/DAHandler.cpp)

target_link_libraries(SpikeNN ${Boost_LIBRARIES})