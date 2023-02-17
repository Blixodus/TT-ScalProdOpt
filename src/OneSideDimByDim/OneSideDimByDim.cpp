#include "OneSideDimByDim.hpp"

cost_t OneSideDBD::solve(){
    pair<int, int> p;
    //nombre max d'arêtes centrales accumulées
    int kmax = 2*dmax + 1; //commenting this might break everything, we'll see
    //passage de l'état s, à s+1
    for(int s = 0; s < n_vertex/2-1; s++){
        int N = m_adjacence_matrix[n_vertex*s + s+n_vertex/2]; //arête centrale à multiplier
        int ofs = 2*s; //offset dans le tableau T
        int ofsc = (s+1)*(s+1)-1; //offset dans le tableau C
        for(int k = min(2*(s+1), kmax); k >= 2; k--){
            //pour chaque produits de Nk, on calcule le coût de garder l'arête
            T[k] = T[k-2]*N;

            //coût de contracter les 2 autres arêtes (dans le meilleur ordre possible)
            C[k] = C[k-2] + contract(s, k, 2, p);

            //stockage de l'ordre ayant donné le coût le plus faible
            O[ofsc + k] = p;

            //coût de garder l'arête du haut, sachant que l'arête centrale est de poids T[k] et a coûté C[k-2]
            int Rs = C[k-2] + contract(s, k, 0, p);

            //si cette valeur d'arête centrale offre les meilleurs contractions pour les arêtes latérale, on le garde en mémoire
            if((Rs < P[ofs] || P[ofs] == -1) && Rs > 0){
                //Rs = minimum de (coût d'arrivée + coût de sortie) pour R à l'état s
                //P[ofs] stock le meilleur coût Rs obtenu pour l'instant
                P[ofs] = Rs;
                //on stock l'arête centrale donnant le meilleur coût pour R et Q à l'état s 
                Z[s] = k;
                //on stock l'ordre (la paire d'arête) dans lequel on a contracté
                O[ofsc] = p;

                //l'arête centrale donnant le meilleur coût pour R est aussi celle donnant le meilleur coût pour Q
                int Qs = C[k-2] + contract(s, k, 1, p);
                P[ofs+1] = Qs;
                O[ofsc + 1] = p; 
            }
        }
        //on ajoute les 2 nouvelles arêtes latérale
        T[0] = m_adjacence_matrix[n_vertex*s + s+1];
        T[1] = m_adjacence_matrix[n_vertex*(s+n_vertex/2) + s+1+n_vertex/2];

        //on range le meilleur coût pour R et Q
        C[0] = P[ofs];
        C[1] = P[ofs+1];
    }

    //gestion de la dernière étape, on récupère directement le résultat
    int s = n_vertex/2-1;
    int N = m_adjacence_matrix[n_vertex*s + s+n_vertex/2];
    int cost = INT32_MAX;

    for(int k = 0; k <= min(2*s, kmax); k++){
        int ck = C[k] + T[k]*N;
        if(ck < cost && ck > 0){
            cost = ck;
            //donne le k de l'avant-dernier état qui mène à l'optimum, afin de le récupérer facilement
            Z[s] = k;
        }
    }
    return cost; 
}

/**
 * @brief computes the contraction cost
 * 
 * @param s the state we are currently at
 * @param k the index in T of the tensor t we consider as the central edge
 * @param x the contraction
 * @param p a buffer that stores the exact order of contraction
 * @return cost_t 
 */
cost_t OneSideDBD::contract(int s, int k, int x, pair<int, int>& p){
    //on calcule les poids sortants des sommets s et s+D
    compute_ect(s, k);
    //on récupère les poids des arêtes latérales (pas nécessaire mais plus lisible)
    cost_t costR = m_adjacence_matrix[n_vertex*s + s+1];
    cost_t costQ = m_adjacence_matrix[n_vertex*(s+n_vertex/2) + s+1+n_vertex/2];
   

    //les coûts associés aux différents ordres de contraction
    cost_t r12;
    cost_t r21;

    cost_t r02;
    cost_t r20;
    cost_t cost;
    switch(x){
        //o-R- (costR)
        //N
        //o-Q- (costQ)
        
        //on garde l'arête du dessus (R)
        case 0:
            //Q puis N
            r12 = m_ext_cost_tab[s+1+n_vertex/2]*T[k] + m_ext_cost_tab[s]*m_ext_cost_tab[s+1+n_vertex/2]/costQ;

            //N puis Q
            r21 = costR*costQ*T[k] + costR*m_ext_cost_tab[s+1+n_vertex/2];
            if(r12 < r21){
                cost = r12;
                p = make_pair(1, 2);
            }else{
                cost = r21;
                p = make_pair(2, 1);
            }
            break;
        //cas arête du dessous (Q)
        case 1:
            r02 = m_ext_cost_tab[s+1]*T[k] + m_ext_cost_tab[s+n_vertex/2]*m_ext_cost_tab[s+1]/costR;
            r20 = costQ*(costR*T[k] + m_ext_cost_tab[s+1]);
            if(r02 < r20){
                cost = r02;
                p = make_pair(0, 2);
            }else{
                cost = r20;
                p = make_pair(2, 0);
            }
            break;
        //cas arête centrale
        case 2:
            cost = T[k]*(m_ext_cost_tab[s+1] + m_ext_cost_tab[s+1+n_vertex/2]);
            p = make_pair(0, 1);
            break;
        default:
            cost = 0;
            break;
    }
    //on remet le tableau des poids sortants à son état initial (au cas où)
    restore_ect(s);
    return cost;
}

