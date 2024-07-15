add_rules("mode.debug", "mode.release")
add_includedirs("extern/pybind11/include")
add_includedirs("/usr/include/python3.11")
add_links("python3.11")

target("OneSidedOneDim")
    set_kind("binary")
    add_files("src/OneSidedOneDim/*.cpp", "src/Components/*.cpp")

target("TwoSidedDeltaDim")
    set_kind("binary")
    add_files("src/TwoSidedDeltaDim/*.cpp", "src/Components/*.cpp")

target("CotengraWrapper")
    set_kind("binary")
    add_files("src/CotengraWrapper/*.cpp", "src/Components/*.cpp")

target("Compute")
    set_kind("binary")
    add_files("tools/Compute/*.cpp", "src/Components/*.cpp")

target("OptiTenseurs")
    set_kind("binary")
    add_files("src/**/*.cpp", "tools/Argparser/*.cpp")
    remove_files("src/**/Main.cpp|src/Main.cpp")
    add_files("src/Main.cpp")
