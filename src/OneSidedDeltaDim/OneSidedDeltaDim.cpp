#include "OneSidedDeltaDim.hpp"
#include <iostream>

/**
 * @brief Solves a given state
 * 
 * @param state The dimensions in this state
 * @return cost_t the best cost for state
 */
cost_t OneSidedDeltaDim::solve(vector_vertexID_t const& state){
    // Encode the state as unique key (hash)
    double key = convert(state);

    // Solve the subpart if the solution is not already in the memoization table
    if(m_cost_memo.find(key) == m_cost_memo.end() && state.size() > 1){
        m_cost_memo[key] =std::numeric_limits<cost_t>::max();
        cost_t cost;
        
        vector_vertexID_t state1;
        vector_vertexID_t state2;
    
        // Solve all the possible splits of the state up to DELTA
        for(dim_t i = 0; i < min(dmax, (dim_t) state.size()); i++){
            state1.clear();
            state2.clear();

            // Assign the nodes to first state (to be solved optimally)
            for(dim_t k = 0; k <= i; k++){
                state1.push_back(state[k]);
                state1.push_back(state[k] + n_vertex/2);
            }

            // Assign the remaining nodes to second state (to be solved recursively)
            for(dim_t k = i+1; k < state.size(); k++){
                state2.push_back(state[k]);
            }

            // Solve the first state optimally (using Cotengra optimal algorithm)
            cost = m_exact_solver.solve(state1, false, (direction_e)this->reversed);

            // Solve the second state recursively
            if(!state2.empty()){
                cost_t sol_state2 = solve(state2);
                cost = (sol_state2 != std::numeric_limits<cost_t>::max())? cost + sol_state2 : std::numeric_limits<cost_t>::max();
            }

            // Update the cost if the new cost is better
            if(cost > 0 && cost < m_cost_memo[key]){
                m_cost_memo[key] = cost;
                m_order_map1[key] = convert(state1);
                m_order_map2[key] = convert(state2);
            }
        }

    }else if(state.size() == 1){
        vector_vertexID_t p = {state[0], state[0] + n_vertex/2};
        m_cost_memo[key] = m_exact_solver.solve(p, false, (direction_e)this->reversed);
    }
    
    return m_cost_memo[key];
}

/**
 * @brief DEPRECATED computes the state.size()-1'th column of m_ext_cost_tab
 * 
 * @param state The tensors in this state
 * @return vector_vertexID_t an updated copy of m_ext_cost_tab
 */
vector_weight_t OneSidedDeltaDim::compute_ect(vector_vertexID_t const& state){
    return vector_weight_t();
}

/**
 * @brief converts a state in a unique integer (hash) key
 * 
 * @param state the dimensions in this state
 * @return int64_t (hash of the state)
 */
int64_t OneSidedDeltaDim::convert(vector_vertexID_t const& state){
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
vector_vertexID_t OneSidedDeltaDim::recover(double key){
    vector_vertexID_t res;
    for(int i = n_vertex/2; i >= 0; i--){
        double p = pow(2, i);
        if(key >= p){
            res.push_back(i);
            res.push_back(i+n_vertex/2);
            key -= p;
        }
    }
    return res;
}

vector_vertexID_t OneSidedDeltaDim::recover_full(vector_vertexID_t const& state){
    vector_vertexID_t res;
    for(vertexID_t i : state){
        res.push_back(i);
        res.push_back(i+n_vertex/2);
    }
    return res;
}

void OneSidedDeltaDim::display_order(vector_vertexID_t const& state){
    if(state.size() >= 1){
        double key = convert(state);
        if(key != -1){
            vector_vertexID_t p1K = recover(m_order_map1[key]);
            m_exact_solver.display_order(p1K);
            if(!p1K.empty() && p1K.size() != state.size()){
                //display_order(recover(m_order_map1[key]));
                display_order(recover(m_order_map2[key]));
            }
            cout << "| ";
            for(vertexID_t i : recover(key)){
                cout << i << " | ";
            }
            cout << endl;
        }
    }
}

/**
 * @brief dummy method to use in template
 * 
 */
void OneSidedDeltaDim::display_order(){}

void OneSidedDeltaDim::init(Network& network){
    set_limit_dim(network.dimension);
    dim = network.dimension;
    std::cout<<"dimension init: "<<dim<<std::endl;
    n_vertex = network.n_vertex;

    m_state.clear();
    m_cost_memo.clear();
    m_order_map1.clear();
    m_order_map2.clear();
    m_state.resize(dim);

    best_cost = std::numeric_limits<cost_t>::max();

    for(vertexID_t i = 0; i < dim; i++){
        if(!this->reversed) m_state[i] = i;
        else m_state[i] = dim - i - 1;
    }

    if(!this->reversed) m_exact_solver.init(network);
}

cost_t OneSidedDeltaDim::call_solve(){
    std::cout<<"Called solve"<<std::endl;
    cost_t cost_left = solve(m_state);

    this->reversed = true;
    this->init(*m_network);
    cost_t cost_right = solve(m_state);

    std::cout<<"[OneSidedDeltaDim] Cost left: "<<cost_left<<std::endl;
    std::cout<<"[OneSidedDeltaDim] Cost right: "<<cost_right<<std::endl;
    std::cout<<"[OneSidedDeltaDim] Best cost: "<<min(cost_left, cost_right)<<std::endl;

    return min(cost_left, cost_right);
}