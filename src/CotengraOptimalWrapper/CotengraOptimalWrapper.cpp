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
cost_t CotengraOptimalWrapper::solve(vector_vertexID_t& state, bool contiguous_ids = true){
    double key = convert(state);
    /*std::cout<<"[Cotengra wrapper CPP] Request for solving state : ";
    for(vertexID_t i : state){
        std::cout << i << " | ";
    }
    std::cout << std::endl << std::flush;*/

    // Locate which range of dimensions from whole network file should be solved
    int dim_min = state[0], dim_max = state[0];
    if(contiguous_ids)
    {
        for(int i = 0; i < state.size() / 2; i++) {
            dim_min = min(state[i], dim_min);
            dim_max = max(state[i], dim_max);
        }
    } 
    else {
        for(int i = 0; i < state.size(); i += 2) {
            dim_min = min(state[i], dim_min);
            dim_max = max(state[i], dim_max);
        }
    }
    dim_max++;

    //std::cout<<"[Cotengra wrapper CPP] Solving range : "<<dim_min<<"..."<<dim_max<<std::endl;
    //std::cout<<"!!"<<std::endl;

    // Start the Python interpreter
    //std::cout << "Starting Python interpreter" << std::endl;
    //py::scoped_interpreter guard{};
    

    //py::gil_scoped_release release;
    //py::initialize_interpreter();
    //py::scoped_interpreter guard{};

    //PyObject *sys = PyImport_ImportModule("sys");
    //PyObject *path = PyObject_GetAttrString(sys, "path");
    //PyList_Append(path, PyUnicode_FromString("."));

    //py::gil_scoped_acquire acquire;
    //std::cout<<"!!!"<<std::endl;
    //py::print("Hello, World!");

    //std::cout<<"[Cotengra wrapper CPP] Calling Python script for dim : "<<dim << "and file "<<m_network->m_filename<<std::endl;
    //std::cout<<"!!!!"<<std::endl<<std::flush;

    auto python_script = py::module::import("cotengra_wrapper");
    //std::cout<<"!!!!!"<<std::endl<<std::flush;
    auto resultobj = python_script.attr("cotengra_wrapper_solve")(m_network->m_filename, dim_min, dim_max);
    //std::cout<<"!!!!!!"<<std::endl<<std::flush;
    cost_t cost = resultobj.cast<cost_t>();
    //std::cout<<"Result of the call: "<<result<<std::endl;
    //std::cout<<"Filename"<<filename<<std::endl;
    //py::gil_scoped_release release;
    
    //py::finalize_interpreter();
    //py::gil_scoped_acquire acquire;
    //std::cout<<"Finalized interpreter"<<std::endl;

    std::cout<<"[Cotengra] Request for solving state : " << key << std::endl;
    for(vertexID_t i : state){
        std::cout << i << " | ";
    }

    /*cost_t result = cost;
    cost_t cout_sortant = produit_sortant(state, compute_ecl(state));

    if(n_vertex - state.size() > 0)
    {
        if(state.size() == 2) {
            result = cost * cout_sortant;
        } else {
            result = cost + cout_sortant * cut(state);
        }
    }

    std::cout << "Cost: " << result << std::endl;
    std::cout << cost << " " << cout_sortant << " " << cut(state) << std::endl;*/
    std::cout << "Cost: " << cost << std::endl;

    return cost;
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
 * @brief computes the product of all edges linking 2 states together
 * 
 * @param state1 a state
 * @param state2 a state
 * @return int 
 */
cost_t CotengraOptimalWrapper::cut(vector_vertexID_t const& state1){
    vector_vertexID_t state2;
    for(vertexID_t i = 0; i < n_vertex; i++){
        if(std::find(state1.begin(), state1.end(), i) == state1.end()){
            state2.push_back(i);
        }
    }

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