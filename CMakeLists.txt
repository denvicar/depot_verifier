cmake_minimum_required(VERSION 3.14..3.28)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/extern/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(
    DepotVerifier
    VERSION 1.0
    LANGUAGES CXX C
)

add_executable(depot_verifier 
depot_verifier.cpp

acf/acf.cpp
hash/hash.cpp
manifest/StructReader.cpp
manifest/DepotManifest.cpp

protobuf/content_manifest.pb.cc

utils/util.cpp
)

target_compile_features(depot_verifier PUBLIC cxx_std_17)
target_include_directories(depot_verifier PUBLIC acf hash manifest protobuf utils)

find_package(OpenSSL REQUIRED)
find_package(protobuf CONFIG REQUIRED)

target_link_libraries(depot_verifier PUBLIC OpenSSL::SSL protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

if(WIN32)
  add_custom_command(
    TARGET depot_verifier POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "$<TARGET_FILE:OpenSSL::SSL>"
    "$<TARGET_FILE:protobuf::libprotoc>"
    "$<TARGET_FILE:protobuf::libprotobuf>"
    "$<TARGET_FILE:protobuf::libprotobuf-lite>"
    "${PROJECT_SOURCE_DIR}/depot_verifier.ini"
    "$<TARGET_FILE_DIR:depot_verifier>"
    VERBATIM
  )
endif()


