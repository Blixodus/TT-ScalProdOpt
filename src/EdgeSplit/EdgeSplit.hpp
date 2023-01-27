#ifndef EdgeSplit_HPP
#define EdgeSplit_HPP
#include "../Components.hpp"
#include "../TriScore/TriScore.hpp"

class ESplit : public Algorithm{
    public:
    vector<pair<int, int>> E; // la liste des arêtes
    CostTab A; //(size*(size+1)) stock le poids sortant des sommets en fonction de la taille du système, ainsi que le poids du système
    CostTab W; //(3*size/2 - 1) stock le poids des arêtes
    unordered_map<unsigned long long, Cost> M; //map servant à mémoïser les coûts (TODO: renommer en C)

    Cost solve(int i1, int i2, int j1, int j2, CostTab A);
    Cost solve_vertical(int i1, int i2, int j1, int j2, unsigned long long key, CostTab A, Cost S_cost);
    Cost solve_diag(int i1, int i2, int j1, int j2, unsigned long long key, CostTab A, Cost S_cost);

    void ext_cost(int i1, int i2, int j1, int j2, CostTab A, Cost& s);

    unsigned long long convert(int i1, int i2, int j1, int j2);

    int D; //les dimensions (pas indispensable)

    void display_order();
    
    void init(string file);
    Cost call_solve();
};

#endif