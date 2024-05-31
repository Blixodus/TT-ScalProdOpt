add_rules("mode.debug", "mode.release")
add_includedirs("extern/pybind11/include")
add_includedirs("/usr/include/python3.11")
add_links("python3.11")

target("GreedyEdgeSort")
    set_kind("binary")
    add_files("src/Components/*.cpp", "src/GreedyEdgeSort/*.cpp", "tools/Compute/Compute.cpp")

target("AllEdgeByEdge")
    set_kind("binary")
    add_files("src/AllEdgeByEdge/*.cpp", "src/Components/*.cpp", "tools/Compute/Compute.cpp")

target("AllSplits")
    set_kind("binary")
    add_files("src/AllSplits/*.cpp", "src/Components/*.cpp", "tools/Compute/Compute.cpp")
    
target("ConvexSplits")
    set_kind("binary")
    add_files("src/ConvexSplits/*.cpp", "src/Components/*.cpp", "tools/Compute/Compute.cpp")

target("OneSideDimByDim")
    set_kind("binary")
    add_files("src/OneSideDimByDim/*.cpp", "src/Components/*.cpp", "tools/Compute/Compute.cpp")

target("SplitsDimByDim")
    set_kind("binary")
    add_files("src/SplitsDimByDim/*.cpp", "src/Components/*.cpp", "tools/Compute/Compute.cpp", "src/CotengraOptimalWrapper/CotengraOptimalWrapper.cpp")

target("CotengraOptimalWrapper")
    set_kind("binary")
    add_files("src/CotengraOptimalWrapper/*.cpp", "src/Components/*.cpp", "tools/Compute/Compute.cpp")

target("CotengraOptimalWrapper")
    set_kind("binary")
    add_files("src/CotengraOptimalWrapper/*.cpp", "src/Components/*.cpp", "tools/Compute/Compute.cpp")

target("OptiTenseurs")
    set_kind("binary")
    add_files("src/**/*.cpp", "tools/Compute/Compute.cpp", "tools/Argparser/*.cpp", "tools/Export/Export.cpp")
    remove_files("src/**/Main.cpp|src/Main.cpp")
    add_files("src/Main.cpp")
