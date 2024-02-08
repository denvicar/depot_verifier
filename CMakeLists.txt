cmake_minimum_required(VERSION 3.21)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/extern/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")

project(
    DepotVerifier
    VERSION 1.0
    LANGUAGES CXX C
)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
set(Protobuf_USE_STATIC_LIBS ON)
set(OPENSSL_USE_STATIC_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(main_sources

acf/acf.cpp
hash/hash.cpp
manifest/StructReader.cpp
manifest/DepotManifest.cpp
validation/validator.cc

utils/depot_utils.cpp)

set(headers
acf/acf.h
hash/hash.h
manifest/DepotFile.h
manifest/DepotManifest.h
manifest/StructReader.h
utils/depot_utils.h
validation/validator.h
)

find_package(OpenSSL REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

add_library(depot_verifier_lib STATIC ${main_sources} protobuf/content_manifest.pb.cc)
target_link_libraries(depot_verifier_lib PUBLIC OpenSSL::SSL protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite Boost::program_options)
target_compile_features(depot_verifier_lib PUBLIC cxx_std_17)
target_include_directories(depot_verifier_lib PUBLIC acf hash manifest protobuf utils validation)
set_property(TARGET depot_verifier_lib PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(depot_verifier depot_verifier.cpp)
target_link_libraries(depot_verifier PRIVATE depot_verifier_lib)
set_property(TARGET depot_verifier PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (NOT ${PROJECT_NAME}_CLANG_FORMAT_BINARY)
  find_program(${PROJECT_NAME}_CLANG_FORMAT_BINARY clang-format)
endif()

if (${PROJECT_NAME}_CLANG_FORMAT_BINARY)
  add_custom_target(format
    COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY}
    -i ${main_sources} ${headers} depot_verifier.cpp tests/depotverifier_test.cc
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

find_package(Catch2 CONFIG REQUIRED)
add_executable(depot_verifier_test tests/depotverifier_test.cc)
set_property(TARGET depot_verifier_test PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_link_libraries(depot_verifier_test PRIVATE Catch2::Catch2 Catch2::Catch2WithMain depot_verifier_lib)

include(CTest)
include(Catch)
catch_discover_tests(depot_verifier_test)

set(ACF_PATH "\"${PROJECT_SOURCE_DIR}/testdata/appmanifest_999999.acf\"")
set(HASH_PATH "\"${PROJECT_SOURCE_DIR}/testdata/test_hash.txt\"")
configure_file(test_config.h.in test_config.h @ONLY)
target_include_directories(depot_verifier_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR})




