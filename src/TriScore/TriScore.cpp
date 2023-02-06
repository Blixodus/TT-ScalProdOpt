#include "TriScore.hpp"

/**
 * @brief solves G
 * 
 * @return int 
 */
Cost TriScore::solve(){
    best_cost = 0;
    //tant qu'il reste des arêtes
    while(!R.empty()){
        best_order.push_back(R.front().first);
        //les vraies extrémités de l'arête
        int i = C(E[R.front().first].first);
        int k = C(E[R.front().first].second);
        if(i != k){
            //ajoute le coût de contraction de R.front à cost
            best_cost += ext_cost(i, k)*G[n_vertex*i + k];
            contract(i, k);
        }
        //suppression de l'arête
        R.erase(R.begin());

        //update du tableau des ratio
        updateRatio();
    }
    return best_cost;
}

/**
 * @brief updates the sorted list of scores R
 * Sub-optimal, but time is not an issue here
 */
void TriScore::updateRatio(){
    for(auto& p : R){
        int i = C(E[p.first].first);
        int k = C(E[p.first].second);
        p.second = G[n_vertex*i + k]/(float) ext_cost(i, k);
    }
    sort(R.begin(), R.end(), [](pair<int, double> a, pair<int, double> b){return a.second > b.second;});
}

/**
 * @brief Computes the external cost of 2 vertex
 * 
 * @param i 
 * @param k 
 * @return Cost 
 */
Cost TriScore::ext_cost(int i, int k){
    int res = 1;
    for(int j = 0; j < n_vertex; j++){
        if(k != j){
            res *= max(1, G[n_vertex*i + j]);
        }
        if(i != j){ 
            res *= max(1, G[n_vertex*k + j]);
        }
    }

    return res;
}

/**
 * @brief contracts an edge by modifying G and V accordingly
 * 
 * @param i
 * @param k
 */
void TriScore::contract(int i, int k){
    for(int j = 0; j < n_vertex; j++){
        G[n_vertex*i + j] *= G[n_vertex*k + j];
        G[n_vertex*k + j] = 0;
        G[n_vertex*j + k] = 0;
        G[n_vertex*j + i] = G[n_vertex*i + j];
    }
    V[k] = i;
}

/**
 * @brief returns i's vertex of reference in the graph
 * 
 * @param i 
 * @return int 
 */
int TriScore::C(int i){
    if(V[i] == -1){
        return i;
    }else{
        return C(V[i]);
    }
}

/**
 * @brief displays O, the order followed by the heuristic
 * 
 */
void TriScore::display_order(){
    for(int i = 0; i < best_order.size()-1; i++){
        cout << best_order[i] << " - ";
    }
    cout << best_order.back() << '\n';
}

void TriScore::init(string file){
    G.clear();
    R.clear();
    V.clear();
    E.clear();
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
                V.resize(n_vertex, -1);
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
        int j = E[i].first;
        int k = E[i].second;
        R.push_back(make_pair(i, G[n_vertex*j  + k]/ (float) ext_cost(j, k)));
    }

    sort(R.begin(), R.end(), [](pair<int, double> a, pair<int, double> b){return a.second > b.second;});
}

Cost TriScore::call_solve(){
    return solve();
}

void get_approx_solution(int& cost, Tab& O, string file){
    TriScore solver;
    solver.init(file.c_str());
    solver.solve();
    cost = solver.best_cost;
    O = solver.best_order;
}