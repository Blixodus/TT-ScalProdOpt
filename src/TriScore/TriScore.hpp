#ifndef TRISCORE_HPP
#define TRISCORE_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

/**
 * @brief Algorithme naïf contractant l'arête maximisant le ratio (poids^2 / coût de contraction) à chaque étape
 */

/**
 * @brief Executes the solver on an instance file, and stores the resulting cost and order, for another algorithm to use
 * 
 * @param cost 
 * @param O 
 * @param file 
 */
void get_approx_solution(int& cost, Tab& O, string file);

class TriScore : public Algorithm{
    public:
    Tab G; //une matrice d'adjacence
    vector<pair<int, double>> R; //La liste des scores triés
    vector<pair<int, int>> E; //la liste des arètes
    Tab V; //la correspondance entre les sommets (un genre d'étiquettage)

    Cost solve();
    //met à jour la liste des score après suppression d'une arète
    void updateRatio();
    //calcule le coût associé à une arète (i, k)
    Cost ext_cost(int i, int k);
    void contract(int i, int k);
    int C(int i);

    void display_order();

    void init(string file);
    Cost call_solve();
};
#endif