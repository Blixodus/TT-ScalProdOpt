/**
 * @brief 
 * Iterates over every possible splits of the TT.
 * Type : exact algorithm
 * Complexity : 2^4D
 */
#ifndef SPLITSOLVER_HPP
#define SPLITSOLVER_HPP
#include "../Components/Algorithm.hpp"

class CotengraOptimalWrapper : public Algorithm{
    public:
    //size : n_vertex(n_vertex + 1)
    //An adjacence matrix, + 1 column that stores the base external cost associated with each vertex
    //i.e. m_ext_cost_tab[n_vertex]
    matrix_weight_t m_adjacence_matrix;

    //Tab S; //un état (ensemble de sommets)
    //Tab m_state;
    vector_vertexID_t m_state;

    //size : n_vertex*n_vertex
    //The leaving-weights of a vertex in a specific state, for the n_vertex states of a recursive dive
    matrix_weight_t m_ext_cost_tab;
    
    //Map binding the binary key of a state to a cost
    //std::unordered_map<unsigned long long, cost_t> m_cost_map;
    std::unordered_map<double, cost_t> m_cost_map;

    //unordered_map<unsigned long long, int> P1; //la liste des ordres S1
    std::unordered_map<double, int> m_order_map_1;

    std::unordered_map<double, int> m_order_map_2; //la liste des ordres S2

    //G : un tableau de taille n+1*n, la matrice d'adjacence + la colonne A de départ. M[i][j] = G[i*size+j]
    //S : Une liste de sommet, les sommets sélectionné pour cet état
    //P : une map associant au code binaire de chaque état un ordre

    AllSplits(){}
    AllSplits(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    //initialiseurs
    void init(Network& network);
    //void init(string file); //initialise G, A, et S (S est simplement la liste des sommets au départ)
    cost_t call_solve();
    cost_t solve(vector_vertexID_t& state); //calcule le coût

    //Renvoie m_ext_cost_list mis à jour pour le state actuel
    vector_cost_t compute_ecl(vector_vertexID_t const& state);

    //renvoie le coût associé aux arètes liant directement state1 et state2 (les arêtes non sortantes)
    cost_t cut(vector_vertexID_t const& state1, vector_vertexID_t const& state2);
    //renvoie le coût sortant de state
    cost_t produit_sortant(vector_vertexID_t const& state, matrix_weight_t const& ext_cost_list);

    //converti un ensemble de sommets en un entier pouvant être stocké dans une map
    double convert(vector_vertexID_t const& state);
    //converti une clé en l'ensemble de sommets correspondant
    vector_vertexID_t recover(double key);

    void display_order(vector_vertexID_t const& state);
    void display_order();
};

#endif