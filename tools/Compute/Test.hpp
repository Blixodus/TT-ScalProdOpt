#ifndef TEST_HPP
#define TEST_HPP
#include "../../src/Components/Components.hpp"
#include "../../src/Components/Algorithm.hpp"
#include <fstream>
#include <sstream>
//prend un fichier texte situé dans le répertoire 'calc' en entrée
//le fichier de test par défaut lorsqu'aucun paramètre n'est passé est 'calc/test.txt'
//une ligne commençant par un 'f' permet de charger un fichier d'instance
//une ligne commençant par un 'o' décrit un ordre à calculer
//les ordres sont représentés par des suites de nombre séparés par des espaces, les nombres correspondent aux arêtes du TT

string path_test;
string path_instance;

int size;
vector<pair<int, int>> E;

Tab G;
Tab O;

void import_instance(string file);
Cost follow_order(Tab O);

class Graph{
    public:
    Tab G;
    Tab V;

    Cost contract(int i);
    int C(int i);

    void init();
};

#endif