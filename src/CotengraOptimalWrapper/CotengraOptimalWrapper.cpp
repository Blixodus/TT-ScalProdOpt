#include "CotengraOptimalWrapper.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <iostream>

namespace py = pybind11;

/**
 * @brief Solves a given state
 * 
 * @param state The tensors in this state
 * @return cost_t the best cost for S
 */
std::pair<cost_t, std::string> CotengraOptimalWrapper::solve(const int dim_min, const int dim_max, const result_direction_e direction) {
    // Call the Cotengra wrapper script to solve the subpart of the network
    // using the optimal algorithm from the Cotengra library
    auto python_script = py::module::import("cotengra_wrapper");
    auto resultobj = python_script.attr("cotengra_wrapper_solve")(this->filename, dim_min, dim_max, this->dimension, (int)direction);
    auto result = resultobj.cast<py::tuple>();

    return make_pair(result[0].cast<cost_t>(), result[1].cast<std::string>());
}

void CotengraOptimalWrapper::init(Network& network){
    this->init(network.m_filename, network.dimension);
}

void CotengraOptimalWrapper::init(std::string filename, const int dimension){
    py::initialize_interpreter();

    this->filename = filename;
    this->dimension = dimension;

    best_cost = numeric_limits<cost_t>::max();
}

cost_t CotengraOptimalWrapper::call_solve(){
    return solve(0, this->dim - 1, LR).first;
}