cmake_minimum_required(VERSION 3.0)

project(
    pbpt
    DESCRIPTION "PBPT: Physically-Based Path Tracer"
    LANGUAGES CXX
)

find_package(Boost REQUIRED COMPONENTS program_options serialization mpi)
find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_executable(
    pbpt
    ${SOURCE_DIR}/main.cpp
)

target_include_directories(
    pbpt PRIVATE
    ${INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(
    pbpt PRIVATE
    Boost::program_options
    Boost::serialization
    Boost::mpi
    MPI::MPI_CXX
    OpenMP::OpenMP_CXX
)

target_compile_options(
    pbpt PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native>
)
target_compile_features(
    pbpt PRIVATE
    cxx_std_20
)
