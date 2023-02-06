#ifndef MATRIXSOLVER_HPP
#define MATRIXSOLVER_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

/**
 * @brief algorithme calculant le meilleur coût en testant toutes les paires possibles d'une matrice d'adjacence
 * Complexité (N!)^2
 */

//TODO: Remove this type, define it as a 1D vector, and rework the algorithm
typedef vector<vector<int>> Network; 

Network contract(int i, int j, Network N);
Cost contractionCost(int i, int j, Network N);
void display(Network N);

class MatrixSolver : public Algorithm{
    public:
        Network network; //matrice d'adjacence

        void solve(int n, Cost cost, vector<pair<int, int>> v, Network N);
        void display_order();
        
        void init(string file);
        Cost call_solve();
};

#endif