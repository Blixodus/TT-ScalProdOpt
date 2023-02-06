#ifndef SIDEEXSOLVER_HPP
#define SIDEEXSOLVER_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

/**
 * @brief Version optimisée de SideIter, n'autorisant pas l'accumulation de plus de DELTA arêtes centrales
 * Peu viable lorsque les arêtes centrales sont de faible dimension 
 */
class SideEx : public Algorithm{
    public:
    //(on rappelle que D = size/2)
    Tab A; //poids sortant (taille size);
    Tab G; //matrice d'adjacence (taille size*(size+1))
    Tab T; //tableau stockant les ti (taille 2(D-1))

    Tab P; //size
    Tab C; //tableau stockant les coûts ci (taille 2(D-1)), C[i] = cout pour obtenir T[i]
    
    Tab Z; //size/2, donne la référence de R et S
    vector<pair<int, int>> O; //tableau stockant l'ensemble des pairs de contractions donnant des coûts minimum

    Cost solve();

    Cost contract(int s, int i, int x, pair<int, int>& p);
 
    void computeA(int s, int k);
    void restoreA(int s);
    void display_order(int s, int k);
    void display_order();
    void get_order(int s, int k);

    void init(string file);
    Cost call_solve();
};
#endif