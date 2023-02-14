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
/**
 * @brief Executes the solver on an instance file, and stores the resulting cost and order, for another algorithm to use
 * 
 * @param cost 
 * @param order
 * @param file 
 */
void get_approx_solution(int& cost, Tab& order, Network& network);

class GreedyEdgeSort : public Algorithm{
    public:
    //Tab G; //une matrice d'adjacence
    //1D Adjacence matrix
    Tab m_adjacence_matrix;

    //vector<pair<int, double>> R; //La liste des scores triés
    //Sorted list of edgeID and score
    std::vector<std::pair<int, double>> m_sorted_scores;

    //vector<pair<int, int>> E; //la liste des arêtes
    //List of edge (vertexID-vertexID pairs)
    std::vector<std::pair<int, int>> m_edge_list;

    //Tab V; //la correspondance entre les sommets (un genre d'étiquettage)
    //Correspondance list, a contracted vertex links to its representant
    //i.e. corr_list[v]==-1 or v's representant
    Tab m_corr_list;

    GreedyEdgeSort(){}
    GreedyEdgeSort(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}
    
    cost_t solve();
    //met à jour la liste des score après suppression d'une arète
    void updateRatio();
    //calcule le coût associé à une arête (i, k)
    cost_t ext_cost(int i, int k);
    void contract(int i, int k);
    int rep(int i);

    void display_order();

    void init(Network& network);
    // void init(string file);
    cost_t call_solve();
};
#endif