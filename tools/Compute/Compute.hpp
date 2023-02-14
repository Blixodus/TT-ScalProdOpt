#ifndef TEST_HPP
#define TEST_HPP
// #include "../../src/Components/Components.hpp"
#include "../../src/Components/Network.hpp"
#include <fstream>
//prend un fichier texte situé dans le répertoire 'calc' en entrée
//le fichier de test par défaut lorsqu'aucun paramètre n'est passé est 'calc/test.txt'
//une ligne commençant par un 'f' permet de charger un fichier d'instance
//une ligne commençant par un 'o' décrit un ordre à calculer
//les ordres sont représentés par des suites de nombre séparés par des espaces, les nombres correspondent aux arêtes du TT

void import_calc_file(std::string& calc_file);

cost_t compute_order(std::string& filename, Tab& _order_list);
cost_t compute_order(Network& network, Tab& order_list);

struct Cost_cpt{
    public:
    int n_vertex;
    std::vector<std::pair<int, int>> m_edge_list;
    Tab m_adjacence_matrix;
    Tab m_corr_list;

    cost_t contract(int edge);
    int rep(int vertex);

    Cost_cpt(Network& network, Tab& order_list) : 
    n_vertex(network.n_vertex),
    m_edge_list(network.edge_list),
    m_adjacence_matrix(network.adjacence_matrix)
    {m_corr_list.resize(network.n_vertex, -1);}
};

#endif