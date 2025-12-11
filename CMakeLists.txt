cmake_minimum_required (VERSION 2.8.12)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
if (NOT "$ENV{CC}" STREQUAL "")
    set(CMAKE_C_COMPILER "$ENV{CC}")
endif()
if (NOT "$ENV{CXX}" STREQUAL "")
    set(CMAKE_CXX_COMPILER "$ENV{CXX}")
endif()

project (triggergraphs)
include(ExternalProject)
set(CMAKE_MACOSX_RPATH 1)

# nanobind
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(nanobind CONFIG REQUIRED)

#Add homebrew include and libs
EXECUTE_PROCESS(COMMAND brew --prefix RESULT_VARIABLE exitstatus OUTPUT_VARIABLE BREW_PREFIX ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
IF (${exitstatus} EQUAL 0)
    LINK_DIRECTORIES(${BREW_PREFIX}/lib)
    INCLUDE_DIRECTORIES(${BREW_PREFIX}/include)
ENDIF()

#Set to release mode if nothing is specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

#add files to compile
file(GLOB main_SRC
    "src/cplusplus/*.cpp"
    )

#ZLIB
find_package(ZLIB REQUIRED)

#Trident
IF(GLOG_LIB)
    message("Using KOGNAC provided installation: ${KOGNAC_LIB} ${KOGNAC_INC}")

    include_directories(${KOGNAC_INC})
    LINK_DIRECTORIES("${KOGNAC_LIB}")
    add_library(kognac-core SHARED IMPORTED)
    set_property(TARGET kognac-core PROPERTY IMPORTED_LOCATION ${KOGNAC_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}kognac-core${CMAKE_SHARED_LIBRARY_SUFFIX})

    message("Using Trident provided installation: ${TRIDENT_LIB} ${TRIDENT_INC}")
    include_directories(${TRIDENT_INC})
    include_directories(${TRIDENT_INC}/layers)
    include_directories(${TRIDENT_INC}/../rdf3x/include)
    add_library(trident-core SHARED IMPORTED)
    set_property(TARGET trident-core PROPERTY IMPORTED_LOCATION ${TRIDENT_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}trident-core${CMAKE_SHARED_LIBRARY_SUFFIX})
    add_library(trident-sparql SHARED IMPORTED)
    set_property(TARGET trident-sparql PROPERTY IMPORTED_LOCATION ${TRIDENT_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}trident-sparql${CMAKE_SHARED_LIBRARY_SUFFIX})
    LINK_DIRECTORIES("${TRIDENT_LIB}/")

    message("Using GLog provided installation: ${GLOG_LIB} ${GLOG_INC}")
    include_directories(${GLOG_INC})
    add_library(glog-core SHARED IMPORTED)
    set_property(TARGET glog-core PROPERTY IMPORTED_LOCATION ${GLOG_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}vlog-core${CMAKE_SHARED_LIBRARY_SUFFIX})
    LINK_DIRECTORIES("${GLOG_LIB}/")

ELSE()
    set(GLOG_LIB ${CMAKE_BINARY_DIR}/glog)
    set(KOGNAC_LIB ${CMAKE_BINARY_DIR}/glog/trident/kognac)
    set(TRIDENT_LIB ${CMAKE_BINARY_DIR}/glog/trident)

    ExternalProject_Add(git-glog
        GIT_REPOSITORY https://github.com/karmaresearch/glog.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CMAKE_BINARY_DIR}/glog
        BUILD_IN_SOURCE 1
        CMAKE_GENERATOR "Unix Makefiles"
        CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        BUILD_COMMAND ${CMAKE_COMMAND} -E remove_directory trident && ${CMAKE_COMMAND} -E remove_directory rulemaker && ${CMAKE_COMMAND} --build .
        INSTALL_COMMAND ""
        UPDATE_COMMAND ""
        BUILD_BYPRODUCTS
            ${GLOG_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}vlog-core${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${TRIDENT_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}trident-core${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${TRIDENT_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}trident-sparql${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${KOGNAC_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}kognac-core${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${KOGNAC_LIB}/external/lz4/lib/${CMAKE_STATIC_LIBRARY_PREFIX}lz4${CMAKE_STATIC_LIBRARY_SUFFIX}
        )
    ExternalProject_Get_Property(git-glog SOURCE_DIR)
    include_directories(${SOURCE_DIR}/include)

    # KOGNAC
    add_library(kognac-core SHARED IMPORTED)
    add_dependencies(kognac-core git-glog)
    include_directories(${SOURCE_DIR}/trident/kognac/include)
    LINK_DIRECTORIES("${KOGNAC_LIB}")
    set_property(TARGET kognac-core PROPERTY IMPORTED_LOCATION ${KOGNAC_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}kognac-core${CMAKE_SHARED_LIBRARY_SUFFIX})

    # TRIDENT
    add_library(trident-core SHARED IMPORTED)
    add_dependencies(trident-core kognac-core)
    include_directories(${SOURCE_DIR}/trident/include)
    include_directories(${SOURCE_DIR}/trident/rdf3x/include)
    set_property(TARGET trident-core PROPERTY IMPORTED_LOCATION "${TRIDENT_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}trident-core${CMAKE_SHARED_LIBRARY_SUFFIX}")
    LINK_DIRECTORIES("${TRIDENT_LIB}")

    # TRIDENT SPARQL
    add_library(trident-sparql SHARED IMPORTED)
    add_dependencies(trident-sparql trident-core)
    set_property(TARGET trident-sparql PROPERTY IMPORTED_LOCATION "${TRIDENT_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}trident-sparql${CMAKE_SHARED_LIBRARY_SUFFIX}")

    # GLOG CORE
    add_library(glog-core SHARED IMPORTED)
    add_dependencies(glog-core trident-sparql)
    set_property(TARGET glog-core PROPERTY IMPORTED_LOCATION "${GLOG_LIB}/${CMAKE_SHARED_LIBRARY_PREFIX}vlog-core${CMAKE_SHARED_LIBRARY_SUFFIX}")
ENDIF()

#SPARSEHASH
find_path(sparsehash sparsehash/sparse_hash_map)
IF (${sparsehash} STREQUAL "sparsehash-NOTFOUND")
    message("Could not find sparsehash. I'm going to use the version from kognac ...")
    include_directories(${KOGNAC_LIB}/external/sparsehash/src/)
ENDIF()

message("I'm going to use LZ4 the version from kognac ${KOGNAC_LIB}")
include_directories(${KOGNAC_LIB}/external/lz4/lib/)
LINK_DIRECTORIES(${KOGNAC_LIB}/external/lz4/lib)
add_library(lz4 STATIC IMPORTED)
set_property(TARGET lz4 PROPERTY IMPORTED_LOCATION ${KOGNAC_LIB}/external/lz4/lib/${CMAKE_STATIC_LIBRARY_PREFIX}lz4${CMAKE_STATIC_LIBRARY_SUFFIX})

#Set compiler options
if((${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang") OR (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"))
    set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        set(COMPILE_FLAGS "${COMPILE_FLAGS} -c -fPIC -MD -std=c++11 ${CLANG_ONLY_COMPILE_FLAGS}")
        if(LINUX)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")
        endif()
    else()
        set(COMPILE_FLAGS "${COMPILE_FLAGS} -c -fPIC -MD -std=c++11 ${GNUCXX_ONLY_COMPILE_FLAGS}")
    endif()
endif()

#standard include
include_directories(include/)

IF(NOT Python3_EXECUTABLE)
    find_package (Python3 COMPONENTS Interpreter Development NumPy) 
    IF(Python3_FOUND)
        INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
    ENDIF()
    message(STATUS "${Python3_INCLUDE_DIRS} ${Python3_LIBRARIES}")
ELSE()
   INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
ENDIF()

nanobind_add_module(glog MODULE ${main_SRC})
set_target_properties(glog PROPERTIES CXX_STANDARD 11)

target_link_libraries(glog
    PRIVATE
        glog-core
        trident-core
        kognac-core
        trident-sparql
        lz4
        ZLIB::ZLIB
)

IF(NOT GLOG_LIB)
    add_dependencies(glog git-glog)

    install(FILES
        ${GLOG_LIB}/libvlog-core.so
        ${TRIDENT_LIB}/libtrident-core.so
        ${TRIDENT_LIB}/libtrident-sparql.so
        ${KOGNAC_LIB}/libkognac-core.so
        DESTINATION triggergraphs)

    set_target_properties(glog PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE)
ELSE()
    set_target_properties(glog PROPERTIES
        INSTALL_RPATH "${GLOG_LIB}:${TRIDENT_LIB}:${KOGNAC_LIB}"
        BUILD_WITH_INSTALL_RPATH TRUE)
ENDIF()

install(TARGETS glog LIBRARY DESTINATION triggergraphs)