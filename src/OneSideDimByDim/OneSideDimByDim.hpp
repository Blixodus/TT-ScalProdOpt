/**
 * @brief 
 * Iterates over the edges, 
 * Explores the solutions by going from one side of the TT to the other.
 * Type : Heuristic
 * Complexity : 
 */
#ifndef SIDEEXSOLVER_HPP
#define SIDEEXSOLVER_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"
//TODO: Finish refactorization, put more explicit names and finish init()
class OneSideDBD : public Algorithm{
    public:
    //(on rappelle que D = size/2)
    //Tab A; //poids sortant (taille size);
    Tab m_ext_cost_tab;

    //Tab G; //matrice d'adjacence (taille size*(size+1))
    Tab m_adjacence_matrix;

    Tab T; //tableau stockant les ti (taille 2(D-1))

    Tab P; //size
    Tab C; //tableau stockant les coûts ci (taille 2(D-1)), C[i] = cout pour obtenir T[i]
    
    Tab Z; //size/2, donne la référence de R et S
    vector<pair<int, int>> O; //tableau stockant l'ensemble des pairs de contractions donnant des coûts minimum

    cost_t solve();

    cost_t contract(int s, int i, int x, pair<int, int>& p);
 
    void compute_ect(int s, int k);
    void restore_ect(int s);
    void display_order(int s, int k);
    void display_order();
    void get_order(int s, int k);

    void init(Network& network);
    cost_t call_solve();
};
#endif