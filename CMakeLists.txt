cmake_minimum_required(VERSION 3.0)

project(
    path_tracer
    DESCRIPTION "Ray Tracing in One Weekend"
    LANGUAGES CXX
)

find_package(Boost REQUIRED COMPONENTS program_options serialization mpi)
find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_executable(
    path_tracer
    ${SOURCE_DIR}/main.cpp
)

target_include_directories(
    path_tracer PRIVATE
    ${INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OpenCL_INCLUDE_DIRS}
)

target_link_libraries(
    path_tracer PRIVATE
    Boost::program_options
    Boost::serialization
    Boost::mpi
    OpenMP::OpenMP_CXX
    MPI::MPI_CXX
)

if(CONSTEXPR)
	target_compile_definitions(
        path_tracer PRIVATE
        CONSTEXPR=constexpr
        IS_CONSTANT_EVALUATED=true
    )
else()
    target_compile_definitions(
        path_tracer PRIVATE
        CONSTEXPR=;
        IS_CONSTANT_EVALUATED=false
    )
endif()

target_compile_definitions(
    path_tracer PRIVATE
    IMAGE_WIDTH=${IMAGE_WIDTH}
    IMAGE_HEIGHT=${IMAGE_HEIGHT}
    PATCH_WIDTH=${PATCH_WIDTH}
    PATCH_HEIGHT=${PATCH_HEIGHT}
    PATCH_COORD_X=${PATCH_COORD_X}
    PATCH_COORD_Y=${PATCH_COORD_Y}
    TERMINATION_PROB=${TERMINATION_PROB}
    NUM_SAMPLES=${NUM_SAMPLES}
    RANDOM_SEED=${RANDOM_SEED}
)

math(EXPR FCONSTEXPR_OPS_LIMIT "(1 << 32) - 1")
target_compile_options(
    path_tracer PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native>
    $<$<BOOL:${CONSTEXPR}>:-fconstexpr-ops-limit=${FCONSTEXPR_OPS_LIMIT}>
)
target_compile_features(
    path_tracer PRIVATE
    cxx_std_20
)
