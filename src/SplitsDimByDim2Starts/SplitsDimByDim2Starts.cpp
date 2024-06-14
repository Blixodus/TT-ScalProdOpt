#include "SplitsDimByDim2Starts.hpp"
#include <iostream>
#include <algorithm>

/**
 * @brief Solves a given state
 * 
 * @param state The dimensions in this state
 * @return cost_t the best cost for state
 */
cost_t SplitsDBD2Starts::solve(vector_vertexID_t const& state, direction_e direction = LEFT_TO_RIGHT){
    // Encode the state as unique key (hash)
    double key = convert(state);

    // Check the momoization table for the cost of the state
    bool memoized_cost = false;
    if(direction == LEFT_TO_RIGHT) {
        if(m_cost_memo_LR.find(key) != m_cost_memo_LR.end()) {
            memoized_cost = true;
        }
    } else {
        if(m_cost_memo_RL.find(key) != m_cost_memo_RL.end()) {
            memoized_cost = true;
        }
    }

    // Solve the subpart if the solution is not already in the memoization table
    if(!memoized_cost && state.size() > 1) {
        cost_t cost;
        
        vector_vertexID_t state1;
        vector_vertexID_t state2;
    
        // Solve all the possible splits of the state up to DELTA
        if(direction == LEFT_TO_RIGHT) {
            m_cost_memo_LR[key] = std::numeric_limits<cost_t>::max();

            for(dim_t i = 0; i < min(dmax, (dim_t) state.size()); i++) {
                state1.clear();
                state2.clear();

                // Assign the nodes to first state (to be solved optimally)
                for(dim_t k = 0; k <= i; k++) {
                    state1.push_back(state[k]);
                    state1.push_back(state[k] + n_vertex/2);
                }

                // Assign the remaining nodes to second state (to be solved recursively)
                for(dim_t k = i+1; k < state.size(); k++) {
                    state2.push_back(state[k]);
                }

                // Solve the first state optimally (using Cotengra optimal algorithm)
                cost = m_exact_solver.solve(state1, false, direction);

                // Solve the second state recursively
                if(!state2.empty()) {
                    cost_t sol_state2 = solve(state2);
                    cost = (sol_state2 != std::numeric_limits<cost_t>::max())? cost + sol_state2 : std::numeric_limits<cost_t>::max();
                }

                // Update the cost if the new cost is better
                if(cost > 0 && cost < m_cost_memo_LR[key]) {
                    m_cost_memo_LR[key] = cost;
                    m_order_map1_LR[key] = convert(state1);
                    m_order_map2_LR[key] = convert(state2);
                }
            }
        } else {
            m_cost_memo_RL[key] = std::numeric_limits<cost_t>::max();

            for(dim_t i = (dim_t)state.size() - 1; i >= max(0, (dim_t)state.size() - dmax + 1); i--) {
                state1.clear();
                state2.clear();

                // Assign the nodes to first state (to be solved optimally)
                for(dim_t k = i; k < state.size(); k++) {
                    state1.push_back(state[k]);
                    state1.push_back(state[k] + n_vertex/2);
                }

                // Assign the remaining nodes to second state (to be solved recursively)
                for(dim_t k = 0; k <= i - 1; k++) {
                    state2.push_back(state[k]);
                }

                // Solve the first state optimally (using Cotengra optimal algorithm)
                cost = m_exact_solver.solve(state1, false, direction);

                // Solve the second state recursively
                if(!state2.empty()) {
                    cost_t sol_state2 = solve(state2);
                    cost = (sol_state2 != std::numeric_limits<cost_t>::max())? cost + sol_state2 : std::numeric_limits<cost_t>::max();
                }

                // Update the cost if the new cost is better
                if(cost > 0 && cost < m_cost_memo_RL[key]) {
                    m_cost_memo_RL[key] = cost;
                    m_order_map1_RL[key] = convert(state1);
                    m_order_map2_RL[key] = convert(state2);
                }
            }
        }
    } else if(state.size() == 1) {
        vector_vertexID_t p = {state[0], state[0] + n_vertex/2};
        cost_t cost = m_exact_solver.solve(p, false, direction);
        if(direction == LEFT_TO_RIGHT) m_cost_memo_LR[key] = cost;
        else m_cost_memo_RL[key] = cost;
    } else if(state.size() == 0) {
        return 0;
    }

    cost_t computed_cost;
    if(direction == LEFT_TO_RIGHT) {
        computed_cost = m_cost_memo_LR[key];
    } else {
        computed_cost = m_cost_memo_RL[key];
    }
    
    return computed_cost;
}

/**
 * @brief DEPRECATED computes the state.size()-1'th column of m_ext_cost_tab
 * 
 * @param state The tensors in this state
 * @return vector_vertexID_t an updated copy of m_ext_cost_tab
 */
vector_weight_t SplitsDBD2Starts::compute_ect(vector_vertexID_t const& state){
    return vector_weight_t();
}

/**
 * @brief converts a state in a unique integer (hash) key
 * 
 * @param state the dimensions in this state
 * @return int64_t (hash of the state)
 */
