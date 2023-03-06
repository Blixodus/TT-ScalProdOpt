#include "AllSplits.hpp"
//TODO: broken, floating point exception
/**
 * @brief Solves a given state
 * 
 * @param state The tensors in this state
 * @return int the best cost for S
 */
cost_t AllSplits::solve(vector_vertexID_t& state){
    //encodage de l'ensemble de sommets
    unsigned long long key = convert(state);

    //si il reste plus d'1 sommet et que le coût n'a pas encore été calculé
    if(m_cost_map.find(key) == m_cost_map.end() && state.size() > 1){
        m_cost_map[key] = std::numeric_limits<cost_t>::max();
        cost_t cost;
        
        vector_vertexID_t state1;
        vector_vertexID_t state2;

        cost_t cout_sortant = produit_sortant(state, compute_ecl(state));
        
        //pour toutes les séparations de state
        do{
            //on déplace la "barre" où on fait la coupure state1/state2 dans state
            for(vertexID_t i = 0; i < state.size() - 1; i++){
                state1.clear();
                state2.clear();
                //on attribue les sommets de state1
                for(vertexID_t k = 0; k <= i; k++){
                    state1.push_back(state[k]);
                }
                //on attribue les sommets de state2
                for(vertexID_t k = i+1; k < state.size(); k++){
                    state2.push_back(state[k]);
                }
                cost = solve(state1);
                if(!state2.empty()){ 
                    cost += solve(state2) + cout_sortant*cut(state1, state2);
                }
                if(cost < m_cost_map[key] && cost > 0){
                    m_cost_map[key] = cost;
                    //m_order_map_1[key] = convert(state1);
                    //m_order_map_2[key] = convert(state2);
                }
            }
        }while(next_permutation(state.begin(), state.end()));
    }else if(state.size() == 1){
        m_cost_map[key] = 0;
    }
    return m_cost_map[key];
}

/**
 * @brief computes the state.size()-1'th column of m_ext_cost_tab
 * 
 * @param state The vertices in this state
 * @return matrix_weight_t an updated copy of m_ext_cost_tab
 */
matrix_weight_t AllSplits::compute_ecl(vector_vertexID_t const& state){
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
cost_t AllSplits::cut(vector_vertexID_t const& state1, vector_vertexID_t const& state2){
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
cost_t AllSplits::produit_sortant(vector_vertexID_t const& state, matrix_cost_t const& ext_cost_tab){
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
unsigned long long AllSplits::convert(vector_vertexID_t const& state){
    unsigned long long res = 0;
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
vector_vertexID_t AllSplits::recover(unsigned long long key){
    vector_vertexID_t res;
    for(vertexID_t i = n_vertex; i >= 0; i--){
        unsigned long long p = pow(2, i);
        if(key >= p){
            res.push_back(i);
            key -= p;
        }
    }
    return res;
}

void AllSplits::display_order(vector_vertexID_t const& state){//dégueulasse
    if(state.size() > 1){
        unsigned long long key = convert(state);
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
void AllSplits::display_order(){}

void AllSplits::init(Network& network){
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

cost_t AllSplits::call_solve(){
    return solve(m_state);
}