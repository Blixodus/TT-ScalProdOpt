#include "CotengraOptimalWrapper.hpp"
#include <pybind11/embed.h>
#include <iostream>

namespace py = pybind11;

//TODO: broken, floating point exception
/**
 * @brief Solves a given state
 * 
 * @param state The tensors in this state
 * @return int the best cost for S
 */
cost_t CotengraOptimalWrapper::solve(vector_vertexID_t& state){
    //std::cout<<"[Cotengra wrapper] Request for solving state : ";
    for(vertexID_t i : state){
        std::cout << i << " | ";
    }
    std::cout << std::endl;

    // Start the Python interpreter
    //std::cout << "Starting Python interpreter" << std::endl;
    py::scoped_interpreter guard{};
    //py::print("Hello, World!");

    auto python_script = py::module::import("contegra_wrapper");
    auto resultobj = python_script.attr("cotengra_wrapper_solve")(m_network->m_filename, dim);
    double result = resultobj.cast<double>();
    //std::cout<<"Result of the call: "<<result<<std::endl;
    //std::cout<<"Filename"<<filename<<std::endl;

    return result;
}

/**
 * @brief computes the state.size()-1'th column of m_ext_cost_tab
 * 
 * @param state The vertices in this state
 * @return matrix_weight_t an updated copy of m_ext_cost_tab
 */
matrix_weight_t CotengraOptimalWrapper::compute_ecl(vector_vertexID_t const& state){
    for(vertexID_t i : state){
        m_ext_cost_tab[n_vertex*(state.size()-1)+i] = m_adjacence_matrix[n_vertex*n_vertex + i];
        for(vertexID_t k : state){
            m_ext_cost_tab[n_vertex*(state.size()-1)+i] /= m_adjacence_matrix[n_vertex*i + k];
        }   
    }
    return m_ext_cost_tab;
}

/**
 * @brief computes the product of all edges linking 2 states together
 * 
 * @param state1 a state
 * @param state2 a state
 * @return int 
 */
cost_t CotengraOptimalWrapper::cut(vector_vertexID_t const& state1, vector_vertexID_t const& state2){
    cost_t res = 1;
    for(vertexID_t i : state1){
        for(vertexID_t j : state2){
            res *= m_adjacence_matrix[n_vertex*i + j];
        }
    }
    return res;
}

/**
 * @brief computes the product of all the edges leaving a state
 * 
 * @param state The tensors in this state
 * @param ext_cost_tab The external-weights of all tensors for each states
 * @return int 
 */
cost_t CotengraOptimalWrapper::produit_sortant(vector_vertexID_t const& state, matrix_cost_t const& ext_cost_tab){
    cost_t res = 1;
    for(vertexID_t i : state){
        res *= ext_cost_tab[n_vertex*(state.size()-1) + i];
    }
    return res;
}

/**
 * @brief converts a state in a unique integer key
 * 
 * @param state The tensors in this state
 * @return int 
 */
double CotengraOptimalWrapper::convert(vector_vertexID_t const& state){
    double res = 0;
    for(vertexID_t i : state){
        res += pow(2, i);
    }
    return res;
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

void CotengraOptimalWrapper::display_order(vector_vertexID_t const& state){//dégueulasse
    if(state.size() > 1){
        double key = convert(state);
        display_order(recover(m_order_map_1[key]));
        display_order(recover(m_order_map_2[key]));
        std::cout << "| ";
        for(vertexID_t i : state){
            std::cout << i << " | ";
        }
        std::cout << std::endl;
    }
}

/**
 * @brief dummy methods to use in template
 * 
 */
void CotengraOptimalWrapper::display_order(){}

void CotengraOptimalWrapper::init(Network& network){
    set_limit_dim(network.n_vertex);
    dim = network.dimension;
    n_vertex = network.n_vertex;
    m_adjacence_matrix.clear();
    m_ext_cost_tab.clear();
    m_cost_map.clear();
    //m_order_map_1.clear();
    //m_order_map_2.clear();

    best_cost = numeric_limits<cost_t>::max();

    m_state.resize(n_vertex);
    m_adjacence_matrix.resize(n_vertex*(n_vertex+1), 1);
    m_ext_cost_tab.resize(n_vertex*n_vertex, 1);
    //m_cost_map.resize(pow(2, size), -1);
    //m_order_map_1.resize(pow(2, size), -1);
    //m_order_map_2.resize(pow(2,size), -1);

    for(auto e : network.edge_list){
        weight_t w = network.adjacence_matrix[n_vertex*e.first + e.second];
        m_adjacence_matrix[n_vertex*e.first + e.second] = w;
        m_adjacence_matrix[n_vertex*e.second + e.first] = w;

        //extra column to store the cumulated weight or smthing
        m_adjacence_matrix[n_vertex*n_vertex + e.first] *= w;
        m_adjacence_matrix[n_vertex*n_vertex + e.second] *= w;
    }

    for(vertexID_t k = 0; k < n_vertex; k ++){
        m_state[k] = k;
    }
}

cost_t CotengraOptimalWrapper::call_solve(){
    return solve(m_state);
}