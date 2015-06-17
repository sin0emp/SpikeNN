cmake_minimum_required(VERSION 2.8)

project (SpikeNN)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

if(WIN32)
   set(Boost_USE_STATIC_LIBS ON)
endif(WIN32)

set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS system filesystem serialization REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${Boost_INCLUDE_DIR})

add_library(SpikeNN SHARED src/Synapse.cpp
                           src/Neuron.cpp
                           src/Network.cpp
                           src/VisualNetwork.cpp
                           src/Logger.cpp
                           src/Layer.cpp
                           src/IzhikevichNeuron.cpp
                           src/LIFNeuron.cpp
                           src/GlobalVars.cpp
                           src/EasyBMP.cpp
                           src/DAHandler.cpp)

target_link_libraries(SpikeNN ${Boost_LIBRARIES})

if (WIN32)
	install(TARGETS SpikeNN DESTINATION "C:/Program Files/SpikeNN/lib")
	install(DIRECTORY "include" DESTINATION "C:/Program Files/SpikeNN")
elseif (UNIX)
	file(GLOB incFiles include/*.h)
	install(FILES ${incFiles} DESTINATION "/usr/local/include/SpikeNN")
	install(TARGETS SpikeNN DESTINATION "/usr/local/lib")
endif()