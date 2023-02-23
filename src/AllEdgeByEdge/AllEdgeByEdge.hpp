/**
 * @brief 
 * Iterates over the edges,
 * Tests every contraction order possible.
 * Type : exact algorithm (allegedly, assuming it is never worth it to contract 2 disconnected vertices)
 * Complexity : 2^3D
 * 
 * A priori pas de problème pour le faire en TOP -> DOWN
 */
#ifndef SIMPLEG_HPP
#define SIMPLEG_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

class SouG{
    public:
    //The edges remaining
    //Tab m_state;
    vector_edgeID_t m_state;

    // Tab m_adjacence_matrix;
    matrix_weight_t m_adjacence_matrix;

    //Tab m_corr_list;
    vector_vertexID_t m_corr_list;

    vertexID_t rep(vertexID_t i);
    SouG(){}
    // SouG(Tab state, Tab adjacence_matrix, Tab corr_list) : m_state(state), m_adjacence_matrix(adjacence_matrix), m_corr_list(corr_list) {}
    SouG(vector_vertexID_t state, matrix_weight_t adjacence_matrix, vector_vertexID_t corr_list) : m_state(state), m_adjacence_matrix(adjacence_matrix), m_corr_list(corr_list) {}
    //void set(Tab edge_list, Tab AM, Tab V){this->S = S; this->adjacence_matrix = AM; this->V = V;};
};

class AllEdgeByEdge : public Algorithm{
    public:
    SouG sgref; //structure avec son propre graphe qu'on peut modifier librement
    // vector<pair<int, int>> E; //liste des arêtes (fixe)
    std::vector<std::pair<vertexID_t, vertexID_t>> m_edge_list;
    //Tab G; //size*(size+1)
    //Tab m_adjacence_matrix;
    matrix_weight_t m_adjacence_matrix;

    //Tab S; //les indices des arêtes
    //Tab m_state;
    vector_edgeID_t m_state;

    //unordered_map<unsigned long long, int> O;
    unordered_map<unsigned long long, edgeID_t> m_order_map;

    //unordered_map<unsigned long long, Cost> C;
    unordered_map<unsigned long long, cost_t> m_cost_memo;

    AllEdgeByEdge(){}
    AllEdgeByEdge(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}
    
    SouG getSG(){return SouG(m_state, m_adjacence_matrix, vector<vertexID_t> (n_vertex, -1));}
    cost_t contract(edgeID_t i, SouG& sg);
    void cheap_contract(edgeID_t i, SouG& sg);

    cost_t solve(SouG& sg);

    unsigned long long get_key(vector_edgeID_t state);

    void display_order(unsigned long long key);
    void display_order();
    void get_order(unsigned long long key);

    void init(Network& network);
    cost_t call_solve();
};
#endif
