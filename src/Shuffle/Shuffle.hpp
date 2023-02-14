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
    Tab m_state;
    Tab m_adjacence_matrix;
    Tab m_corr_list;

    int rep(int i);
    SousG(Tab am, Tab corr_list) : m_adjacence_matrix(am), m_corr_list(corr_list){}
    SousG(Tab state, Tab am, Tab corr_list) : m_state(state), m_adjacence_matrix(am), m_corr_list(corr_list) {}
};

class Shuffle : public Algorithm{
    public:
    GreedyEdgeSort triscore;

    //vector<pair<int, int>> E; //liste d'arêtes
    std::vector<std::pair<int, int>> m_edge_list;

    //Tab G; //matrice d'djacence
    Tab m_adjacence_matrix;

    Tab R; //liste d'indice des arêtes de E : R[0] = 2 signifie que la première arête considérée sera l'arête E[2]
    
    vector<bool> VB;

    Shuffle(){}
    Shuffle(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    SousG getSG(){return SousG(m_adjacence_matrix, vector<int> (n_vertex, -1));}
    void solve(int cr, int s);
    void follow_order(Tab S);
    cost_t contract(int i, SousG& sg);
    bool is_still_in(int s);
    Tab still_in();
    bool place_to_default(Tab& R);
    Tab generate_order(Tab R);

    void display_order();

    void init(Network& network);
    cost_t call_solve();
};

#endif