#include "SimpleG.hpp"

Cost SimpleG::solve(SouG& sg){
    //encodage de l'ensemble d'arête
    int key = get_key(sg.S);
    //copie du sous-graphe d'entrée pour pouvoir le modifier
    SouG sgref = sg; 

    //cas où il reste plusieurs arêtes, et le coût de l'ensemble n'a pas encore été calculé
    if(sg.S.size() > 1 && C.find(key) == C.end()){
        C[key] = best_cost+1;

        for(int i = 0; i < sgref.S.size(); i++){
            //on copie la copie, et on laisse l'arête en cours de côté
            SouG sg2 = sgref;
            sg2.S.erase(sg2.S.begin() + i);

            //solve nous donne le meilleur coût pour cet ensemble d'arête, et mettra à jour G et V
            Cost cost = solve(sg2);
            //on contract finalement l'arête mise de côté
            cost += contract(sgref.S[i], sg2);
            if(cost < C[key] && cost > 0){
                //mémoïsation
                C[key] = cost;
                O[key] = sgref.S[i];
                //mise à jour de SG pour faire remonter G et V
                sg.G = sg2.G; 
                sg.V = sg2.V;
            }
        }
    }else if(C.find(key) == C.end()){
        //si il ne reste qu'une arête mais que le coût n'a pas été calculé
        C[key] = contract(sg.S[0], sg);
        O[key] = sg.S[0];
    }else{
        //cas où C[key] est déjà calculé, on a quand même besoin de connaître l'état du graphe suite aux contractions
        //on connait l'ensemble des arêtes qui ont été contractées : sg.S
        //on peut bruteforce les S.size() contractions, puisqu'on a déjà le meilleur coût on se fiche de l'ordre
        for(int i : sg.S){
            cheap_contract(i, sg);
        }
    }

    return C[key];
}

/**
 * @brief computes the cost of a contraction and modifies the sub-graph (V and G)
 * 
 * @param i the index (in E) of the contracted edge
 * @param sg the sub-graph
 * @return Cost 
 */
Cost SimpleG::contract(int i, SouG& sg){
    //on va chercher les sommets avec lesquels l'arête i est réellement en contact dans le graphe
    int a = sg.C(E[i].first);
    int b = sg.C(E[i].second);

    if(a != b){
        //calcul du coût
        int res = sg.G[n_vertex*b + a];

        for(int j = 0; j < n_vertex; j++){
            if(b != j){
                res *= max(1, sg.G[n_vertex*a + j]);
            }

            if(a != j){
                res *= max(1, sg.G[n_vertex*b + j]);
            }
        }

        //mise à jour de G
        for(int j = 0; j < n_vertex; j++){
            sg.G[n_vertex*a + j] *= sg.G[n_vertex*b + j];
            sg.G[n_vertex*b + j] = 0;
            sg.G[n_vertex*j + b] = 0;
            sg.G[n_vertex*j + a] = sg.G[n_vertex*a + j];
        }

        //mise à jour de V
        sg.V[b] = a;
        return res;
    }else{
        return 0;
    }
}

/**
 * @brief same as contract, but doesn't compute the cost (only updates G and V)
 * 
 * @param i 
 * @param sg 
 */
void SimpleG::cheap_contract(int i, SouG& sg){
    int a = sg.C(E[i].first);
    int b = sg.C(E[i].second);

    if(a != b){
        for(int j = 0; j < n_vertex; j++){
            sg.G[n_vertex*a + j] *= sg.G[n_vertex*b + j];
            sg.G[n_vertex*b + j] = 0;
            sg.G[n_vertex*j + b] = 0;
            sg.G[n_vertex*j + a] = sg.G[n_vertex*a + j];
        }
        sg.V[b] = a;
    }
}

/**
 * @brief retrieves the vertex thats is the reference for i in the graph
 * 
 * @param i a vertex
 * @return int 
 */
int SouG::C(int i){
    //on parcours la "chaîne de sommet" jusqu'à arriver au sommet de référence (normalement celui ayant le plus petit indice)
    while(V[i] != -1){
        i = V[i];
    }
    return i;
}

/**
 * @brief encodes a set of edges as a unique int key
 * 
 * @param S 
 * @return int 
 */
unsigned long long SimpleG::get_key(Tab S){
    unsigned long long res = 0;
    for(int i : S){
        res += pow(2, i);
    }
    return res-1;
}

/**
 * @brief displays the best order given a starting state encoded as a key
 * 
 * @param key 
 */
void SimpleG::display_order(unsigned long long key){
    int i = O[key]; //i = la meilleure arête à contracter pour l'état key-2^i + 1
    unsigned long long next = key-pow(2, i);
    if(key == get_key(S)){
        display_order(next);
        cout << i << '\n';
    }else if(next >= 0){
        display_order(next);
        cout << i << " - ";
    }else if(next == -1){
        cout << i << " - ";
    }
}

void SimpleG::display_order(){
    for(int i = 0; i < best_order.size()-1; i++){
        cout << best_order[i] << " - ";
    }
    cout << best_order.back() << '\n';

}

void SimpleG::get_order(unsigned long long key){
    int i = O[key];

    unsigned long long next = key-pow(2, i);

    if(next+1 > 0){
        get_order(next);
        best_order.push_back(i);
    }else{
        best_order.clear();
        best_order.push_back(i);
    }
}

void SimpleG::init(string file){
    G.clear();
    E.clear();
    O.clear();
    C.clear();
    S.clear();

    best_cost = numeric_limits<Cost>::max() - 1;
    best_order.clear();

    ifstream ifile(file);
    string line;
    int i, j, w;
    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
            case 'p':
                n_vertex = atoi(&line[2]);
                G.resize(n_vertex*n_vertex, 1);
                S.resize(3*n_vertex/2 - 2);
                //O.resize(pow(2, 3*size/2-2)-1, -1);
                //C.resize(pow(2, 3*size/2-2)-1, -1);
            break;
            case 'e':
                flux >> i >> j >> w;
                E.push_back(make_pair(i, j));
                G[n_vertex*i + j] = w;
                G[n_vertex*j + i] = w;
            break;
            default:
            break;
        }
    }
    for(int i = 0; i < n_vertex; i ++){
        G[n_vertex*i + i] = 0;
    }

    sort_edges(E);

    for(int i = 0; i < E.size(); i++){
        S[i] = i;
    }

    sgref = getSG();
}

Cost SimpleG::call_solve(){
    Cost c = solve(sgref);
    get_order(get_key(S));
    return c;
}