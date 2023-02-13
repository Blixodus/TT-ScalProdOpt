/**
 * @brief 
 * Iterates over every possible splits of the TT.
 * Type : exact algorithm
 * Complexity : 2^4D
 */
#ifndef SPLITSOLVER_HPP
#define SPLITSOLVER_HPP
#include "../Components/Algorithm.hpp"

class AllSplits : public Algorithm{
    public:
    //CostTab G; //la matrice d'adjacence + 1 colonne donnant le poids sortant de chaque sommet
    CostTab m_adjacence_matrix;

    //Tab S; //un état (ensemble de sommets)
    Tab m_state;

    //CostTab A; //un tableau donnant pour chaque taille d'état le poids sortant des sommets
    CostTab m_ext_cost_tab;
    
    //unordered_map<unsigned long long, cost_t> C; //la liste des coûts obtenus
    std::unordered_map<unsigned long long, cost_t> m_cost_map;

    //unordered_map<unsigned long long, int> P1; //la liste des ordres S1
    unordered_map<unsigned long long, int> m_order_map_1;

    unordered_map<unsigned long long, int> m_order_map_2; //la liste des ordres S2

    //A : Un tableau de taille n*n, le poids des arrêtes sortantes de chaque sommet, pour les n états S d'un "plongeon"
    //G : un tableau de taille n+1*n, la matrice d'adjacence + la colonne A de départ. M[i][j] = G[i*size+j]
    //S : Une liste de sommet, les sommets sélectionné pour cet état
    //C : une map associant au code binaire de chaque état un cout
    //P : une map associant au code binaire de chaque état un ordre

    AllSplits(){}
    AllSplits(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){}

    //initialiseurs
    void init(Network& network);
    //void init(string file); //initialise G, A, et S (S est simplement la liste des sommets au départ)
    cost_t call_solve();
    cost_t solve(Tab state); //calcule le coût

    //Renvoie m_ext_cost_list mis à jour pour le state actuel
    Tab compute_ecl(Tab state);

    //renvoie le coût associé aux arètes liant directement state1 et state2 (les arêtes non sortantes)
    cost_t cut(Tab state1, Tab state2);
    //renvoie le coût sortant de state
    cost_t produit_sortant(Tab state, Tab ext_cost_list);

    //converti un ensemble de sommets en un entier pouvant être stocké dans une map
    unsigned long long convert(Tab state);
    //converti une clé en l'ensemble de sommets correspondant
    Tab recover(unsigned long long key);

    void display_order(Tab state);
    void display_order();
};

#endif