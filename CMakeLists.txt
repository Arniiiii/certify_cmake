cmake_minimum_required(VERSION 3.8)

project(
  certify
  VERSION 0.2
  LANGUAGES CXX)

if(${${PROJECT_NAME}_TEST_INSTALLED_VERSION})
  add_subdirectory(tests)
  return()
endif()
# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS
      "${PROJECT_NAME}: Setting build type to '${default_build_type}' as none was specified."
  )
  set(CMAKE_BUILD_TYPE
      "${default_build_type}"
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  set(${PROJECT_NAME}_DEFAULT_VAL ON)
else()
  set(${PROJECT_NAME}_DEFAULT_VAL OFF)
endif()

option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples?"
       ${${PROJECT_NAME}_DEFAULT_VAL})
option(${PROJECT_NAME}_BUILD_TESTS "Build tests?"
       ${${PROJECT_NAME}_DEFAULT_VAL})

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

target_include_directories(
  ${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>)

if(MSVC)
  target_link_libraries(${PROJECT_NAME} INTERFACE Crypt32.lib)
endif()

if(APPLE)
  target_link_libraries(
    ${PROJECT_NAME} INTERFACE "-framework CoreFoundation" ${PROJECT_NAME}
                              "-framework Security")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS
                                                   "-Wl,-F/Library/Frameworks")
endif()

include(./cmake/getCPM.cmake)

set(TRY_BOOST_VERSION "1.85.0")
set(BOOST_NOT_HEADER_ONLY_COMPONENTS_THAT_YOU_NEED "system;filesystem")
set(BOOST_HEADER_ONLY_COMPONENTS_THAT_YOU_NEED "date_time;core")
# set(ADDBOOSTCMAKE_LINK_TYPE "PUBLIC")

include(./cmake/add_boost.cmake)

add_boost(TRY_BOOST_VERSION BOOST_HEADER_ONLY_COMPONENTS_THAT_YOU_NEED
          BOOST_NOT_HEADER_ONLY_COMPONENTS_THAT_YOU_NEED ${PROJECT_NAME})

find_package(Threads)

find_package(OpenSSL REQUIRED)

target_link_libraries(
  ${PROJECT_NAME}
  INTERFACE # Boost::system Boost::filesystem Boost::date_time
            Threads::Threads OpenSSL::SSL OpenSSL::Crypto OpenSSL::applink)

# ======================== INSTALL ==========================

string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

cpmaddpackage(
  NAME PackageProject.cmake GIT_REPOSITORY
  https://github.com/Arniiiii/PackageProject.cmake.git GIT_TAG master)
packageproject(
  # the name of the target to export
  NAME
  ${PROJECT_NAME} # the version of the target to export
  VERSION
  ${PROJECT_VERSION} # a temporary directory to create the config files
  BINARY_DIR
  ${PROJECT_BINARY_DIR} # location of the target's public headers
  INCLUDE_DIR
  ${PROJECT_SOURCE_DIR}/include # should match the target's INSTALL_INTERFACE
                                # include directory
  INCLUDE_DESTINATION
  include/${PROJECT_NAME}
  # dir for .cmake files related to scripts for finding targets from the project
  # . We need to change it because it's not multilib and is header-only lib.
  INSTALL_CMAKEDIR
  share/${PROJECT_NAME}/cmake
  # (optional) option to install only header files with matching pattern
  # INCLUDE_HEADER_PATTERN "*.h" semicolon separated list of the project's
  # dependencies DEPENDENCIES "fmt 7.1.3;cxxopts 2.2.0" (optional) create a
  # header containing the version info Note: that the path to headers should be
  # lowercase
  VERSION_HEADER
  "${PROJECT_NAME}/version.h"
  # (optional) create a export header using GenerateExportHeader module
  EXPORT_HEADER
  "${PROJECT_NAME}/export.h"
  # (optional) install your library with a namespace (Note: do NOT add extra
  # '::') NAMESPACE ${PROJECT_NAMESPACE} (optional) define the project's version
  # compatibility, defaults to `AnyNewerVersion` supported values:
  # `AnyNewerVersion|SameMajorVersion|SameMinorVersion|ExactVersion`
  COMPATIBILITY
  AnyNewerVersion
  # (optional) option to disable the versioning of install destinations
  DISABLE_VERSION_SUFFIX
  YES
  # (optional) option to ignore target architecture for package resolution
  # defaults to YES for header only (i.e. INTERFACE) libraries
  ARCH_INDEPENDENT
  YES
  # (optional) option to generate CPack variables
  CPACK
  YES
  DEPENDENCIES
  "${ADDBOOSTCMAKE_PACKAGEPROJECT_INSTALL_TARGETS};OpenSSL 3.0")

if(${${PROJECT_NAME}_BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()

if(${${PROJECT_NAME}_BUILD_TESTS})
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
