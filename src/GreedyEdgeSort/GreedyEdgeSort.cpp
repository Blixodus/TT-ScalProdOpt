#include "GreedyEdgeSort.hpp"

/**
 * @brief solves m_adjacence_matrix
 * 
 * @return int 
 */
cost_t GreedyEdgeSort::solve(){
    best_cost = 0;
    //tant qu'il reste des arêtes
    while(!m_sorted_scores.empty()){
        best_order.push_back(m_sorted_scores.front().first);
        //les vraies extrémités de l'arête
        vertexID_t i = rep(m_edge_list[m_sorted_scores.front().first].first);
        vertexID_t k = rep(m_edge_list[m_sorted_scores.front().first].second);
        if(i != k){
            //ajoute le coût de contraction de R.front à cost
            best_cost += ext_cost(i, k)*m_adjacence_matrix[n_vertex*i + k];
            contract(i, k);
        }
        //suppression de l'arête
        m_sorted_scores.erase(m_sorted_scores.begin());

        //update du tableau des ratio
        updateRatio();
    }
    return best_cost;
}

/**
 * @brief updates the sorted list of scores R
 * Sub-optimal, but time is not an issue here
 */
void GreedyEdgeSort::updateRatio(){
    for(auto& p : m_sorted_scores){
        vertexID_t i = rep(m_edge_list[p.first].first);
        vertexID_t k = rep(m_edge_list[p.first].second);
        p.second = m_adjacence_matrix[n_vertex*i + k]/(float) ext_cost(i, k);
    }
    sort(m_sorted_scores.begin(), m_sorted_scores.end(), [](pair<edgeID_t, double> a, pair<edgeID_t, double> b){return a.second > b.second;});
}

/**
 * @brief Computes the external cost of 2 vertex
 * 
 * @param i a vertex
 * @param k a vertex
 * @return cost_t 
 */
cost_t GreedyEdgeSort::ext_cost(vertexID_t i, vertexID_t k){
    cost_t res = 1;
    for(vertexID_t j = 0; j < n_vertex; j++){
        if(k != j){
            res *= max(1, m_adjacence_matrix[n_vertex*i + j]);
        }
        if(i != j){ 
            res *= max(1, m_adjacence_matrix[n_vertex*k + j]);
        }
    }

    return res;
}

/**
 * @brief contracts an edge by modifying m_adjacence_matrix and m_corr_list accordingly
 * 
 * @param i a vertex
 * @param k a vertex
 */
void GreedyEdgeSort::contract(vertexID_t i, vertexID_t k){
    for(vertexID_t j = 0; j < n_vertex; j++){
        m_adjacence_matrix[n_vertex*i + j] *= m_adjacence_matrix[n_vertex*k + j];
        m_adjacence_matrix[n_vertex*k + j] = 0;
        m_adjacence_matrix[n_vertex*j + k] = 0;
        m_adjacence_matrix[n_vertex*j + i] = m_adjacence_matrix[n_vertex*i + j];
    }
    m_corr_list[k] = i;
}

/**
 * @brief returns i's representant
 * 
 * @param i a vertex
 * @return int 
 */
vertexID_t GreedyEdgeSort::rep(vertexID_t i){
    if(m_corr_list[i] == -1){
        return i;
    }else{
        return rep(m_corr_list[i]);
    }
}

/**
 * @brief displays the order followed by the heuristic
 * 
 */
void GreedyEdgeSort::display_order(){
    for(edgeID_t i = 0; i < best_order.size()-1; i++){
        std::cout << best_order[i] << " - ";
    }
    std::cout << best_order.back() << std::endl;
}

// GreedyEdgeSort::GreedyEdgeSort(int rdmax, int rdmin) : Algorithm(rdmax, rdmin){
//TODO:
// }

void GreedyEdgeSort::init(Network& network){
    set_limit_dim(network.n_edge);
    //m_network = /*std::as_const()*/ &network;
    //param of the network
    dim = network.dimension;
    n_vertex = network.n_vertex;

    //adjacence matrix and list of edges
    m_adjacence_matrix = network.adjacence_matrix;
    m_edge_list = network.edge_list;

    m_corr_list.clear();
    m_corr_list.resize(n_vertex, -1);

    best_cost = numeric_limits<cost_t>::max()-1;
    best_order.clear();

    //computes the scores
    for(int i = 0; i < m_edge_list.size(); i++){
        int j = m_edge_list[i].first;
        int k = m_edge_list[i].second;
        m_sorted_scores.push_back(make_pair(i, m_adjacence_matrix[n_vertex*j  + k]/ (float) ext_cost(j, k)));
    }

    //sort by descending order
    sort(m_sorted_scores.begin(), m_sorted_scores.end(), [](pair<edgeID_t, double> a, pair<edgeID_t, double> b){return a.second > b.second;});
}

cost_t GreedyEdgeSort::call_solve(){
    solve();
    return best_cost;
}
