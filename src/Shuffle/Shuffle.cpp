#include "Shuffle.hpp"


/**
 * @brief 
 * 
 * @param cr the remaining credits
 * @param s the index we are considering
 * @param n the amount of indexes left to place
 */
void Shuffle::solve(int cr, int s){
    //si il nous reste du crédit
    if(cr > 0){
        //pour toutes les distances qui sont inférieures ou égales au crédit
        for(int d = 0; d <= cr; d++){
            //si l'emplacement vers la gauche à cette distance est valide 
            if(s >= d && R[s-d]== -1){
                R[s-d] = s;
                VB[s] = 1;
                
                //si on a encore des arêtes à placer après celle-là, on poursuit le parcours
                if(s < m_edge_list.size()-1){
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
                if(s > m_edge_list.size()-1){
                    solve(cr-d, s+1);
                }else{
                    follow_order(generate_order(R));
                }
                R[s+d] = -1;
                VB[s] = 0;
            }
        }
    }else{
        vector_edgeID_t R_copy (R);
        for(int i = s; i < m_edge_list.size(); i++){
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
 * @param state the list of edges to contract
 * @return cost_t 
 */
void Shuffle::follow_order(vector_edgeID_t state){
    SousG sg = getSG();
    cost_t cost = 0;
    bool still_up = true;
    for(edgeID_t i : state){
        cost += contract(i, sg);
        if(cost >= best_cost /*&& best_cost != -1*/){
            still_up = false;
            break;
        }
    }
    if(still_up && cost < best_cost){
        best_cost = cost;
        best_order = state;
    }
}

/**
 * @brief Computes the cost of contracting an edge i in the graph m_adjacence_matrix, and updates m_adjacence_matrix and m_corr_list
 * 
 * @param i 
 * @param sg a sub-graph
 * @return cost_t 
 */
cost_t Shuffle::contract(edgeID_t i, SousG &sg){
    vertexID_t a = sg.rep(m_edge_list[i].first);
    vertexID_t b = sg.rep(m_edge_list[i].second);

    if(a != b){
        cost_t res = sg.m_adjacence_matrix[n_vertex*a + b];
        for(vertexID_t j = 0; j < n_vertex; j++){
            if(a != j){
                res *= max((weight_t) 1, sg.m_adjacence_matrix[n_vertex*b + j]);
            }
            if(b != j){ 
                res *= max((weight_t) 1, sg.m_adjacence_matrix[n_vertex*a + j]);
            }
        }

        for(vertexID_t j = 0; j < n_vertex; j++){
            sg.m_adjacence_matrix[n_vertex*a + j] *= sg.m_adjacence_matrix[n_vertex*b + j];
            sg.m_adjacence_matrix[n_vertex*b + j] = 0;
            sg.m_adjacence_matrix[n_vertex*j + b] = 0;
            sg.m_adjacence_matrix[n_vertex*j + a] = sg.m_adjacence_matrix[n_vertex*a + j];
        }
        sg.m_corr_list[b] = a;
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
vertexID_t SousG::rep(vertexID_t i){
    while(m_corr_list[i] != -1){
        i = m_corr_list[i];
    }
    return i;
}


bool Shuffle::is_still_in(int s){
    return !VB[s];
}

/**
 * @brief returns a vector containing all the edges remaining to be placed
 * 
 * @return vector_edgeID_t
 */
vector_edgeID_t Shuffle::still_in(){
    vector_edgeID_t res;
    for(int i = 0; i < VB.size(); i++){
        if(!VB[i]){
            res.push_back(i);
        }
    }
    return res;
}

bool Shuffle::place_to_default(vector_edgeID_t& R){
    for(edgeID_t i : still_in()){
        if(R[i] == -1){
            R[i] = i;
        }else{
            return false;
        }
    }
    return true;
}

vector_edgeID_t Shuffle::generate_order(vector_edgeID_t R){
    vector_edgeID_t res;
    for(edgeID_t i : R){
        res.push_back(triscore.best_order[i]);
    }
    return res;
}

void Shuffle::display_order(){
    for(int i = 0; i < best_order.size()-1; i++){
        cout << best_order[i] << " - ";
    }
    cout << best_order.back() << '\n';
}

void Shuffle::init(Network& network){
    set_limit_dim(network.n_edge);
    triscore.init(network);
    m_adjacence_matrix.clear();
    m_edge_list.clear();
    R.clear();
    VB.clear();
    //best_cost = -1;
    best_order.clear();

    ifstream ifile("instances/" + network.m_filename);
    string line;
    int i, j, w;
    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
            case 'p':
                n_vertex = atoi(&line[2]);
                m_adjacence_matrix.resize(n_vertex*n_vertex, 1);
                dmax = min(max(refdmax, 0), 3*n_vertex/2 - 2);
                R.resize(3*n_vertex/2 - 2, -1);
                VB.resize(3*n_vertex/2 - 2, false);
            break;
            case 'e':
                flux >> i >> j >> w;
                m_edge_list.push_back(make_pair(i, j));
                m_adjacence_matrix[n_vertex*i + j] = w;
                m_adjacence_matrix[n_vertex*j + i] = w;
            break;
            default:
            break;
        }
    }

    best_cost = triscore.solve();
    best_order = triscore.best_order;

    for(int i = 0; i < n_vertex; i ++){
        m_adjacence_matrix[n_vertex*i + i] = 0;
    }

    m_edge_list = network.edge_list;
}

cost_t Shuffle::call_solve(){
    solve(dmax, 0);
    /*for(int i : generate_order({1, 0, 6, 3, 4, 5, 2})){
        cout << i << '\n';
    }*/
    return best_cost;
}