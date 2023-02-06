#include "TriScoreM.hpp"


/**
 * @brief 
 * 
 * @param cr the remaining credits
 * @param s the index we are considering
 * @param n the amount of indexes left to place
 */
void TriScoreM::solve(int cr, int s){
    //si il nous reste du crédit
    if(cr > 0){
        //pour toutes les distances qui sont inférieures ou égales au crédit
        for(int d = 0; d <= cr; d++){
            //si l'emplacement vers la gauche à cette distance est valide 
            if(s >= d && R[s-d]== -1){
                R[s-d] = s;
                VB[s] = 1;
                
                //si on a encore des arêtes à placer après celle-là, on poursuit le parcours
                if(s < E.size()-1){
                    //si l'arête à l'indice où on a atterri n'a pas encore été placée, on poursuit l'exécution sur elle
                        solve(cr-d, s+1);
                }else{
                    //Si on a plus d'arêtes à placer, on converti le tableau R en un ordre, puis on calcule
                    follow_order(generate_order(R));
                }
                //on remet les tableaux à l'état précédent
                R[s-d] = -1;
                VB[s] = 0;
            }

            //même chose pour le côté droit
            if(s+d < R.size() && R[s+d]==-1){
                R[s+d] = s;
                VB[s] = 1;
                if(s > E.size()-1){
                    solve(cr-d, s+1);
                }else{
                    follow_order(generate_order(R));
                }
                R[s+d] = -1;
                VB[s] = 0;
            }
        }
    }else{
        Tab R_copy (R);
        for(int i = s; i < E.size(); i++){
            if(R_copy[i] != -1){
                break;
            }
            R_copy[i] = i;
        }
        /*if(place_to_default(R_copy)){
            follow_order(generate_order(R_copy));
        }*/
    }
}

/**
 * @brief Computes a cost given an order of contraction to follow
 * 
 * @param S the list of edges to contract
 * @return Cost 
 */
void TriScoreM::follow_order(Tab S){
    SousG sg = getSG();
    Cost cost = 0;
    bool still_up = true;
    for(int i : S){
        cost += contract(i, sg);
        if(cost >= best_cost /*&& best_cost != -1*/){
            still_up = false;
            break;
        }
    }
    if(still_up && cost < best_cost){
        best_cost = cost;
        best_order = S;
    }
}

/**
 * @brief Computes the cost of contracting an edge i in the graph G, and updates G and V
 * 
 * @param i 
 * @param sg a sub-graph
 * @return Cost 
 */
Cost TriScoreM::contract(int i, SousG &sg){
    int a = sg.C(E[i].first);
    int b = sg.C(E[i].second);

    if(a != b){
        int res = sg.adjacence_matrix[n_vertex*a + b];
        for(int j = 0; j < n_vertex; j++){
            if(a != j){
                res *= max(1, sg.adjacence_matrix[n_vertex*b + j]);
            }
            if(b != j){ 
                res *= max(1, sg.adjacence_matrix[n_vertex*a + j]);
            }
        }

        for(int j = 0; j < n_vertex; j++){
            sg.adjacence_matrix[n_vertex*a + j] *= sg.adjacence_matrix[n_vertex*b + j];
            sg.adjacence_matrix[n_vertex*b + j] = 0;
            sg.adjacence_matrix[n_vertex*j + b] = 0;
            sg.adjacence_matrix[n_vertex*j + a] = sg.adjacence_matrix[n_vertex*a + j];
        }
        sg.V[b] = a;
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
int SousG::C(int i){
    while(V[i] != -1){
        i = V[i];
    }
    return i;
}


bool TriScoreM::is_still_in(int s){
    return !VB[s];
}

/**
 * @brief returns a vector containing all the edges remaining to be placed
 * 
 * @return Tab 
 */
Tab TriScoreM::still_in(){
    Tab res;
    for(int i = 0; i < VB.size(); i++){
        if(!VB[i]){
            res.push_back(i);
        }
    }
    return res;
}

bool TriScoreM::place_to_default(Tab& R){
    for(int i : still_in()){
        if(R[i] == -1){
            R[i] = i;
        }else{
            return false;
        }
    }
    return true;
}

Tab TriScoreM::generate_order(Tab R){
    Tab res;
    for(int i : R){
        res.push_back(triscore.best_order[i]);
    }
    return res;
}

void TriScoreM::display_order(){
    for(int i = 0; i < best_order.size()-1; i++){
        cout << best_order[i] << " - ";
    }
    cout << best_order.back() << '\n';
}

void TriScoreM::init(string file){
    triscore.init(file);
    G.clear();
    E.clear();
    R.clear();
    VB.clear();
    //best_cost = -1;
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
                delta = min(max(refdelta, 0), 3*n_vertex/2 - 2);
                R.resize(3*n_vertex/2 - 2, -1);
                VB.resize(3*n_vertex/2 - 2, false);
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

    best_cost = triscore.solve();
    best_order = triscore.best_order;

    for(int i = 0; i < n_vertex; i ++){
        G[n_vertex*i + i] = 0;
    }

    sort_edges(E);
}

Cost TriScoreM::call_solve(){
    solve(delta, 0);
    /*for(int i : generate_order({1, 0, 6, 3, 4, 5, 2})){
        cout << i << '\n';
    }*/
    return best_cost;
}