/**
 * @brief computes m_ext_cost_tab, the outer weights of each tensor in a given state
 * 
 * @param k the index int T of the tensor t we just calc'ed
 * @param s the state we are currently at
 */
void OneSideDBD::compute_ect(int s, int k){
    m_ext_cost_tab[s] = m_adjacence_matrix[n_vertex*s + s+1]*T[k];
    m_ext_cost_tab[s+n_vertex/2] = m_adjacence_matrix[n_vertex*(s+n_vertex/2) + s+1+n_vertex/2]*T[k];
}

/**
 * @brief restores m_ext_cost_tab to a given state
 * 
 * @param s the state
 */
void OneSideDBD::restore_ect(int s){
    m_ext_cost_tab[s] = m_adjacence_matrix[n_vertex*n_vertex + s]; //stock le poids sortant du sommet s
    m_ext_cost_tab[s+n_vertex/2] = m_adjacence_matrix[n_vertex*n_vertex + s+n_vertex/2];
}

/**
 * @brief displays the best contraction order using backtracking
 * 
 * @param s a state
 * @param k the center-edge that lead to the best final cost
 */
void OneSideDBD::display_order(int s, int k){
    int ofs = (s+1)*(s+1)-1;
    if(s >= 0){
        if(k > 1){
            display_order(s-1, k-2);
        }else{
            display_order(s-1, Z[s]-2);
        }
        cout << "|" << O.at(ofs + k).first << " - " << O.at(ofs + k).second << "|";
    }
}

void OneSideDBD::display_order(){
    for(int i = 0; i < best_order.size()-1; i++){
        cout << best_order[i] << " - ";
    }
    cout << best_order.back() << '\n';
}

void OneSideDBD::get_order(int s, int k){
    int ofs = (s+1)*(s+1)-1;
    if(s >= 0){
        if(k > 1){
            get_order(s-1, k-2);
        }else{
            get_order(s-1, Z[s]-2);
        }
        int e1 = O[ofs + k].first;
        int e2 = O[ofs + k].second;

        switch(e1){
            case 0:
                best_order.push_back(s);
            break;
            case 2:
                best_order.push_back(n_vertex/2-1 + s);
            break;
            case 1:
                best_order.push_back(n_vertex-1+s);
            break;
        }

        switch(e2){
            case 0:
                best_order.push_back(s);
            break;
            case 2:
                best_order.push_back(n_vertex/2-1 + s);
            break;
            case 1:
                best_order.push_back(n_vertex-1 + s);
            break;
        }
        //cout << "|" << O.at(ofs + k).first << " - " << O.at(ofs + k).second << "|";
    }
}

void OneSideDBD::init(Network& network){
    set_limit_dim(network.dimension);
    dim = network.dimension;
    n_vertex = network.n_vertex;

    m_adjacence_matrix.clear();
    m_ext_cost_tab.clear();
    P.clear();
    C.clear();
    T.clear();
    O.clear();
    Z.clear();
    best_order.clear();
    // int kmax = 2*dmax+1;

    C.resize(min(2*(n_vertex/2)-1, 2*dmax+1), 0); //tableau des coûts
    T.resize(min(2*(n_vertex/2)-1, 2*dmax+1), 1); //tableau des arêtes centrales
    m_adjacence_matrix.resize(n_vertex*(n_vertex+1), 1);
    m_ext_cost_tab.resize(n_vertex, 1);
    P.resize(n_vertex, -1);
    O.resize(n_vertex*n_vertex/4, {-1, -1});
    Z.resize(n_vertex/2, -1);

    for(const auto& [v1, v2] : network.edge_list){
        weight_t w = network[n_vertex*v1 + v2];

        m_ext_cost_tab[v1] *= w;
        m_ext_cost_tab[v2] *= w;

        m_adjacence_matrix[n_vertex*v1 + v2] = w;
        m_adjacence_matrix[n_vertex*v2 + v1] = w;

        m_adjacence_matrix[n_vertex*n_vertex + v1] *= w;
        m_adjacence_matrix[n_vertex*n_vertex + v2] *= w;
    }
}

cost_t OneSideDBD::call_solve(){
    cost_t c = solve();
    get_order(n_vertex/2-2, Z[n_vertex/2-1]);
    best_order.push_back(n_vertex-2);
    return c;
}