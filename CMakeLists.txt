cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)  # Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()


# Cuda



#SMFL
set(SFML_DIR "${CMAKE_SOURCE_DIR}/c_lib/SFML/SFML-2.6.1/lib/cmake/SFML")
set(SFML_LIBRARIES"${CMAKE_SOURCE_DIR}/c_lib/SFML/lib")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/SFML/share/sfml")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/SFML/SFML-2.6.1/include")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/SFML/SFML-2.6.1/include/SFML")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/SFML/include/SFML")
find_package(SFML 2.6.1 COMPONENTS graphics window system CONFIG REQUIRED)

#CRYPTOPP
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/c_lib/cryptopp-cmake/cmake" ${CMAKE_MODULE_PATH})
add_subdirectory("${CMAKE_SOURCE_DIR}/c_lib/cryptopp-cmake")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/cryptopp-cmake/cryptopp")
set(cryptopp_DIR "${CMAKE_SOURCE_DIR}/c_lib/cryptopp-cmake/cryptopp")

#Eigen3
set(Eigen3_DIR "${CMAKE_SOURCE_DIR}/c_lib/eigen3_x64-windows/share/eigen3")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/eigen3_x64-windows/include")
find_package(Eigen3 REQUIRED)

#LSL
set(LSL_DIR "${CMAKE_SOURCE_DIR}/c_lib/liblsl_x64-windows/share/LSL")
set(LSL_LIBRARIES "${CMAKE_SOURCE_DIR}/c_lib/liblsl_x64-windows/lib/lsl.lib")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/liblsl_x64-windows/include")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/liblsl_x64-windows/include/lsl")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/liblsl_x64-windows")
include_directories(${LSL_INCLUDE_DIRS}/liblsl/include)
find_package(LSL REQUIRED)

#npcap
set(NPCAP_LIBRARIES "${CMAKE_SOURCE_DIR}/c_lib/npcap/bin/wpcap.lib")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/npcap/include")

#hidapi
set(HID_LIBRARIES "${CMAKE_SOURCE_DIR}/c_lib/hidapi-master/bin/hidapi.lib")
include_directories("${CMAKE_SOURCE_DIR}/c_lib/hidapi-master/include")


#Resource folder
file(COPY "${CMAKE_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}")


include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(APPEND SOURCES "src/main.cpp")
add_executable(Kerrigan2 ${SOURCES})
include_directories(${CUDA_INCLUDE_DIRS})
target_link_libraries(Kerrigan2 sfml-graphics sfml-window sfml-system ${LSL_LIBRARIES} ${HID_LIBRARIES} ${NPCAP_LIBRARIES} cryptopp::cryptopp ${CUDA_LIBRARIES})
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

