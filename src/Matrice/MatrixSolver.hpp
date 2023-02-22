#ifndef MATRIXSOLVER_HPP
#define MATRIXSOLVER_HPP
#include "../Components/Components.hpp"
#include "../Components/Network.hpp"
#include "../Components/Algorithm.hpp"

/**
 * @brief algorithme calculant le meilleur coût en testant toutes les paires possibles d'une matrice d'adjacence
 * Complexité (N!)^2
 */

//TODO: Remove this type, define it as a 1D vector, and rework the algorithm
typedef vector<vector<int>> network_t; 

network_t contract(int i, int j, network_t N);
cost_t contractionCost(int i, int j, network_t N);
void display(network_t N);

class MatrixSolver : public Algorithm{
    public:
        network_t m_network; //matrice d'adjacence

        MatrixSolver(){}
        MatrixSolver(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

        void solve(int n, cost_t cost, std::vector<pair<int, int>> v, network_t N);
        void display_order();
        
        void init(Network& network);
        cost_t call_solve();
};

#endif