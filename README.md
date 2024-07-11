# Exploring optimal contraction strategies for fast vector scalar product in Tensor-Train format

# Algorithms


## Tensor-Train network format

## Contraction orderding format
In the implementation of the $1-sided$ $1-dim$ and $2-sided$ $\Delta-dim$ algorithms, we have opted for the recursive definition of the final contraction order, as it provides an elegant and unambiguous way to define the order of each contraction and resulting tensor.

![Contraction ordering example](./docs/Ordering_example.png)

For example for contraction presented above, the order returned by our algorithm would be defined as `((((0, 1), (3, 4)), 2), 5)`, which can be represented in form of list of nodes to be contracted as `[(0, 1), (3, 4), (0, 3), (0, 2), (0, 5)]` (assuming that node with lower id represents the result of each contraction). 

We have provided the script to flatten the recursive order definition:
`tools/flat_order.py`.


# Preparing the environment

### Cloning the repository
In order to clone the repository, use the git client of your choosing, for example:
```
https://github.com/Blixodus/OptiTenseurs.git
```

### Cloning the external dependencies
Our project relies on [pybind11](https://github.com/pybind/pybind11) library, in order to launch Python interpreter from C++ code, therefore after cloning the repository, the submodules needs to be initialized using the following commands:

```
git submodule init
git submodule update
```

### Initializing build system


### Preparing Python environment and dependencies


## Compilation
In order to compile the project along with all available algorithms and tools, use the following command:
```
xmake b
```

## Usage






# Preparing repository for usage


# OptiTenseurs
The goal of this project is to implement algorithms capable of finding the best order of contraction of any tensor train, given the size of each vertex's dimension.

# Structure

* 'results' : contains all exported results, it is the default root directory for output files.
* 'instances' : contains the network files, it is the default root directory for input files.
* 'src' : contains all the main algorithms (divided in their own sub-directories), the components that define the networks and some types, as well as the main .cpp file that will process the execution queue.
* 'tools' : contains quality-of-life programs such as the code that displays a TT, the code to test contraction orders, and the code to generate instances.
* 'GUI' : contains the python code for the graphical user interface.

# Compiling
This program is compiled using xmake, you can use the following command :  
```
mkdir [build_directory]
```  
```
xmake f -o [build_directory] 
```
(default is 'build')  
```
xmake b [target]
```  
The targets are :  

* AllEdgeByEdge  
* AllSplits  
* ConvexSplits  
* GreedyEdgeSort  
* OneSideDimByDim  
* SplitsDimByDim  
* OptiTenseurs, which is the general caller to the other algorithms

# Executing
You can run the program using  
```
xmake run -w . [target] [file_name]
``` 
(although this is deprecated and not recommanded)  
or  
```
xmake run -w . OptiTenseurs -a dictionary1 ... dictionaryN -f network_file1 ... network_fileN -o output_file
```  
Where each dictionary is a string with format : "key1 val1 key2 val2 ... keyn valn"
The keys are :  

* "main_alg" : (Algorithm name) The main solver, corresponds to the target and .cpp name (default : none)
* "dmin" : (integer) Minimum size in number of vertex, edge, or dimension (depends on the main algorithm) of the network the algorithm can handle, before delegating to sub_alg. (default : 1) (Not implemented) 
* "dmax" : (integer) Limiting factor in number of vertex, edge, or dimension (depends on the main algorithm). Implementation varies for each algorithm. Limits the size of the sub-networks that can be considered/generated by the algorithm, the accumulation of edges (default : depends on algorithm)
* "sub_alg" : (Algorithm name) Secondary algorithm to solve networks of size < dmin. (Not implemented) (default : none)
* "start_sol" : (Algorithm name) Algorithm to initialize the solution with, for pruning. (Implemented, but not fully operational) (default : none)
* "time" : (integer) Time alloted to an algorithm for each execution (in minutes). After a timeout an algorithm is kicked out of the execution queue permanently (default : 10)
* "test" : ({0, 1}) Wheter the solution found by the algorithm should be tested. For debugging purpose, not available for every algorithm (default : 0) (only available to the algorithms which contraction order we manage to recover)

An example of a valid command can be found in exec_example.sh.

Alternatively, you can use the GUI to select the parameters and execute the code. You can also use it to generate the execution command and copy it to your clipboard.

# Networks
Networks are text files located in the 'instances' directory. They can be read by the programs in order to be solved.
They are formatted in the following way :  

* Lines starting with 'c' are comment lines, and are not taken into account by the execution
* Lines starting with 'v' are a representation of the Tensor Train
* One single line starts with 'd' followed by the dimension (width) of the TT
* Lines starting with 'e' describe edges, they read 'node1 node2 weight'

Examples of input files can be found in the 'instances/test/' directory.

# Algorithms
The code currently supports 6 algorithms.
We note D, the dimension of a network.
We note "explicit edges" the edges of the network, and "implicit edges" the edges of weight 1 that implicitly link every vertices together.

1. ### AllSplits
    * Type : exact, dynamic programming
    * Description : Solves every possible splits (over the vertices) of the network
    * Complexity : 2^4D

2. ### AllEdgeByEdge 
    * Type : theoretically exact (need proof), iterative (edges)
    * Description : Solves every contraction order of the network
    * Complexity : 
    * Remark : Unlike AllSplits which consider every splits possible, this algorithm only considers the explicit edges.

3. ### ConvexSplits
    * Type : heuristic (close to exact), dynamic programming
    * Description : Solves every possible convex splits (over the vertices) of the network
    * Complexity : 
    * dmax : Limits how many edges can be crossed when splitting
    * Remark : By virtue of being unable to generate concave sub-problem, this algorithm is not exact. However it is unlikely to find a network where the optimal solution would require a concave decomposition. Determining the best and worst network-structure for this algorithm could be interesting.

4. ### GreedyEdgeSort
    * Type : greedy, iterative (edges)
    * Description : Solves a single contraction order, sorts the edges by weight^2/contraction_cost (descending order), re-computed at each iteration.
    * Complexity : D*log(D!)
    * Remark : This algorithm attempts to minimize the overall weight of the network, while not skyrocketting the final cost. This strategy is both cheap and consistently good, which makes its solution a good starting point for any algorithm. Additionaly, a thorough analysis of the network could provide information that would allow more constraint to be added in order to approach the optimal solution.

5. ### OneSideDimByDim
    * Type : heuristic, iterative (edges + dimensions)
    * Description : Explores the solutions by going from one side of the network to the other. Contracts 2 edges in {upper, central, lower} per dimension.
    * Complexity : 
    * dmax : The amount of central edges that can be accumulated
    * Remark : Each dimension is composed of 3 edges, two lateral and one central. Once 2 edges have been contracted on that dimension, the remaining edge fuses with the next central edge. Hence why it is interesting to limit how many central edges can fuse together.

6. ### SplitsDimByDim :
    * Type : heuristic, iterative (dimensions) + dynamic programming
    * Description : Iterates over d in [1, dmax], then splits the network in 2 sub-networks of dimensions d and D-d.
    * Complexity : 
    * dmax : 

# GUI
In order to ease the execution of many algorithms with different parameters on many files, a basic Graphical User Interface is provided.  
It is recommanded to have python 3.* installed to use the GUI.  

## Launching
You can launch the gui by executing OptiGUI, or by directly using python3 GUI/main.py.

## Functionalities
The GUI provides the ability to add algorithm entries, tweak the parameters, add network files, select an output file, copy the execution command to clipboard, and launch the execution.  
It will only display the parameters available/relevant for the selected algorithms.  
Currently, launching the execution directly from the interface will lock it, hence why it may be interesting to copy the command and execute it separately.  
Additionnaly, the GUI provides a tab to import and inspect the results of an execution.

# Results

## Exporting
If [output_file] is specified, the resulting file contains for each algorithm used, their parameters, the resulting cost, the time to solve, and the network and its caracteristics. Algorithms that timed-out are omitted.  
If the output file does not exist prior to the execution, it will be created by the program. If it already exists, the new results will be added at the end of the file.  
The default delimiter is ';' and cannot easily be modified yet.  
An example of output file is provided in the 'results/' directory.

## Visualizing results
In the results visualization tab, the user can import .csv files generated by an execution, and plot them.  
2 types of line-plot are available, Cost per Network Dimension, and Time per Network Dimension.  
Each algorithm can be toggled on or off.  
Plots can be named and saved to the 'plot/' directory

# Known bugs

Bug | Occurrence | Cause
----|----|----
SplitsDimByDim returns 2^63 | Past 50 dimensions | In solve(), when the best_cost for state2 is computed.
ConvexSplits undershoots the best cost | If best_cost is initialized as something close to the max of int64 | Likely an overflow in some part of the code, because it is unlikely that a network actually lends a result greater than 2^63, it is more likely that a value is summed to best_cost at some point.

# Additional Notes
* AllSplits is slower than AllEdgeByEdge and should therefore be ignored.  
* AllEdgeByEdge should be replaced by a proper dynamic programming algorithm.  
* More naive algorithms should be implemented.  
* Results visualization does not properly support having the same algorithm executed multiple time on the same network, but could be added.  
* A slider could be added to select the range of networks to display (a zoom essentially).
* GreedyEdgeSort is a bit rough around the edges (mostly the sorting part), it has no excuse to be this slow, considering the range of solution it explores (1).