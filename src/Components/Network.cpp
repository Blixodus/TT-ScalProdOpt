#include "Network.hpp"
/**
 * @brief Sorts edge_list given the order right->left, up->down
 * 
 */
/*void Network::sort_edges(){
    // int size = (2*edge_list.size() + 4)/3; //dimension
    
    sort(edge_list.begin(), edge_list.end(), [dimension](edge_t e1, edge_t e2){
        if(e1.m_edge.first < e2.m_edge.first && e1.m_edge.second < e2.m_edge.second){
            return true;
        }else{
            if(e1.m_edge.first < dimension && e2.m_edge.first < dimension){
                return e1.m_edge.second < e2.m_edge.second;
            }else{
                return e1.m_edge.first < dimension && e2.m_edge.first >=dimension;
            }
        }
    });
}*/

void Network::sort_edges(){
    sort(edge_list.begin(), edge_list.end(), [this](std::pair<vertexID_t, vertexID_t> e1, std::pair<vertexID_t, vertexID_t> e2){
        if(e1.first < e2.first && e1.second < e2.second){
            return true;
        }else{
            if(e1.first < dimension && e2.first < dimension){
                return e1.second < e2.second;
            }else{
                return e1.first < dimension && e2.first >=dimension;
            }
        }
    });
}

/**
 * @brief Construct a new Network:: Network object
 * 
 * @param file a file containing a network
 */
Network::Network(std::string file){
    m_filename=file;

    ifstream ifile(file);
    if(!ifile){
        std::cerr << "Could not open file : " << file << std::endl;
        exit(-1);
    }
    std::string line;
    int vertex1, vertex2, weight;
    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
            case 'v': 
                m_display += std::string(line.data()+2) + "\n";
            break;
            case 'd':
                //characteristics of the network
                dimension = atoi(&line[2]);
                n_vertex = dimension*2;
                n_edge = 3*dimension - 2;

                //defining the size of the adjacence matrix
                adjacence_matrix.resize(n_vertex*n_vertex, 1);
            break;
            case 'e':
                //grabing the edge
                flux >> vertex1 >> vertex2 >> weight;
                //adding the edge to the list
                //edge_list.push_back(edge_t(vertex1, vertex2, weight));
                edge_list.push_back(std::make_pair(vertex1, vertex2));

                //updating the adjacence matrix
                adjacence_matrix[n_vertex*vertex1 + vertex2] = weight;
                adjacence_matrix[n_vertex*vertex2 + vertex1] = weight;
                //accumulating the weights
                density+=weight;
            break;
            default:
            break;
        }
    }

    //getting the average weight
    density/=n_edge;

    //setting adjacence_amtrix[i][i] to 0
    for(int i = 0; i < n_vertex; i ++){
        adjacence_matrix[n_vertex*i + i] = 0;
    }

    sort_edges();
}