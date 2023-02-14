#include "MatrixSolver.hpp"

/**
 * @brief generates all permutations recursively
 * 
 * @param n the depth (amount of remaining tensors)
 * @param cost the current cost
 * @param v the current order
 * @param N the current m_network
 */
void MatrixSolver::solve(int n, cost_t cost, vector<pair<int, int>> v, network_t N){
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
    if(n <= 1 && cost <= best_cost && cost > 0){
        best_cost = cost;
        //bestOrder = v;
    }
}

/**
 * @brief contracts an edge in the matrix N and returns the newly formed matrix
 * 
 * @param i 
 * @param j 
 * @param N
 * @return network_t
 */
network_t contract(int j, int i, network_t N){
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
cost_t contractionCost(int i, int j, network_t N){
    int cost = N[i][j];
    for(unsigned int k = 0; k < N.size(); k++){
        cost *= max(1, N[k][j]*N[k][i]);
    }
    return cost;
}

/**
 * @brief initializes the network based on a network
 * 
 * @param path a path to a textfile
 */
void MatrixSolver::init(Network& network){
    set_limit_dim(network.n_vertex);
    best_order.clear();

    best_cost = numeric_limits<cost_t>::max() - 1;
    for(int i = 0; i < m_network.size(); i ++){
        m_network[i].clear();
    }
    m_network.clear();
    ifstream ifile("instances/" + network.m_filename);
    string line;
    int i, j, w;
    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
            case 'p':
                n_vertex = atoi(&line[2]);
                //bestOrder.resize(size);
                m_network.resize(n_vertex);
                for(int k = 0; k < n_vertex; k ++){
                    m_network[k].resize(n_vertex, 1);
                    m_network[k][k] = 0;
                }
            break;
            case 'e':
                flux >> i >> j >> w;
                m_network[i][j] = w;
                m_network[j][i] = w;
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

cost_t MatrixSolver::call_solve(){
    solve(n_vertex, 0, {}, m_network);
    return best_cost;
}