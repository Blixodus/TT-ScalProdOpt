/**
 * @brief 
 * Iterates over the dimensions,
 * Splits the TT into 2 sub-problems of size i and D-i.
 * Type : Heuristic
 * Complexity : 
 */
#ifndef RANGESPLITSOLVER_HPP
#define RANGESPLITSOLVER_HPP
//We should technically include EVERY other algorithm
#include "../Components/Components.hpp"
#include "../AllSplits/AllSplits.hpp"

class SplitsDBD : public Algorithm{
    public:
    //Tab G; //la matrice d'adjacence + 1 colonne donnant le poids sortant de chaque sommet
    //Tab m_adjacence_matrix;
    matrix_weight_t m_adjacence_matrix;

    //Tab S; //un état (ensemble de sommets)
    //Tab m_state;
    vector_vertexID_t m_state;

    //Tab A; //un tableau donnant pour chaque taille d'état le poids sortant des sommets
    //Tab m_ext_cost_tab;
    vector_weight_t m_ext_cost_tab;

    //unordered_map<unsigned long long, cost_t> C;//vector<cost_t> C; //la liste des coûts obtenus
    unordered_map<unsigned long long, cost_t> m_cost_memo;
    
    //unordered_map<unsigned long long, int> P1; //vector<long int> P1; //la liste des ordres state1
    //unordered_map<unsigned long long, int> m_order_map1;
    unordered_map<unsigned long long, int> m_order_map1;

    unordered_map<unsigned long long, int> m_order_map2;//vector<long int> P2; //la liste des ordres state2

    // AllSplits solverGreedy;
    AllSplits m_exact_solver;

    //A : Un tableau de taille n*n, le poids des arrêtes sortantes de chaque sommet, pour les n états state d'un "plongeon"
    //G : un tableau de taille n+1*n, la matrice d'adjacence + la colonne A de départ. M[i][j] = G[i*size+j]
    //state : Une liste de sommet, les sommets sélectionné pour cet état
    //C : une map associant au code binaire de chaque état un cout
    //P : une map associant au code binaire de chaque état un ordre

    SplitsDBD(){}
    SplitsDBD(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    //initialiseurs
    void init(Network& network); //initialise G, A, et state (state est simplement la liste des sommets au départ)
    cost_t solve(vector_vertexID_t state); //calcule le coût
    cost_t call_solve();

    //Renvoie A mis à jour pour le state actuel
    vector_weight_t compute_ect(vector_vertexID_t state);

    //renvoie le coût associé aux arètes liant directement state1 et state2 (les arètes non sortantes)
    cost_t cut(vector_vertexID_t state1, vector_vertexID_t state2);
    //renvoie le coût sortant de state
    cost_t produit_sortant(vector_vertexID_t state, vector_vertexID_t ext_cost_tab);

    //converti un ensemble de sommets en un entier pouvant être stocké dans une map
    long int convert(vector_vertexID_t state);
    //converti une clé en l'ensemble de sommets correspondant
    vector_vertexID_t recover(unsigned long long key);
    vector_vertexID_t recover_full(vector_vertexID_t state);

    void display_order();
    void display_order(vector_vertexID_t state);
};

#endif