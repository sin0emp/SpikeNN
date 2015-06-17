cmake_minimum_required(VERSION 2.8)

project (spikenn)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

if(WIN32)
   set(Boost_USE_STATIC_LIBS ON)
endif(WIN32)

set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS system filesystem serialization REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${Boost_INCLUDE_DIR})

add_library(spikenn SHARED src/Synapse.cpp
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

target_link_libraries(spikenn ${Boost_LIBRARIES})

if (WIN32)
	install(TARGETS spikenn DESTINATION "C:/Program Files/spikenn/lib")
	install(DIRECTORY "include" DESTINATION "C:/Program Files/spikenn")
elseif (UNIX)
	file(GLOB incFiles include/*.h)
	install(FILES ${incFiles} DESTINATION "/usr/include/spikenn")
	install(TARGETS spikenn DESTINATION "/usr/lib")
endif()