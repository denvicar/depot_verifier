cmake_minimum_required(VERSION 3.21)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/extern/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(
    DepotVerifier
    VERSION 1.0
    LANGUAGES CXX C
)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
set(Protobuf_USE_STATIC_LIBS ON)
set(OPENSSL_USE_STATIC_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(exe_sources depot_verifier.cpp

acf/acf.cpp
hash/hash.cpp
manifest/StructReader.cpp
manifest/DepotManifest.cpp

utils/util.cpp)

set(headers
acf/acf.h
hash/hash.h
manifest/DepotFile.h
manifest/DepotManifest.h
manifest/StructReader.h
utils/util.h
)

add_executable(depot_verifier 
${exe_sources}

protobuf/content_manifest.pb.cc
)

set_property(TARGET depot_verifier PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


target_compile_features(depot_verifier PRIVATE cxx_std_17)
target_include_directories(depot_verifier PRIVATE acf hash manifest protobuf utils)

find_package(OpenSSL REQUIRED)
find_package(protobuf CONFIG REQUIRED)

target_link_libraries(depot_verifier PRIVATE OpenSSL::SSL protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

if (NOT ${PROJECT_NAME}_CLANG_FORMAT_BINARY)
  find_program(${PROJECT_NAME}_CLANG_FORMAT_BINARY clang-format)
endif()

if (${PROJECT_NAME}_CLANG_FORMAT_BINARY)
  add_custom_target(format
    COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY}
    -i ${exe_sources} ${headers}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()

if(WIN32)
  add_custom_command(
    TARGET depot_verifier POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${PROJECT_SOURCE_DIR}/depot_verifier.ini"
    "$<TARGET_FILE_DIR:depot_verifier>"
    VERBATIM
  )
endif()


