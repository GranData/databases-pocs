cmake_minimum_required(VERSION 3.5)
project(SimpleDb)

# set(LLVM_DIR /opt/llvm-3.6.2/share/llvm/cmake)

# -------------------------------- LLVM ----------------------------------------
find_package(LLVM REQUIRED)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})
# CMake version checking does not work for the LLVM package for some
# reason, so we do it by hand
message(STATUS "LLVM: ${LLVM_PACKAGE_VERSION} on ${LLVM_INSTALL_PREFIX}")
llvm_map_components_to_libnames(LLVM_LIBS mcjit native)

# LLVM has these annoying headers that won't compile without this flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")


#-------------------------------------------------------------------------------
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp SumOperator.cpp LLVMExecutor.cpp SimpleIRBuilder.cpp)
add_executable(SimpleDb ${SOURCE_FILES})

# ${CMAKE_THREAD_LIBS_INIT}
target_link_libraries(SimpleDb ${LLVM_LIBS})