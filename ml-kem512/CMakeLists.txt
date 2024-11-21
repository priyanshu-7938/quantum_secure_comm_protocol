cmake_minimum_required(VERSION 3.10)
project(ML_KEM_512 C)
   
set(CMAKE_C_STANDARD 11)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/internal/PRG)
include_directories(${CMAKE_SOURCE_DIR}/internal/utils)
include_directories(${CMAKE_SOURCE_DIR}/internal/k_pke)

include_directories(${CMAKE_SOURCE_DIR}/external)

# Add source files
set(SOURCES
    ml_kem_512.c
    internal/PRG/prg.c
    internal/k_pke/k_pke.c
    internal/k_pke/k_pke_utils.c
    internal/utils/utils.c
    
    external/Compact_FIPS202.c
    external/aes.c
)

# Create executable
add_executable(ml_kem_512_keygen ${SOURCES})

# Link math library if needed
# target_link_libraries(ml_kem_512_keygen m)