int64_t SplitsDBD2Starts::convert(vector_vertexID_t state){
    std::sort(state.begin(), state.end());
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
vector_vertexID_t SplitsDBD2Starts::recover(double key){
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

vector_vertexID_t SplitsDBD2Starts::recover_full(vector_vertexID_t const& state){
    vector_vertexID_t res;
    for(vertexID_t i : state){
        res.push_back(i);
        res.push_back(i+n_vertex/2);
    }
    return res;
}

void SplitsDBD2Starts::display_order(vector_vertexID_t const& state){
    if(state.size() >= 1){
        double key = convert(state);
        if(key != -1){
            vector_vertexID_t p1K = recover(m_order_map1_LR[key]);
            m_exact_solver.display_order(p1K);
            if(!p1K.empty() && p1K.size() != state.size()){
                //display_order(recover(m_order_map1[key]));
                display_order(recover(m_order_map2_LR[key]));
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
void SplitsDBD2Starts::display_order(){}

void SplitsDBD2Starts::init(Network& network){
    set_limit_dim(network.dimension);
    dim = network.dimension;
    std::cout<<"dimension init: "<<dim<<std::endl;
    n_vertex = network.n_vertex;

    m_state.clear();
    m_cost_memo_LR.clear();
    m_cost_memo_RL.clear();
    m_order_map1_LR.clear();
    m_order_map2_LR.clear();
    m_order_map1_RL.clear();
    m_order_map2_RL.clear();
    m_state.resize(dim);

    best_cost = std::numeric_limits<cost_t>::max();

    for(vertexID_t i = 0; i < dim; i++){
        m_state[i] = i;
    }

    m_exact_solver.init(network);
}

cost_t SplitsDBD2Starts::call_solve(){
    cost_t final_cost = std::numeric_limits<cost_t>::max();
    int final_split = 0;

    // Iterate over all the possible starting splits
    vector_vertexID_t nodes_left(m_state);
    vector_vertexID_t nodes_right;
    for(int split = 0; split < dim; split++){
        // Solve the left part using 1DΔD
        // (assuming we start the split from the left side of TT)
        cost_t cost_left = solve(nodes_left, LEFT_TO_RIGHT);
        
        // Solve the right part using 1DΔD in reverse order
        // (assuming we start the split from the right side of TT)
        cost_t cost_right = solve(nodes_right, RIGHT_TO_LEFT);

        // Compute the cost of contracting the final two nodes (results of
        // the two subproblems) together
        cost_t cost_connect = 0;
        if(!nodes_left.empty() && !nodes_right.empty()) {
            if(split == 2)
            {
                std::cout<<nodes_left.back()<<" "<<nodes_right.front()<<std::endl;
                std::cout<<nodes_left.back()+ n_vertex/2<<" "<<nodes_right.front()+ n_vertex/2<<std::endl;
            }
            cost_connect = m_network->adjacence_matrix[nodes_left.back() * n_vertex + nodes_right.front()] *
                           m_network->adjacence_matrix[(nodes_left.back() + n_vertex/2) * n_vertex + (nodes_right.front() + n_vertex/2)];
        }

        // Calculate the total cost of the split
        cost_t cost = cost_left + cost_right + cost_connect;

        std::cout<<split<<": \t"<<cost<<"\t"<<cost_left<<"\t"<<cost_right<<"\t"<<cost_connect<<std::endl;

        if (cost < final_cost) {
            final_cost = cost;
            final_split = split;
        }

        // Move the split node to the right part
        auto node = nodes_left.back();
        nodes_left.pop_back();

        std::reverse(nodes_right.begin(), nodes_right.end());
        nodes_right.push_back(node);
        std::reverse(nodes_right.begin(), nodes_right.end());
    }


    /*std::cout<<"Called solve"<<std::endl;
    cost_t cost_left = solve(m_state);

    std::cout<<"!!!!"<<std::endl;
    for(auto elem : m_cost_memo) {
        std::cout << elem.first << " " << elem.second << std::endl;
    }
    std::cout<<endl;

    auto m_cost_memo_left(m_cost_memo);

    this->reversed = true;
    this->init(*m_network);
    cost_t cost_right = solve(m_state);

    std::cout<<"[SplitsDBD2Starts] Cost left: "<<cost_left<<std::endl;
    std::cout<<"[SplitsDBD2Starts] Cost right: "<<cost_right<<std::endl;
    std::cout<<"[SplitsDBD2Starts] Best cost: "<<min(cost_left, cost_right)<<std::endl;

    for(auto elem : m_cost_memo) {
        std::cout << elem.first << " " << elem.second << std::endl;
    }

    cost_t cost_2starts = std::numeric_limits<cost_t>::max();
    vector_vertexID_t state_left, state_right(m_state);
    for(int i = 0; i < dim; i++)
    {
        state_left.push_back(i);
        state_right.pop_back();

        cost_t cost_left = m_cost_memo_left[convert(state_left)];
        cost_t cost_right = m_cost_memo[convert(state_right)];

        std::cout<<i<<": "<<cost_left<<" "<<cost_right<<std::endl;
        cost_2starts = min(cost_2starts, cost_left + cost_right);
    }
    std::cout<<m_cost_memo_left[convert(m_state)]<<" "<<m_cost_memo[convert(m_state)]<<std::endl;

    return min(min(cost_left, cost_right), cost_2starts);*/
    return final_cost;
}