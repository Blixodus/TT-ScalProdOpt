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
    Tab m_state;
    Tab m_adjacence_matrix;
    Tab m_corr_list;

    int rep(int i);
    SouG(){}
    SouG(Tab state, Tab adjacence_matrix, Tab corr_list) : m_state(state), m_adjacence_matrix(adjacence_matrix), m_corr_list(corr_list) {}
    //void set(Tab edge_list, Tab AM, Tab V){this->S = S; this->adjacence_matrix = AM; this->V = V;};
};

class AllEdgeByEdge : public Algorithm{
    public:
    SouG sgref; //structure avec son propre graphe qu'on peut modifier librement
    // vector<pair<int, int>> E; //liste des arêtes (fixe)
    std::vector<std::pair<int, int>> m_edge_list;
    //Tab G; //size*(size+1)
    Tab m_adjacence_matrix;

    //Tab S; //les indices des arêtes
    Tab m_state;

    //unordered_map<unsigned long long, int> O;
    unordered_map<unsigned long long, int> m_order_map;

    //unordered_map<unsigned long long, Cost> C;
    unordered_map<unsigned long long, cost_t> m_cost_memo;

    AllEdgeByEdge(){}
    AllEdgeByEdge(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}
    
    SouG getSG(){return SouG(m_state, m_adjacence_matrix, vector<int> (n_vertex, -1));}
    cost_t contract(int i, SouG& sg);
    void cheap_contract(int i, SouG& sg);

    cost_t solve(SouG& sg);

    unsigned long long get_key(Tab state);

    void display_order(unsigned long long key);
    void display_order();
    void get_order(unsigned long long key);

    void init(Network& network);
    cost_t call_solve();
};
#endif
