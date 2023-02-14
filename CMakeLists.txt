cmake_minimum_required(VERSION 3.16)

include_directories(/include/CLI11)

project(OptiTenseurs)

set(TOOLS
    tools/Display/Display.cpp
    tools/Sort/Sort.cpp
    )

set(COMPONENTS
    src/Components/*)
    
#SolverMatrix
set(MATRIX src/Matrice/)

#SolverTS
set(TS src/TriScore/)

#SolverMTS
set(MTS src/TriScoreMargin/)

#SolverNTS
set(NTS src/TriScoreNaive/)

#OneSideDimByDim
set(ONESIDEDBD src/OneSideDimByDim/)

#SolverSplit
set(SPLIT src/Split/)

#SplitsDimByDim
set(SPLITSDBD src/SplitsDimByDim/)

#SolverESplit
set(ESPLIT src/EdgeSplit/)

#SolverSimpleG
set(SIMPLEGREEDY src/SimpleGreedy/)

#InitInstances
set(INITINSTANCES tools/InstancesGenerator/)

#Compute
set(COMPUTE tools/Compute/)


set(ALGORITHMS
    ${MATRIX}MatrixSolver.cpp 
    ${TS}TriScore.cpp
    ${MTS}TriScoreM.cpp
    ${NTS}TriScoreN.cpp
    ${SPLITSDBD}SplitsDimByDim.cpp 
    ${SPLIT}SplitSolver.cpp
    ${ESPLIT}EdgeSplit.cpp
    ${SIDE}SideExSolver.cpp
    ${SIMPLEGREEDY}SimpleG.cpp
    )

add_executable(SolverMatrix ${MATRIX}MatrixSolver.cpp ${MATRIX}Main.cpp ${TOOLS})

add_executable(SolverSimpleG ${SIMPLEGREEDY}SimpleG.cpp ${SIMPLEGREEDY}Main.cpp ${TOOLS})

add_executable(SolverSplit ${SPLIT}SplitSolver.cpp ${SPLIT}Main.cpp ${TOOLS})

add_executable(SolverVSplit ${SPLITSDBD}SplitsDimByDim.cpp ${SPLITSDBD}Main.cpp ${SPLIT}SplitSolver.cpp ${TOOLS})
add_executable(SolverESplit ${ESPLIT}EdgeSplit.cpp ${ESPLIT}Main.cpp ${TS}TriScore.cpp ${TOOLS})

add_executable(SolverSide ${ONESIDEDBD}SideExSolver.cpp ${ONESIDEDBD}Main.cpp ${TOOLS})

add_executable(SolverTS ${TS}TriScore.cpp ${TS}Main.cpp ${TOOLS})

add_executable(SolverMTS ${MTS}TriScoreM.cpp ${MTS}Main.cpp ${TS}TriScore.cpp ${TOOLS})

add_executable(SolverNTS ${NTS}TriScoreN.cpp ${NTS}Main.cpp ${TOOLS})

#find_package(Threads)
#add_executable(Solver ../src/Main.cpp ${ALGORITHMS} ${TOOLS})
#target_link_libraries(Solver ${CMAKE_THREAD_LIBS_INIT})
#add_executable(InitInstances ${INITINSTANCES}InstanceGenerator.cpp)
#add_executable(Compute ${COMPUTE}Test.cpp ${TOOLS})
