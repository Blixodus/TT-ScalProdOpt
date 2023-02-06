#ifndef TRISCOREN_HPP
#define TRISCOREN_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

/**
 * @brief Version alternative naïve de TriScore
 * L'ordre de contraction est calculée au départ et n'est jamais mis à jour
 */

class NTS : public Algorithm{
    public:
    Tab G;
    Tab V;
    vector<pair<int, int>> E;
    vector<pair<int, double>> R;

    Cost solve();
    int C(int i);
    Cost contract(int i);
    double ratio(int i);

    void display_order();

    void init(string file);
    Cost call_solve();
};
#endif