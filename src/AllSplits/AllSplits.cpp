#include "AllSplits.hpp"
//TODO: broken, floating point exception
/**
 * @brief Solves a given state
 * 
 * @param state The tensors in this state
 * @return int the best cost for S
 */
cost_t AllSplits::solve(Tab state){
    //encodage de l'ensemble de sommets
    unsigned long long key = convert(state);

    //si il reste plus d'1 sommet et que le coût n'a pas encore été calculé
    if(m_cost_map.find(key) == m_cost_map.end() && state.size() > 1){
        m_cost_map[key] = best_cost+1;
        cost_t cost;
        
        Tab state1;
        Tab state2;

        cost_t cout_sortant = produit_sortant(state, compute_ecl(state));
        
        //pour toutes les séparations de state
        do{
            //on déplace la "barre" où on fait la coupure state1/state2 dans state
            for(int i = 0; i < state.size() - 1; i++){
                state1.clear();
                state2.clear();
                //on attribue les sommets de state1
                for(int k = 0; k <= i; k++){
                    state1.push_back(state[k]);
                }
                //on attribue les sommets de state2
                for(int k = i+1; k < state.size(); k++){
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
 * @return Tab an updated copy of m_ext_cost_tab
 */
Tab AllSplits::compute_ecl(Tab state){
    for(int i : state){
        m_ext_cost_tab[n_vertex*(state.size()-1)+i] = m_adjacence_matrix[n_vertex*n_vertex + i];
        for(int k : state){
            //TODO: only place with a division I reckon
            // std::cout << "enter" << std::endl;
            m_ext_cost_tab[n_vertex*(state.size()-1)+i] /= m_adjacence_matrix[n_vertex*i + k];
            // std::cout << "exit" << std::endl;
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
cost_t AllSplits::cut(Tab state1, Tab state2){
    cost_t res = 1;
    for(int i : state1){
        for(int j : state2){
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
cost_t AllSplits::produit_sortant(Tab state, Tab ext_cost_tab){
    cost_t res = 1;
    for(int i : state){
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
unsigned long long AllSplits::convert(Tab state){
    unsigned long long res = 0;
    for(int i : state){
        res += pow(2, i);
    }
    return res;
}

/**
 * @brief converts a key into a set of tensors
 * 
 * @param key a code generated from a state using convert(state)
 * @return Tab 
 */
Tab AllSplits::recover(unsigned long long key){
    Tab res;
    for(int i = n_vertex; i >= 0; i--){
        int p = pow(2, i);
        if(key >= p){
            res.push_back(i);
            key -= p;
        }
    }
    return res;
}

void AllSplits::display_order(Tab state){//dégueulasse
    if(state.size() > 1){
        int key = convert(state);
        display_order(recover(m_order_map_1[key]));
        display_order(recover(m_order_map_2[key]));
        std::cout << "| ";
        for(int i : state){
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

    best_cost = numeric_limits<cost_t>::max() - 1;

    m_state.resize(n_vertex);
    m_adjacence_matrix.resize(n_vertex*(n_vertex+1), 1);
    m_ext_cost_tab.resize(n_vertex*n_vertex, 1);
    //m_cost_map.resize(pow(2, size), -1);
    //m_order_map_1.resize(pow(2, size), -1);
    //m_order_map_2.resize(pow(2,size), -1);

    for(auto e : network.edge_list){
        int w = network.adjacence_matrix[n_vertex*e.first + e.second];
        m_adjacence_matrix[n_vertex*e.first + e.second] = w;
        m_adjacence_matrix[n_vertex*e.second + e.first] = w;

        //extra column to store the cumulated weight or smthing
        m_adjacence_matrix[n_vertex*n_vertex + e.first] *= w;
        m_adjacence_matrix[n_vertex*n_vertex + e.second] *= w;
    }

    for(int k = 0; k < n_vertex; k ++){
        m_state[k] = k;
    }
}

cost_t AllSplits::call_solve(){
    return solve(m_state);
}