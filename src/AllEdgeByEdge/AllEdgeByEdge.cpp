#include "AllEdgeByEdge.hpp"

cost_t AllEdgeByEdge::solve(SouG& sg){
    //encodage de l'ensemble d'arête
    int key = get_key(sg.m_state);
    //copie du sous-graphe d'entrée pour pouvoir le modifier
    SouG sgref = sg; 

    //cas où il reste plusieurs arêtes, et le coût de l'ensemble n'a pas encore été calculé
    if(sg.m_state.size() > 1 && m_cost_memo.find(key) == m_cost_memo.end()){
        m_cost_memo[key] = best_cost+1;

        for(int i = 0; i < sgref.m_state.size(); i++){
            //on copie la copie, et on laisse l'arête en cours de côté
            SouG sg2 = sgref;
            sg2.m_state.erase(sg2.m_state.begin() + i);

            //solve nous donne le meilleur coût pour cet ensemble d'arête, et mettra à jour G et m_corr_list
            cost_t cost = solve(sg2);
            //on contract finalement l'arête mise de côté
            cost += contract(sgref.m_state[i], sg2);
            if(cost < m_cost_memo[key] && cost > 0){
                //mémoïsation
                m_cost_memo[key] = cost;
                m_order_map[key] = sgref.m_state[i];
                //mise à jour de SG pour faire remonter m_adjacence_matrix et m_corr_list
                sg.m_adjacence_matrix = sg2.m_adjacence_matrix; 
                sg.m_corr_list = sg2.m_corr_list;
            }
        }
    }else if(m_cost_memo.find(key) == m_cost_memo.end()){
        //si il ne reste qu'une arête mais que le coût n'a pas été calculé
        m_cost_memo[key] = contract(sg.m_state[0], sg);
        m_order_map[key] = sg.m_state[0];
    }else{
        //cas où m_cost_memo[key] est déjà calculé, on a quand même besoin de connaître l'état du graphe suite aux contractions
        //on connait l'ensemble des arêtes qui ont été contractées : sg.m_state
        //on peut bruteforce les m_state.size() contractions, puisqu'on a déjà le meilleur coût on se fiche de l'ordre
        for(int i : sg.m_state){
            cheap_contract(i, sg);
        }
    }

    return m_cost_memo[key];
}

/**
 * @brief computes the cost of a contraction and modifies the sub-graph (m_corr_list and m_adjacence_matrix)
 * 
 * @param i the index (in m_edge_list) of the contracted edge
 * @param sg the sub-graph
 * @return cost_t 
 */
cost_t AllEdgeByEdge::contract(int i, SouG& sg){
    //on va chercher les sommets avec lesquels l'arête i est réellement en contact dans le graphe
    int a = sg.rep(m_edge_list[i].first);
    int b = sg.rep(m_edge_list[i].second);

    if(a != b){
        //calcul du coût
        int res = sg.m_adjacence_matrix[n_vertex*b + a];

        for(int j = 0; j < n_vertex; j++){
            if(b != j){
                res *= max(1, sg.m_adjacence_matrix[n_vertex*a + j]);
            }

            if(a != j){
                res *= max(1, sg.m_adjacence_matrix[n_vertex*b + j]);
            }
        }

        //mise à jour de m_adjacence_matrix
        for(int j = 0; j < n_vertex; j++){
            sg.m_adjacence_matrix[n_vertex*a + j] *= sg.m_adjacence_matrix[n_vertex*b + j];
            sg.m_adjacence_matrix[n_vertex*b + j] = 0;
            sg.m_adjacence_matrix[n_vertex*j + b] = 0;
            sg.m_adjacence_matrix[n_vertex*j + a] = sg.m_adjacence_matrix[n_vertex*a + j];
        }

        //mise à jour de m_corr_list
        sg.m_corr_list[b] = a;
        return res;
    }else{
        return 0;
    }
}

/**
 * @brief same as contract, but doesn't compute the cost (only updates m_adjacence_matrix and m_corr_list)
 * 
 * @param i 
 * @param sg 
 */
void AllEdgeByEdge::cheap_contract(int i, SouG& sg){
    int a = sg.rep(m_edge_list[i].first);
    int b = sg.rep(m_edge_list[i].second);

    if(a != b){
        for(int j = 0; j < n_vertex; j++){
            sg.m_adjacence_matrix[n_vertex*a + j] *= sg.m_adjacence_matrix[n_vertex*b + j];
            sg.m_adjacence_matrix[n_vertex*b + j] = 0;
            sg.m_adjacence_matrix[n_vertex*j + b] = 0;
            sg.m_adjacence_matrix[n_vertex*j + a] = sg.m_adjacence_matrix[n_vertex*a + j];
        }
        sg.m_corr_list[b] = a;
    }
}

/**
 * @brief retrieves the vertex thats is the reference for i in the graph
 * 
 * @param i a vertex
 * @return int 
 */
int SouG::rep(int i){
    //on parcours la "chaîne de sommet" jusqu'à arriver au sommet de référence (normalement celui ayant le plus petit indice)
    while(m_corr_list[i] != -1){
        i = m_corr_list[i];
    }
    return i;
}

/**
 * @brief encodes a set of edges as a unique int key
 * 
 * @param state an edge list
 * @return int 
 */
unsigned long long AllEdgeByEdge::get_key(Tab state){
    unsigned long long res = 0;
    for(int i : state){
        res += pow(2, i);
    }
    return res-1;
}

/**
 * @brief displays the best order given a starting state encoded as a key
 * 
 * @param key 
 */
void AllEdgeByEdge::display_order(unsigned long long key){
    int i = m_order_map[key]; //i = la meilleure arête à contracter pour l'état key-2^i + 1
    unsigned long long next = key-pow(2, i);
    if(key == get_key(m_state)){
        display_order(next);
        cout << i << '\n';
    }else if(next >= 0){
        display_order(next);
        cout << i << " - ";
    }else if(next == -1){
        cout << i << " - ";
    }
}

void AllEdgeByEdge::display_order(){
    for(int i = 0; i < best_order.size()-1; i++){
        cout << best_order[i] << " - ";
    }
    cout << best_order.back() << '\n';

}

void AllEdgeByEdge::get_order(unsigned long long key){
    int i = m_order_map[key];

    unsigned long long next = key-pow(2, i);

    if(next+1 > 0){
        get_order(next);
        best_order.push_back(i);
    }else{
        best_order.clear();
        best_order.push_back(i);
    }
}

void AllEdgeByEdge::init(Network& network){
    set_limit_dim(network.n_edge);
    dim = network.dimension;
    n_vertex = network.n_vertex;

    m_adjacence_matrix = network.adjacence_matrix;
    m_edge_list = network.edge_list;

    m_order_map.clear();
    m_cost_memo.clear();

    m_state.clear();
    m_state.resize(network.n_edge);
    for(int i = 0; i < network.n_edge; i++){
        m_state[i] = i;
    }

    best_cost = numeric_limits<cost_t>::max() - 1;
    best_order.clear();

    sgref = getSG();
}

cost_t AllEdgeByEdge::call_solve(){
    cost_t c = solve(sgref);
    get_order(get_key(m_state));
    //TODO: something crashes here, not sure why
    //verify();
    return c;
}