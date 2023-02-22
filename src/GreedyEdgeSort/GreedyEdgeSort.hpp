/**
 * @brief 
 * Iterates over the edges, 
 * Sorts the edges by weight^2/contraction_cost (descending order) at each iteration
 * Type : Greedy
 * Complexity : D
 */
#ifndef TRISCORE_HPP
#define TRISCORE_HPP
#include "../Components/Algorithm.hpp"

class GreedyEdgeSort : public Algorithm{
    public:
    //1D Adjacence matrix
    matrix_weight_t m_adjacence_matrix;

    //Sorted list of edgeID and score
    std::vector<std::pair<edgeID_t, double>> m_sorted_scores;

    //List of edge (vertexID-vertexID pairs)
    std::vector<std::pair<vertexID_t, vertexID_t>> m_edge_list;

    //Correspondance list, a contracted vertex links to its representant
    //i.e. corr_list[v]==-1 or v's representant
    vector_vertexID_t m_corr_list;

    GreedyEdgeSort(){}
    GreedyEdgeSort(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}
    
    cost_t solve();
    //met à jour la liste des score après suppression d'une arète
    void updateRatio();
    //calcule le coût associé à une arête (i, k)
    cost_t ext_cost(vertexID_t i, vertexID_t k);
    void contract(vertexID_t i, vertexID_t k);
    vertexID_t rep(vertexID_t i);

    void display_order();

    void init(Network& network);
    // void init(string file);
    cost_t call_solve();
};
#endif