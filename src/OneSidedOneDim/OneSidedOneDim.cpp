#include "OneSidedOneDim.hpp"

cost_t OneSidedOneDim::solve(){
    std::pair<int, int> p;
    // Maximum number of accumulated central edges results
    int kmax = 2 * dmax + 1;

    // Iterating over the dimensions of the tensor train
    for(int s = 0; s < n_vertex/2-1; s++){
        // Prepare the current central edge
        weight_t N = m_adjacence_matrix[n_vertex*s + s+n_vertex/2]; // Central edge to multiply
        int ofs = 2*s; // Offset in the m_central_cost array
        int ofsc = (s+1)*(s+1)-1; // Offset in the m_cost_to_reach array

        // Iterate over the possible weights of the central edge accumulated
        // up to current dimension s
        for(int k = min(2*(s+1), kmax); k >= 2; k--){
            // For possible central edge weights, we calculate the cost of keeping the edge
            // Since the new edges are addes as 0 and 1, we move previous results
            // from position (k-2) to (k).
            m_central_weight[k] = m_central_weight[k-2]*N;

            // Cost of contracting the other 2 edges (in the best possible order)
            m_cost_to_reach[k] = m_cost_to_reach[k-2] + contract(s, k, 2, p);

            // Store the order with the lowest cost
            m_order_by_dim[ofsc + k] = p;
            
            // Cost of keeping the top edge, knowing that the central edge 
            // has weight m_central_weight[k] and cost m_cost_to_reach
            cost_t Rs = m_cost_to_reach[k-2] + contract(s, k, 0, p);

            // If this central edge value offers the best contractions for 
            // the lateral edges, we keep it in memory
            if((Rs < m_ref_cost[ofs] || m_ref_cost[ofs] == -1) && Rs > 0){
                //Rs = minimum of (arrival cost + exit cost) for R at state s
                //m_ref_cost[ofs] stores the best cost Rs obtained for the moment
                m_ref_cost[ofs] = Rs;

                // Store the central edge giving the best cost for R and Q at state s
                m_central_ref[s] = k;
                m_order_by_dim[ofsc] = p; // best order of contraction

                // The central edge giving the best cost for R is also 
                // the one giving the best cost for Q
                cost_t Qs = m_cost_to_reach[k-2] + contract(s, k, 1, p);
                m_ref_cost[ofs+1] = Qs;
                m_order_by_dim[ofsc + 1] = p; 
            }
        }

        // Add the 2 new lateral edges
        m_central_weight[0] = m_adjacence_matrix[n_vertex*s + s+1];
        m_central_weight[1] = m_adjacence_matrix[n_vertex*(s+n_vertex/2) + s+1+n_vertex/2];

        // Rank the best cost for R and Q
        m_cost_to_reach[0] = m_ref_cost[ofs];
        m_cost_to_reach[1] = m_ref_cost[ofs+1];
    }

    // Last step, we directly retrieve the result
    int s = n_vertex/2-1;
    weight_t N = m_adjacence_matrix[n_vertex*s + s+n_vertex/2];
    cost_t cost = std::numeric_limits<cost_t>::max();

    for(int k = 0; k <= min(2*s, kmax); k++){
        cost_t ck = m_cost_to_reach[k] + m_central_weight[k]*N;
        if(ck < cost && ck > 0){
            cost = ck;        
            m_central_ref[s] = k;
        }
    }

    return cost; 
}

/**
 * @brief computes the contraction cost
 * 
 * @param s the state we are currently at
 * @param k the index in m_central_weight of the tensor t we consider as the central edge
 * @param x the contraction in {0, 1, 2}
 * @param p a buffer that stores the exact order of contraction in {0, 1, 2}^2
 * @return cost_t 
 */
cost_t OneSidedOneDim::contract(int s, int k, int x, pair<int, int>& p){
    // Calculate the outgoing weights of the vertices
    compute_ect(s, k);

    // Recover the weights of the side edges (not necessary but more readable)
    cost_t costR = m_adjacence_matrix[n_vertex*s + s+1];
    cost_t costQ = m_adjacence_matrix[n_vertex*(s+n_vertex/2) + s+1+n_vertex/2];
   

    // The costs associated with the different contraction orders
    cost_t r12, r21, r02, r20;
    cost_t cost;
    switch(x){
        //o-R- (costR)
        //C
        //o-Q- (costQ)
        
        // Keep the top edge (R)
        case 0:
            // First Q followed by N
            r12 = m_ext_cost_tab[s+1+n_vertex/2]*m_central_weight[k] + m_ext_cost_tab[s]*m_ext_cost_tab[s+1+n_vertex/2]/costQ;

            // First N followed by Q
            r21 = costR*costQ*m_central_weight[k] + costR*m_ext_cost_tab[s+1+n_vertex/2];
            if(r12 < r21){
                cost = r12;
                p = make_pair(1, 2);
            }else{
                cost = r21;
                p = make_pair(2, 1);
            }
            break;

        // Keep the bottom edge (Q)
        case 1:
            r02 = m_ext_cost_tab[s+1]*m_central_weight[k] + m_ext_cost_tab[s+n_vertex/2]*m_ext_cost_tab[s+1]/costR;
            r20 = costQ*(costR*m_central_weight[k] + m_ext_cost_tab[s+1]);
            if(r02 < r20){
                cost = r02;
                p = make_pair(0, 2);
            }else{
                cost = r20;
                p = make_pair(2, 0);
            }
            break;

        // Keep the central edge (C)
        case 2:
            cost = m_central_weight[k]*(m_ext_cost_tab[s+1] + m_ext_cost_tab[s+1+n_vertex/2]);
            p = make_pair(0, 1);
            break;
        default:
            cost = 0;
            break;
    }
    
    // Restore the outgoing weights of the vertices
    restore_ect(s);

    return cost;
}

