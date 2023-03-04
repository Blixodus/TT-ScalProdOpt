#include "SplitsDimByDim.hpp"

/**
 * @brief Solves a given state
 * 
 * @param state The dimensions in this state
 * @return int the best cost for state
 */
cost_t SplitsDBD::solve(vector_vertexID_t const& state){
    //encodage de l'ensemble de sommets
    unsigned long long key = convert(state);

    if(m_cost_memo.find(key) == m_cost_memo.end() && state.size() > 1){
        m_cost_memo[key] = best_cost+1;

        cost_t cost;
        
        vector_vertexID_t state1;
        vector_vertexID_t state2;

        //Product of all the edges leaving a state (set of vertices)
        cost_t cout_sortant = produit_sortant(state, compute_ect(state));
    
        //on parcours tous les découpages de dimension DELTA et moins
        for(int i = 0; i < min(dmax, (int) state.size()); i++){
            state1.clear();
            state2.clear();
            //on attribue les sommets de state1
            //TODO: this is unnecessary, we can get all the vertices from state1 just thanks to the dimension
            for(int k = 0; k <= i; k++){
                state1.push_back(state[k]);
                state1.push_back(state[k] + n_vertex/2);
            }
            //TODO: same here
            //on attribue les sommets de state2
            for(int k = i+1; k < state.size(); k++){
                state2.push_back(state[k]);
            }
            //on résoud state1 de manière exacte, on découpe state2 (le reste du TT)
            cost = m_exact_solver.solve(state1);
            if(!state2.empty()){ //what
                //There was a random '+' here ???
                cost += solve(state2) + cout_sortant*cut(state1, state2);
            } 
            //cout << cost << '\n';
            //cout << m_cost_memo[key] << '\n';
            if(cost > 0 && (cost < m_cost_memo[key] || m_cost_memo.find(key) == m_cost_memo.end())){
                m_cost_memo[key] = cost;
                m_order_map1[key] = convert(state1);
                m_order_map2[key] = convert(state2);
            }
        }
    }else if(state.size() == 1){
        vector_vertexID_t p = {state[0], state[0] + n_vertex/2};
        //overkill
        m_cost_memo[key] = m_exact_solver.solve(p);
    }
    return m_cost_memo[key];
}

/**
 * @brief computes the state.size()-1'th column of m_ext_cost_tab
 * 
 * @param state The tensors in this state
 * @return vector_vertexID_t an updated copy of m_ext_cost_tab
 */
vector_weight_t SplitsDBD::compute_ect(vector_vertexID_t const& state){
    for(int i : state){
        //poids sortant de i dans l'ensemble de taille state.size() = m_adjacence_matrix[ofs + i] / le poids des liaisons avec des sommets dans state
        m_ext_cost_tab[n_vertex*(state.size()-1) + i] = m_adjacence_matrix[n_vertex*n_vertex + i];
        m_ext_cost_tab[n_vertex*(state.size()-1) + i+n_vertex/2] = m_adjacence_matrix[n_vertex*n_vertex + i+n_vertex/2];
        
        for(int k : state){
            m_ext_cost_tab[n_vertex*(state.size()-1) + i] /= m_adjacence_matrix[n_vertex*i + k];
            m_ext_cost_tab[n_vertex*(state.size()-1) + i] /= m_adjacence_matrix[n_vertex*i + k+n_vertex/2];
            m_ext_cost_tab[n_vertex*(state.size()-1) + i + n_vertex/2] /= m_adjacence_matrix[n_vertex*(i+n_vertex/2) + k];
            m_ext_cost_tab[n_vertex*(state.size()-1) + i + n_vertex/2] /= m_adjacence_matrix[n_vertex*(i+n_vertex/2) + k + n_vertex/2];
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
cost_t SplitsDBD::cut(vector_vertexID_t const& state1, vector_vertexID_t const& state2){
    cost_t res = 1;
    for(int i : state1){
        for(int j : state2){
            res *= m_adjacence_matrix[n_vertex*i + j];
            res *= m_adjacence_matrix[n_vertex*i + j + n_vertex/2];
        }
    }
    return res;
}

/**
 * @brief computes the product of all the edges leaving a state
 * 
 * @param state The tensors in this state
 * @param ext_cost_tab The external-weight of all tensors for each states
 * @return int 
 */
cost_t SplitsDBD::produit_sortant(vector_vertexID_t const& state, matrix_weight_t const& ext_cost_tab){
    cost_t res = 1;
    for(int i : state){
        res *= ext_cost_tab[n_vertex*(state.size()-1) + i];
        res *= ext_cost_tab[n_vertex*(state.size()-1) + i + n_vertex/2];
    }
    return res;
}

/**TODO: Juste use the start and end (in dimension of the sub-network)
 * @brief converts a state in a unique integer key
 * 
 * @param state The tensors in this state
 * @return int 
 */
long int SplitsDBD::convert(vector_vertexID_t const& state){
    int res = 0;
    for(int i : state){
        if(i < n_vertex/2){
            res += pow(2, i);
        }
    }
    return res;
}

/**
 * @brief converts a key into a set of tensors
 * 
 * @param key a code generated from a state using convert(state)
 * @return vector_vertexID_t 
 */
vector_vertexID_t SplitsDBD::recover(unsigned long long key){
    vector_vertexID_t res;
    for(int i = n_vertex/2; i >= 0; i--){
        int p = pow(2, i);
        if(key >= p){
            res.push_back(i);
            res.push_back(i+n_vertex/2);
            key -= p;
        }
    }
    return res;
}

vector_vertexID_t SplitsDBD::recover_full(vector_vertexID_t const& state){
    vector_vertexID_t res;
    for(vertexID_t i : state){
        res.push_back(i);
        res.push_back(i+n_vertex/2);
    }
    return res;
}

void SplitsDBD::display_order(vector_vertexID_t const& state){
    if(state.size() >= 1){
        long int key = convert(state);
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
void SplitsDBD::display_order(){}

void SplitsDBD::init(Network& network){
    set_limit_dim(network.dimension);
    dim = network.dimension;
    n_vertex = network.n_vertex;

    m_state.clear();
    m_adjacence_matrix.clear();
    m_ext_cost_tab.clear();
    m_cost_memo.clear();
    m_order_map1.clear();
    m_order_map2.clear();

    m_state.resize(dim);
    m_adjacence_matrix.resize(n_vertex*(n_vertex+1), 1);
    
    for(const auto& [v1, v2] : network.edge_list){
        weight_t w = network.adjacence_matrix[n_vertex*v1 + v2];
        m_adjacence_matrix[n_vertex*v1 + v2] = w;
        m_adjacence_matrix[n_vertex*v2 + v1] = w;

        m_adjacence_matrix[n_vertex*n_vertex + v1] *= w;
        m_adjacence_matrix[n_vertex*n_vertex + v2] *= w;
    }
    
    m_ext_cost_tab.resize(n_vertex*n_vertex, 1);

    best_cost = numeric_limits<cost_t>::max()-1;

    for(vertexID_t i = 0; i < dim; i ++){
        m_state[i] = i;
    }

    m_exact_solver.init(network);
}

cost_t SplitsDBD::call_solve(){
    return solve(m_state);
}