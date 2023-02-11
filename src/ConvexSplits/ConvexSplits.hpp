/**
 * @brief
 * Iterates over every possible convex-splits (in the geometrical sense) of the TT.
 * Type : Heuristic
 * Complexity : 
 */
#ifndef EdgeSplit_HPP
#define EdgeSplit_HPP
#include "../Components/Components.hpp"
#include "../GreedyEdgeSort/GreedyEdgeSort.hpp"

//TODO: finish refactorisation, most notably of init(Network& network)


class ConvexSplits : public Algorithm{
    public:
    //vector<pair<int, int>> E; // la liste des arêtes
    std::vector<std::pair<int, int>> m_edge_list;

    //CostTab A; //(size*(size+1)) stock le poids sortant des sommets en fonction de la taille du système, ainsi que le poids du système
    CostTab m_ext_cost_tab;

    //CostTab W; //(3*size/2 - 1) stock le poids des arêtes
    CostTab m_weight_list;

    //unordered_map<unsigned long long, cost_t> M; //map servant à mémoïser les coûts (TODO: renommer en C)
    unordered_map<unsigned long long, cost_t> m_cost_memo;

    cost_t solve(int i1, int i2, int j1, int j2, CostTab ext_cost_tab);
    cost_t solve_vertical(int i1, int i2, int j1, int j2, unsigned long long key, CostTab ext_cost_tab, cost_t S_cost);
    cost_t solve_diag(int i1, int i2, int j1, int j2, unsigned long long key, CostTab ext_cost_tab, cost_t S_cost);

    void ext_cost(int i1, int i2, int j1, int j2, CostTab ext_cost_tab, cost_t& s);

    unsigned long long convert(int i1, int i2, int j1, int j2);

    void display_order();
    
    void init(Network& network);
    cost_t call_solve();
};

#endif