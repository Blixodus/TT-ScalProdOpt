cmake_minimum_required(VERSION 3.28)
project(OptiTenseurs)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the main source directory
set(SRC_DIR src)
set(TOOLS_DIR tools)

# Setup the main executable
add_executable(OptiTenseurs ${SRC_DIR}/Main.cpp ${TOOLS_DIR}/Argparser/Argparser.cpp ${TOOLS_DIR}/Argparser/Splitter.cpp ${SRC_DIR}/OneSidedOneDim/OneSidedOneDim.cpp)
target_include_directories(OptiTenseurs PRIVATE ${SRC_DIR}/CotengraWrapper ${SRC_DIR}/OneSidedOneDim ${SRC_DIR}/TwoSidedDeltaDim ${SRC_DIR}/Components ${TOOLS_DIR}/Argparser extern/pybind11/include)

# Link to Python library
find_package(Python COMPONENTS Interpreter Development)

message("Python_FOUND:${Python_FOUND}")
message("Python_VERSION:${Python_VERSION}")
message("Python_Development_FOUND:${Python_Development_FOUND}")
message("Python_LIBRARIES:${Python_LIBRARIES}")
target_include_directories(OptiTenseurs PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries(OptiTenseurs ${Python_LIBRARIES} pthread dl util m)

# Set up validation executable
add_executable(Compute ${TOOLS_DIR}/Compute/Main.cpp)
target_include_directories(Compute PRIVATE ${TOOLS_DIR}/Compute ${SRC_DIR}/Components)