cmake_minimum_required(VERSION 3.28)
project(TTScalProdOpt)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the main source directory
set(SRC_DIR src)
set(TOOLS_DIR tools)

# Setup the main executable
add_executable(TTScalProdOpt ${SRC_DIR}/Main.cpp ${TOOLS_DIR}/Argparser/Argparser.cpp ${TOOLS_DIR}/Argparser/Splitter.cpp)
target_include_directories(TTScalProdOpt PRIVATE ${SRC_DIR}/Components ${SRC_DIR}/CotengraWrapper ${SRC_DIR}/SweepingOpt ${SRC_DIR}/DeltaOpt ${TOOLS_DIR}/Argparser extern/pybind11/include)

# Link to Python library
find_package(Python COMPONENTS Interpreter Development)

message("Python_FOUND:${Python_FOUND}")
message("Python_VERSION:${Python_VERSION}")
message("Python_Development_FOUND:${Python_Development_FOUND}")
message("Python_LIBRARIES:${Python_LIBRARIES}")
target_include_directories(TTScalProdOpt PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries(TTScalProdOpt ${Python_LIBRARIES} pthread dl util m)

# Set up validation executable
add_executable(Compute ${TOOLS_DIR}/Compute/Main.cpp)
target_include_directories(Compute PRIVATE ${TOOLS_DIR}/Compute ${SRC_DIR}/Components)