#include "MatrixSolver.hpp"

/**
 * @brief generates all permutations recursively
 * 
 * @param n the depth (amount of remaining tensors)
 * @param cost the current cost
 * @param v the current order
 * @param N the current network
 */
void MatrixSolver::solve(int n, Cost cost, vector<pair<int, int>> v, Network N){
    for(int j = 1; j < n; j++){
        for(int i = 0; i < j; i++){
            //on copie l'ordre actuel
            vector<pair<int, int>> v2 = v;
            //on rajoute l'arête en considération l'ordre
            v2.push_back(make_pair(i, j));
            //on continue d'itérer
            solve(n-1, cost + contractionCost(i, j, N), v2, contract(i, j, N));
        }
    }
    if(n <= 1 && cost <= bestCost && cost > 0){
        bestCost = cost;
        //bestOrder = v;
    }
}

/**
 * @brief contracts an edge in the matrix N and returns the newly formed matrix
 * 
 * @param i 
 * @param j 
 * @param N
 * @return Network
 */
Network contract(int j, int i, Network N){
    for(unsigned int k = 0; k < N.size(); k++){
        //migration des poids vers la j-ième ligne/colonne
        N[k][j] *= N[k][i]; //TODO: trouver une alternative à la matrice symétrique
        N[j][k] *= N[k][i];
    }
    for(unsigned int k = 0; k < N.size(); k++){
        N[k].erase(N[k].begin() + i);
    }
    //suppression de la i-ième colonne
    N.erase(N.begin() + i);
    return N;
}

/**
 * @brief calculates the contractions cost of the edge on (i, j) in the matrix N
 * 
 * @param i 
 * @param j 
 * @param N
 * @return int 
 */
Cost contractionCost(int i, int j, Network N){
    int cost = N[i][j];
    for(unsigned int k = 0; k < N.size(); k++){
        cost *= max(1, N[k][j]*N[k][i]);
    }
    return cost;
}

/**
 * @brief initializes the network based on a textfile
 * 
 * @param path a path to a textfile
 */
void MatrixSolver::init(string file){
    bestOrder.clear();

    bestCost = numeric_limits<Cost>::max() - 1;
    for(int i = 0; i < network.size(); i ++){
        network[i].clear();
    }
    network.clear();
    ifstream ifile(file);
    string line;
    int i, j, w;
    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
            case 'p':
                size = atoi(&line[2]);
                //bestOrder.resize(size);
                network.resize(size);
                for(int k = 0; k < size; k ++){
                    network[k].resize(size, 1);
                    network[k][k] = 0;
                }
            break;
            case 'e':
                flux >> i >> j >> w;
                network[i][j] = w;
                network[j][i] = w;
            break;
            default:
            break;
        }
    }
}

/**
 * @brief displays bestOrder
 * 
 */
void MatrixSolver::display_order(){
    /*for(int k = 0; k < bestOrder.size(); k++){
        cout << "(" << bestOrder[k].first << ", " << bestOrder[k].second << ")||";
    }*/
}

Cost MatrixSolver::call_solve(){
    solve(size, 0, {}, network);
    return bestCost;
}