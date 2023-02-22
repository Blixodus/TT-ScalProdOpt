#ifndef TRISCOREN_HPP
#define TRISCOREN_HPP
#include "../Components/Algorithm.hpp"

/**
 * @brief Version alternative naïve de TriScore
 * L'ordre de contraction est calculée au départ et n'est jamais mis à jour
 */

class NaiveEdgeSort : public Algorithm{
    public:
    //Tab G;
    matrix_weight_t G;
    //Tab V;
    vector_vertexID_t V;
    //vector<pair<int, int>> E;
    std::vector<pair<vertexID_t, vertexID_t>> E;
    //vector<pair<int, double>> R;
    std::vector<pair<edgeID_t, double>> R;

    NaiveEdgeSort(){}
    NaiveEdgeSort(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    cost_t solve();
    vertexID_t C(int i);
    cost_t contract(int i);
    double ratio(int i);

    void display_order();

    void init(Network& network);
    cost_t call_solve();
};
#endif