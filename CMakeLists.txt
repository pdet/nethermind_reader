cmake_minimum_required(VERSION 3.10)
project(ReadRocksDB)

set(CMAKE_CXX_STANDARD 17)

# Specify the path to the RocksDB installation
set(ROCKSDB_DIR "/opt/homebrew/Cellar/rocksdb/9.3.1")

# Find Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_definitions(-DBOOST_ALL_DYN_LINK)
endif()

# Include RocksDB headers
include_directories(${ROCKSDB_DIR}/include)

# Find RocksDB library
find_library(ROCKSDB_LIB rocksdb HINTS ${ROCKSDB_DIR}/lib)

# Specify the source file
set(SOURCE_FILES main.cpp Common.cpp CommonData.cpp FixedHash.cpp main.cpp rlp.cpp)

# Add the executable
add_executable(read_rocksdb ${SOURCE_FILES})

# Link against RocksDB and its dependencies
target_link_libraries(read_rocksdb ${ROCKSDB_LIB} ${Boost_LIBRARIES})
