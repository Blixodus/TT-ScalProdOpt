#include "Compute.hpp"

//Code de contraction d'un ordre sur une instance

/**
 * @brief parse the file containing the order to test
 * 
 * @param calc_file
 */
void import_calc_file(string& calc_file){
    std::string path_test = "calc/" + calc_file;

    ifstream ifile(path_test);
    if(ifile.fail()){
        cout << "Could not open test file : " << calc_file << '\n';
        exit(1);
    }
    std::string path_instance;
    Network network;
    std::vector<edgeID_t> order_list;
    edgeID_t edge;

    std::string line;
    while(getline(ifile, line)){
        switch(line[0]){
            case 'f':
                path_instance = "instances/" + std::string(line.data() + 2);
                network = Network(path_instance); 
                // path_instance.append(line.data() + 2);
                //import_instance(path_instance);
                break;
            case 'o':
                order_list.clear();
                if(path_instance.empty()){
                    std::cout << "No file specified, skipped order" << std::endl;
                    break;
                }
                std::string order_string(line.data() + 2);
                istringstream order(order_string);
                while(order >> edge){
                    order_list.push_back(edge);
                }
                std::cout << order_string << " : " << compute_order(network, order_list) << std::endl;

                break;
        }        
    }
}

/**
 * @brief Computes the cost of a contraction order
 * 
 * @param filename the name of a file containing a network
 * @param order_list a list of edge
 * @return cost_t 
 */
cost_t compute_order(std::string& filename, vector_edgeID_t& order_list){
    Network network(filename);
    return compute_order(network, order_list);
}

/**
 * @brief computes the cost of contraction order
 * 
 * @param network a network
 * @param order_list a list of edge
 * @return cost_t 
 */
cost_t compute_order(Network& network, vector_edgeID_t& order_list){

    if(order_list.size() > network.n_edge){
        std::cout << "[WARNING] Too many edges given, skipping order" << std::endl;
        return 0;
    }

    Cost_cpt c_cpt(network);

    cost_t cost = 0;
    for(edgeID_t i : order_list){
        cost += c_cpt.contract(i);
    }
    return cost;
}

cost_t compute_order(Network& network, vector_vertex_pair_t& order_list){

    if(order_list.size() > network.n_edge){
        std::cout << "[WARNING] Too many edges given, skipping order" << std::endl;
        return 0;
    }

    Cost_cpt c_cpt(network);

    cost_t cost = 0;
    for(vertex_pair_t e : order_list){
        cost += c_cpt.contract(e);
    }
    return cost;
}

/**
 * @brief Contracts an edge, modifies the adjacence matrix and correspondance list,
 * and returns the cost
 * 
 * @param edge the edge to contract
 * @return cost_t 
 */
cost_t Cost_cpt::contract(edgeID_t edge){
    vertexID_t e1 = rep(m_edge_list[edge].first);
    vertexID_t e2 = rep(m_edge_list[edge].second);

    if(e1 != e2){
        cost_t res = m_adjacence_matrix[n_vertex*e1 + e2];
        for(vertexID_t j = 0; j < n_vertex; j++){
            if(e1 != j){
                res *= max((weight_t) 1, m_adjacence_matrix[n_vertex*e2 + j]);
            }
            if(e2 != j){ 
                res *= max((weight_t) 1, m_adjacence_matrix[n_vertex*e1 + j]);
            }
        }

        for(vertexID_t j = 0; j < n_vertex; j++){
            m_adjacence_matrix[n_vertex*e1 + j] *= m_adjacence_matrix[n_vertex*e2 + j];
            m_adjacence_matrix[n_vertex*e2 + j] = 0;
            m_adjacence_matrix[n_vertex*j + e2] = 0;
            m_adjacence_matrix[n_vertex*j + e1] = m_adjacence_matrix[n_vertex*e1 + j];
        }
        m_corr_list[e2] = e1;
        return res;
    }else{
        return 0;
    }
}

cost_t Cost_cpt::contract(vertex_pair_t edge){
    vertexID_t e1 = rep(edge.first);
    vertexID_t e2 = rep(edge.second);

    // We always keep smaller node as representative of the contracted edge
    if(e1 > e2) std::swap(e1, e2);

    if(e1 != e2){
        cost_t res = m_adjacence_matrix[n_vertex*e1 + e2];
        for(vertexID_t j = 0; j < n_vertex; j++){
            if(e1 != j){
                res *= max((weight_t) 1, m_adjacence_matrix[n_vertex*e2 + j]);
            }
            if(e2 != j){ 
                res *= max((weight_t) 1, m_adjacence_matrix[n_vertex*e1 + j]);
            }
        }

        for(vertexID_t j = 0; j < n_vertex; j++){
            m_adjacence_matrix[n_vertex*e1 + j] *= m_adjacence_matrix[n_vertex*e2 + j];
            m_adjacence_matrix[n_vertex*e2 + j] = 0;
            m_adjacence_matrix[n_vertex*j + e2] = 0;
            m_adjacence_matrix[n_vertex*j + e1] = m_adjacence_matrix[n_vertex*e1 + j];
        }

        //std::cout<< "Contracting " << edge.first << " (" << e1 << ")" << " and " << edge.second << "(" << e2 << ")" << " with c0st: " << res << std::endl;

        return res;
    }else{
        return 0;
    }
}

/**
 * @brief Returns the representant of a vertex
 * 
 * @param vertex 
 */
vertexID_t Cost_cpt::rep(vertexID_t vertex){
    while(m_corr_list[vertex] != -1){
        vertex = m_corr_list[vertex];
    }
    return vertex;
}