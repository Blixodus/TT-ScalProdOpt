# Exploring optimal contraction strategies for fast vector scalar product in Tensor-Train format

This repository presents an implementation of the algorithms for finding quasi-optimal contraction orderings for scalar products in Tensor-Train format discussed in the paper:

```bibtex
@article{paper_name,
  author    = {author list},
  title     = {Exploring optimal contraction strategies for fast vector scalar product in Tensor-Train format},
  year      = {2024},
  journal   = {},
  doi       = {}
}
```


# Algorithms
In this project, we include the implementations of two algorithms presented in the article: **1-sided 1-dim** algorithm and **2-sided $\Delta$-dim**. Moreover we provide a wrapper for the `Cotengra` library, which allows to use the algorithms implemented in this library for the benchmarking purposes.

## 1-sided 1-dim
The **1-sided 1-dim** algorithm is an iterative algorithm, which finds a solution for a window consisting of the left-most mode contraction and rank contractions. At each step of the algorithm, only two out of the three contractions are performed, generating multiple sub-problems with different mode contraction cost.

**Time complexity:** $\mathcal{O}(N^2)$

**Memory complexity:** $\mathcal{O}(N)$

## 2-sided $\Delta$-dim
The **2-sided $\Delta$-dim algorithm** is an iterative algorithm, which finds the solutions for each window in the network using either optimal solver for windows up to $\Delta$, or by building quasi-optimal orders based on already calculated smaller windows for windows larger than $\Delta$ 


**Time complexity:** $\mathcal{O}(\Delta N^2 + \Delta N \cdot opt)$

**Memory complexity:** $\mathcal{O}(N^2)$

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

### Preparing Python environment and dependencies
[Ruche only step] Load the necessary modules.
```
module load miniconda3/24.5.0/gcc-13.2.0
module load gcc/13.2.0/gcc-4.8.5
module load cmake/3.28.3/gcc-11.2.0
```

We recommend using Conda manage Python version and necessary dependencies. You can create new Conda environment with Python 3.10.14 using the following command:
```
conda create --prefix ./tt_contr_env python=3.10.14 --channel conda-forge
source activate ./tt_contr_env
```
If you opt for using your own Python environment, make sure to include Python-dev package, as the project relies on Python.h header file and Python.so library. Moreover, make sure to install Python version 3.10.14, as newer versions cause compatibility issues with the
`kahypar` library used by `Cotengra`.

After activating the environment, install necessary Python packages.
```
conda install conda-forge::cotengra anaconda::pandas conda-forge::seaborn conda-forge::alive-progress
pip install kahypar cgreedy
```

# Compilation and usage

## Compilation
In order to compile the project along with all available algorithms and tools, use the following commands:
```
cmake -B build
cd build
make -j
```

## Usage
[Ruche only step] Load the necessary modules.
```
module load miniconda3/24.5.0/gcc-13.2.0
module load gcc/13.2.0/gcc-4.8.5
module load cmake/3.28.3/gcc-11.2.0
```

Activate the Python environment.
```
source activate ./tt_contr_env
```

Run the project with the following command:
```
./build/OptiTenseurs -a "[arguments]" -f [test case filename]"
```
where `argument` is a dictionary of parameters, which include:
* `main_alg` - the main algorithm to be used (`OneSidedOneDim`, `TwoSidedDeltaDim`, `CotengraWrapper`),
* `tt_dim` - problem type (for example 2 for $xy$ case, 3 for $xAy$ case, 4 for $xABy$ case),
* `delta` - the maximum size of the window (for $2-sided$ $\Delta-dim$ algorithm),
* `ctg_algorithm` - the algorithm from Cotengra library which should be used by our wrapper (when `main_alg` is set to `CotengraWrapper`).

### Examples
In order to run $2-sided$ $\Delta-dim$ algorithm with $\Delta=4$ on the $xx^T$ test case, use the following command:
```
./OptiTenseurs -a "main_alg TwoSidedDeltaDim tt_dim 2 delta 4" -f /path_to_tests/xxT/random/low/d_003_v001.txt
```

In order to run $Hyper-Greedy$ algorithm using Cotengra wrapper on the $xAy$ test case, use the following command:
```
./OptiTenseurs -a "main_alg CotengraWrapper tt_dim 3 ctg_algorithm hyper-greedy" -f /path_to_tests/xAy/quantized/medium/d_004_v002.txt
```

# Input and output formats
## Input: tensor-train network format
Tensor-train networks are defined as text files which use the following format:
* lines starting with 'c' are comment lines, and are not taken into account by the parser,
* lines starting with 'v' are a visual representation of the tensor-train network, and are not taken into account by the parser,
* single line starting with 't' followed by the number of rows (tt_dim) of the tensor-train,
* single line starting with 'd' followed by the dimension (width) of the tensor-train,
* lines starting with 'e' describe contraction weight: 'node1 node2 weight' (as weight of the edge between node1 and node2).

## Output: contraction orderding format
The contraction order returned by $1-sided$ $1-dim$ and $2-sided$ $\Delta-dim$ algorithms, as well as Cotengra library algorithms run using our wrapper, uses a recursive definition the contractions.

![Contraction ordering example](./docs/Ordering_example.png){: width="50%"}

For example for contractions presented above, the order returned by the algorithm would be defined as `((((0, 1), (3, 4)), 2), 5)`, which can be represented in form of list of nodes to be contracted as `[(0, 1), (3, 4), (0, 3), (0, 2), (0, 5)]` (assuming that node with lower id represents the result of each contraction). 

For convenience, we have provided the script to flatten the recursive order definition:
`tools/flat_order.py`.

# Repository Structure

* 'results' : contains all exported results, it is the default root directory for output files.
* 'instances' : contains the network files, it is the default root directory for input files.
* 'src' : contains all the main algorithms (divided in their own sub-directories), the components that define the networks and some types, as well as the main .cpp file that will process the execution queue.
* 'tools' : contains quality-of-life programs such as the code that displays a TT, the code to test contraction orders, and the code to generate instances.
* 'GUI' : contains the python code for the graphical user interface.

# Experiments

## Generating test cases

## Computing the results

## Plotting the results