/**
 * @brief computes m_ext_cost_tab, the outer weights of each tensor in a given state
 * 
 * @param k the index in m_central_weight of the tensor t we just calculated
 * @param s the state we are currently at
 */
void OneSidedOneDim::compute_ect(int s, int k){
    m_ext_cost_tab[s] = m_adjacence_matrix[n_vertex*s + s+1]*m_central_weight[k];
    m_ext_cost_tab[s+n_vertex/2] = m_adjacence_matrix[n_vertex*(s+n_vertex/2) + s+1+n_vertex/2]*m_central_weight[k];
}

/**
 * @brief restores m_ext_cost_tab to a given state
 * 
 * @param s the state
 */
void OneSidedOneDim::restore_ect(int s){
    m_ext_cost_tab[s] = m_adjacence_matrix[n_vertex*n_vertex + s];
    m_ext_cost_tab[s+n_vertex/2] = m_adjacence_matrix[n_vertex*n_vertex + s+n_vertex/2];
}

/**
 * @brief displays the best contraction order using backtracking
 * 
 * @param s a state
 * @param k the center-edge that lead to the best final cost
 */
void OneSidedOneDim::display_order(int s, int k){
    int ofs = (s+1)*(s+1)-1;
    if(s >= 0){
        if(k > 1){
            display_order(s-1, k-2);
        }else{
            display_order(s-1, m_central_ref[s]-2);
        }
        cout << "|" << m_order_by_dim.at(ofs + k).first << " - " << m_order_by_dim.at(ofs + k).second << "|";
    }
}

void OneSidedOneDim::display_order(){
    for(int i = 0; i < best_order.size()-1; i++){
        cout << best_order[i] << " - ";
    }
    cout << best_order.back() << '\n';
}

void OneSidedOneDim::get_order(int s, int k){
    int ofs = (s+1)*(s+1)-1;
    if(s >= 0){
        if(k > 1){
            get_order(s-1, k-2);
        }else{
            get_order(s-1, m_central_ref[s]-2);
        }
        int e1 = m_order_by_dim[ofs + k].first;
        int e2 = m_order_by_dim[ofs + k].second;

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
        //cout << "|" << m_order_by_dim.at(ofs + k).first << " - " << m_order_by_dim.at(ofs + k).second << "|";
    }
}

void OneSidedOneDim::init(Network& network){
    set_limit_dim(network.dimension);
    dim = network.dimension;
    n_vertex = network.n_vertex;

    m_adjacence_matrix.clear();
    m_ext_cost_tab.clear();
    m_ref_cost.clear();
    m_cost_to_reach.clear();
    m_central_weight.clear();
    m_order_by_dim.clear();
    m_central_ref.clear();
    best_order.clear();

    m_cost_to_reach.resize(min(2*(n_vertex/2)-1, 2*dmax+1), 0); // cost table
    m_central_weight.resize(min(2*(n_vertex/2)-1, 2*dmax+1), 1); // possible weight of the central edge
    m_adjacence_matrix.resize(n_vertex*(n_vertex+1), 1);
    m_ext_cost_tab.resize(n_vertex, 1);
    m_ref_cost.resize(n_vertex, -1);
    m_order_by_dim.resize(n_vertex*n_vertex/4, {-1, -1});
    m_central_ref.resize(n_vertex/2, -1);

    for(const auto& [v1, v2] : network.edge_list){
        weight_t w = network.adjacence_matrix[n_vertex*v1 + v2];

        m_ext_cost_tab[v1] *= w;
        m_ext_cost_tab[v2] *= w;

        m_adjacence_matrix[n_vertex*v1 + v2] = w;
        m_adjacence_matrix[n_vertex*v2 + v1] = w;

        m_adjacence_matrix[n_vertex*n_vertex + v1] *= w;
        m_adjacence_matrix[n_vertex*n_vertex + v2] *= w;
    }
}

cost_t OneSidedOneDim::call_solve(){
    cost_t c = solve();
    get_order(n_vertex/2-2, m_central_ref[n_vertex/2-1]);
    best_order.push_back(n_vertex-2);
    return c;
}