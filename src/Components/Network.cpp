#include "Network.hpp"

/**
 * @brief Construct a new Network:: Network object
 * 
 * @param file a file containing a network
 */
Network::Network(std::string file){
    filename=file;

    ifstream ifile(file);
    string line;
    int vertex1, vertex2, weight;
    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
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
                edge_list.push_back(make_pair(vertex1, vertex2));
                //adding the weight to the list
                weights.push_back(weight);
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
}