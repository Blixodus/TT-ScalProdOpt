# Exploring optimal contraction strategies for fast scalar product in tensor-train format

## Introduction

This repository contains reference implementations of the algorithms for finding quasi-optimal contraction orderings in tensor-train scalar products presented in the paper:

```bibtex
@article{torri_ttscal_2025,
  author  = {Torri, Atte and Dominikowski, Przemysław and Pointal, Brice and Kaya, Oguz and Lima Pilla, Laercio and Coulaud, Olivier},
  title   = {Near-Optimal Contraction Strategies for the Scalar Product in the Tensor-Train Format},
  year    = {2025},
  journal = {},
  doi     = {}
}
```

The `src` directory contains the implementations of the ***Sweep-opt*** and ***$\Delta$-opt*** algorithms, which are presented in the paper. We provide a wrapper for the [`Cotengra` library](https://github.com/jcmgray/cotengra) and include a submodule that is a fork of [`RL-TNCO`](https://github.com/NVlabs/RL-TNCO), both of which offer different approaches to solving the tensor network contraction ordering problem. `Cotengra` provides a state-of-the-art implementation of the optimal algorithm, and multiple heuristics, while `RL-TNCO` is a reinforcement learning method.

## Using `RL-TNCO`

This repository contains two scripts that are useful in working with `RL-TNCO`. The first one converts from our tensor-train network format into a pickle file that `RL-TNCO` can then train or evaluate on. The second converts results from `RL-TNCO` evaluation into data usable by our plotting scripts.

### Converting to `RL-TNCO` pickle file format

### Training and evaluation in `RL-TNCO`

The training and evaluation steps are described in detail in the [fork of RL-TNCO](https://github.com/Blixodus/RL-TNCO) that we have as submodule.

### Getting results from `RL-TNCO`

## Initializing the datasets used in the paper

### Synthetic datasets

### Quantum chemistry use case datasets

To reproduce our real-world use-case datasets, which come from a quantum chemistry use-case, you must use the [ttvibr](https://bitbucket.org/rakhuba/ttvibr/) library. Getting it to work can take a bit of effort, as it uses an outdated python version and libraries. We provide below complete steps to make it work using a [conda](https://www.anaconda.com/) environment, which is provided in the `envs` directory.

```
# Create a working conda environment
conda env create -n ttvibr --file=envs/ttvibr.yml
conda activate ttvibr
# Install ttpy
git clone --recurse-submodules https://github.com/oseledets/ttpy.git
cd ttpy
git checkout v1.2.0
FFLAGS="-w -std=legacy -fPIC" python setup.py install
cd ..
# Install ttvibr
git clone https://bitbucket.org/rakhuba/ttvibr.git
cd ttvibr
chmod +x compile.sh
FFLAGS="-w -std=legacy -fPIC" ./compile.sh
cd quadgauss
FFLAGS="-w -std=legacy -fPIC" python setup.py build_ext --inplace
cd ..
# Run the jupyter notebook
jupyter-notebook iteration.ipynb
```

When you open the jupyter notebook, you need to set `eps = 1e-6` and `rmax = 5000` everywhere so as to limit the ranks only by the defined precision and not artificially. You can then run all kernels. The values of `psi_new`, `A0`, and `Apsi_new` in the first iteration of the "Harmonic + Henon-Heiles correction" method was used in our paper as the real-world use-case. These values can then be converted to our data format for tensor-train networks.




### 1-sided 1-dim
The **1-sided 1-dim** algorithm is an iterative algorithm, which finds a solution for a window consisting of the left-most mode contraction and rank contractions. At each step of the algorithm, only two out of the three contractions are performed, generating multiple sub-problems with different mode contraction cost.

**Time complexity:** $\mathcal{O}(N^2)$

**Memory complexity:** $\mathcal{O}(N)$

### 2-sided $\Delta$-dim
The **2-sided $\Delta$-dim algorithm** is an iterative algorithm, which finds the solutions for each window in the network using either optimal solver for windows up to $\Delta$, or by building quasi-optimal orders based on already calculated smaller windows for windows larger than $\Delta$ 


**Time complexity:** $\mathcal{O}(\Delta N^2 + \Delta N \cdot opt)$

**Memory complexity:** $\mathcal{O}(N^2)$

## Repository Structure
* `src/Main.cpp` - file with entry point to the application that process runtime arguments and executes the algorithms,
* `src/[Algorithm_name]` : contain sthe implementation of the algorithms, each placed in their respective sub-directory, the components that define the networks and some types,
* `src/Components` : contains the implementation of the components that define the  tensor-train network and types,
* `tools` : contains scripts and tools that are used to generate test cases, perform benchmarks and plot the results.
* `tools/configs` : contains the configuration files for the benchmarking scripts,
* `extern` : contains the external libraries and binaries used by the project.
* `build` : contains the compiled binaries and object files.

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

### Legacy Cotengra environment
To use `quickbb` and `flowcutter` algorithms from Cotengra library, you need to setup another environment, with older version of cotengra library.
```
conda create --prefix ./tt_contr_env_legacy python=3.10.14 --channel conda-forge
source activate ./tt_contr_env_legacy
pip install cotengra==0.2.0
pip install networkx
conda install anaconda::pandas conda-forge::seaborn conda-forge::alive-progress
pip install kahypar cgreedy
```

Then you should compile the `quickbb` and `flowcutter` libraries and move the binaries to `extern` directory.
* https://web.archive.org/web/20171001202810/http://www.hlt.utdallas.edu/~vgogate/quickbb_64
* https://github.com/kit-algo/flow-cutter-pace17

Finally, you should add `extern` directory to your `PATH` environment variable.
```
export PATH=$PATH:/[repository path]/extern
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


## Experiments
We provide a set of scripts that allow to reproduce the experiments which we have performed in the paper. The scripts are used to generate the test cases, run the benchmarks on specified algorithms, and plot the results.

The scripts are located in `tools` directory and they base on the common configuration files, which are located in `tools/configs` directory. For the purpose of the experiments, we have prepared the following configuration files:
* `test_cases_dim2.ini` - contains the configuration for the test cases with dimensions equal to 2 (for cases when dim is constant),
* `test_cases_dim50.ini` - contains the configuration for the test cases with dimensions equal to 50 (for cases when dim is constant),
* `test_cases_xABY.ini` - contains the configuration for the test cases of type $xABy$.
The example of the configuration file with the explanation of each parameter is provided in the `tools/configs/config_example.ini` file.

You may adjust the configuration files to your needs, for example by changing number of cores, algorithms to be used, or the number of test cases.
For task-based computing environment, you may split the job into smaller chunks, using one config file for each algorithm, and run the benchmarks in parallel. The example of such configuration files is provided in `tools/configs/dim2` directory.

### Generating test cases
In order to generate the test cases, use the following command:
```
python tools/generate_tests.py [config file]
```

### Computing the results
In order to compute the results, use the following command:
```
python tools/compute_tests.py [config file]
```

### Plotting the results
In order to plot the results, use the following command:
```
python tools/plot_tests.py [config file]
```

# Technical considerations

## Input tensor-train network format
Tensor-train networks are defined as text files which use the following format:
* lines starting with 'c' are comment lines, and are not taken into account by the parser,
* lines starting with 'v' are a visual representation of the tensor-train network, and are not taken into account by the parser,
* single line starting with 't' followed by the number of rows (tt_dim) of the tensor-train,
* single line starting with 'd' followed by the dimension (width) of the tensor-train,
* lines starting with 'e' describe contraction weight: 'node1 node2 weight' (as weight of the edge between node1 and node2).

## Output contraction orderding format
The contraction order returned by $1-sided$ $1-dim$ and $2-sided$ $\Delta-dim$ algorithms, as well as Cotengra library algorithms run using our wrapper, uses a recursive definition the contractions.

<img src="./docs/Ordering_example.png" width="240px"/>

For example for contractions presented above, the order returned by the algorithm would be defined as `((((0, 1), (3, 4)), 2), 5)`, which can be represented in form of list of nodes to be contracted as `[(0, 1), (3, 4), (0, 3), (0, 2), (0, 5)]` (assuming that node with lower id represents the result of each contraction). 

For convenience, we have provided the script to flatten the recursive order definition:
`tools/flat_order.py`.