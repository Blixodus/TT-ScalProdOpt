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
    auto resultobj = python_script.attr("cotengra_wrapper_solve")(m_network->m_filename, dim_min, dim_max, (int)direction);
    auto result = resultobj.cast<py::tuple>();

    return make_pair(result[0].cast<cost_t>(), result[1].cast<std::string>());
}

/**
 * @brief converts a state in a unique integer key
 * 
 * @param state The tensors in this state
 * @return int64_t 
 */
int64_t CotengraOptimalWrapper::convert(vector_vertexID_t const& state){
    int64_t seed = state.size();
    for(auto x : state) {
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

/**
 * @brief converts a key into a set of tensors
 * 
 * @param key a code generated from a state using convert(state)
 * @return vector_vertexID_t
 */
vector_vertexID_t CotengraOptimalWrapper::recover(double key){
    vector_vertexID_t res;
    for(vertexID_t i = n_vertex; i >= 0; i--){
        double p = pow(2, i);
        if(key >= p){
            res.push_back(i);
            key -= p;
        }
    }
    return res;
}

void CotengraOptimalWrapper::display_order(vector_vertexID_t const& state){
    std::cout << "| CTG-wrapper: TBD | " << std::endl;
}

/**
 * @brief dummy methods to use in template
 * 
 */
void CotengraOptimalWrapper::display_order(){}

void CotengraOptimalWrapper::init(Network& network){
    py::initialize_interpreter();

    m_network = &network;
    set_limit_dim(network.n_vertex);
    dim = network.dimension;
    n_vertex = network.n_vertex;
    m_cost_map.clear();

    best_cost = numeric_limits<cost_t>::max();

    m_state.resize(n_vertex);

    for(vertexID_t k = 0; k < n_vertex; k ++){
        m_state[k] = k;
    }
}

cost_t CotengraOptimalWrapper::call_solve(){
    return solve(0, this->dim - 1, LR).first;
}