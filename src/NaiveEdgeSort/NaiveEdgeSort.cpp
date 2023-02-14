#include "NaiveEdgeSort.hpp"

cost_t NaiveEdgeSort::solve(){
    best_cost = 0;
    //l'ordre R a été défini à l'initialisation
    for(auto& p : R){
        best_cost += contract(p.first);
    }
    return best_cost;
}

/**
 * @brief Computes the cost of contracting an edge i in the graph G, and updates G and V
 * 
 * @param i 
 * @return cost_t 
 */
cost_t NaiveEdgeSort::contract(int i){
    int a = C(E[i].first);
    int b = C(E[i].second);

    if(a != b){
        int res = G[n_vertex*a + b];
        for(int j = 0; j < n_vertex; j++){
            if(a != j){
                res *= max(1, G[n_vertex*b + j]);
            }
            if(b != j){ 
                res *= max(1, G[n_vertex*a + j]);
            }
        }

        for(int j = 0; j < n_vertex; j++){
            G[n_vertex*a + j] *= G[n_vertex*b + j];
            G[n_vertex*b + j] = 0;
            G[n_vertex*j + b] = 0;
            G[n_vertex*j + a] = G[n_vertex*a + j];
        }
        V[b] = a;
        return res;
    }else{
        return 0;
    }
}

/**
 * @brief returns i's vertex of reference in the graph
 * 
 * @param i 
 * @return int 
 */
int NaiveEdgeSort::C(int i){
    if(V[i] == -1){
        return i;
    }else{
        return C(V[i]);
    }
}

/**
 * @brief Computes the ratio (weight^2/contraction cost) of an edge
 * 
 * @param i 
 * @return double 
 */
double NaiveEdgeSort::ratio(int i){
    int a = E[i].first;
    int b = E[i].second;
    int res = 1;
    for(int j = 0; j < n_vertex; j++){
        if(a != j){
            res *= max(1, G[n_vertex*b + j]);
        }
        if(b != j){ 
            res *= max(1, G[n_vertex*a + j]);
        }
    }

    return G[n_vertex*a + b]/(double) res;
}

void NaiveEdgeSort::display_order(){
    for(int i = 0; i < best_order.size()-1; i++){
        cout << best_order[i] << " - ";
    }
    cout << best_order.back() << '\n';
}

void NaiveEdgeSort::init(Network& network){
    set_limit_dim(network.n_edge);
    G.clear();
    E.clear();
    R.clear();
    V.clear();
    best_order.clear();

    ifstream ifile("instances/" + network.m_filename);
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
                // E.push_back(make_pair(i, j));
                G[n_vertex*i + j] = w;
                G[n_vertex*j + i] = w;
            break;
            default:
            break;
        }
    }

    for(int i = 0; i < n_vertex; i++){
        G[n_vertex*i + i] = 0;
    }

    E = network.edge_list;

    for(int i = 0; i < E.size(); i++){
        R.push_back(make_pair(i, ratio(i)));
    }

    sort(R.begin(), R.end(), [](pair<int, double> a, pair<int, double> b){return a.second > b.second;});

    for(auto& p : R){
        best_order.push_back(p.first);
    }
}

cost_t NaiveEdgeSort::call_solve(){
    return solve();
}