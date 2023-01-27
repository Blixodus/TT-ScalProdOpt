#ifndef SIMPLEG_HPP
#define SIMPLEG_HPP
#include "../Components.hpp"
/**
 * @brief Algorithme glouton calculant le meilleurs coût en ne considérant que les liens effectifs entre les sommets
 * Complexité : 2^3D
 * 
 * A priori pas de problème pour le faire en TOP -> DOWN
 */

class SouG{
    public:
    Tab S;
    Tab G;
    Tab V;

    int C(int i);
    void set(Tab S, Tab G, Tab V){this->S = S; this->G = G; this->V = V;};
};

class SimpleG : public Algorithm{
    public:
    SouG sgref; //structure avec son propre graphe qu'on peut modifier librement
    vector<pair<int, int>> E; //liste des arêtes (fixe)
    Tab G; //size*(size+1)
    Tab S; //les indices des arêtes

    unordered_map<unsigned long long, int> O;
    unordered_map<unsigned long long, Cost> C;

    SouG getSG(){SouG sg; sg.set(S, G, vector<int> (size, -1)); return sg;}
    Cost contract(int i, SouG& sg);
    void cheap_contract(int i, SouG& sg);

    Cost solve(SouG& sg);

    unsigned long long get_key(Tab S);

    void display_order(unsigned long long key);
    void display_order();
    void get_order(unsigned long long key);

    void init(string file);
    Cost call_solve();
};
#endif
