/**
 * @brief
 * Modifies a base solution by slightly switching the contraction order
 * Type : 
 * Complexity : 
 */
#ifndef TRISCOREMARGIN_HPP
#define TRISCOREMARGIN_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"
#include "../GreedyEdgeSort/GreedyEdgeSort.hpp"

//TODO: put more explicit member names

class SousG{
    public:
    vector_edgeID_t m_state;
    matrix_weight_t m_adjacence_matrix;
    vector_vertexID_t m_corr_list;

    vertexID_t rep(vertexID_t i);
    SousG(matrix_weight_t am, vector_vertexID_t corr_list) : m_adjacence_matrix(am), m_corr_list(corr_list){}
    SousG(vector_edgeID_t state, matrix_weight_t am, vector_vertexID_t corr_list) : m_state(state), m_adjacence_matrix(am), m_corr_list(corr_list) {}
};

class Shuffle : public Algorithm{
    public:
    GreedyEdgeSort triscore;

    //vector<pair<int, int>> E; //liste d'arêtes
    std::vector<std::pair<edgeID_t, edgeID_t>> m_edge_list;

    //Tab G; //matrice d'djacence
    matrix_weight_t m_adjacence_matrix;

    vector_edgeID_t R; //liste d'indice des arêtes de E : R[0] = 2 signifie que la première arête considérée sera l'arête E[2]
    
    std::vector<bool> VB;

    Shuffle(){}
    Shuffle(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    SousG getSG(){return SousG(m_adjacence_matrix, vector<int> (n_vertex, -1));}
    void solve(int cr, int s);
    void follow_order(vector_edgeID_t S);
    cost_t contract(edgeID_t i, SousG& sg);
    bool is_still_in(int s);
    vector_edgeID_t still_in();
    bool place_to_default(vector_edgeID_t& R);
    vector_edgeID_t generate_order(vector_edgeID_t R);

    void display_order();

    void init(Network& network);
    cost_t call_solve();
};

#endif