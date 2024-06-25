#include "Network.hpp"

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
    m_filename = file;
    return; // TODO: debug the issue with tt_dim>2 files

    std::ifstream ifile;

    if(std::filesystem::path(file).is_absolute()){
        ifile = std::ifstream(file);
    }else{
        ifile = std::ifstream("instances/" + file);
    }

    // std::ifstream ifile("instances/" + file);
    if(!ifile){
        std::cerr << "Could not open file : " << file << " at Network initialization" << std::endl;
        exit(-1);
    }else{
        printf("Instantiating network : '%s'\n", file.data());
    }

    std::string line;
    int vertex1, vertex2, weight;

    int tt_dim = 2; // (backward compatibility)

    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
            case 'v': 
                m_display += std::string(line.data()+2) + "\n";
            break;
            case 'd':
                //characteristics of the network
                dimension = atoi(&line[2]);
                n_vertex = dimension * tt_dim;
                n_edge = 3*dimension - 2;

                //defining the size of the adjacence matrix
                adjacence_matrix.resize(n_vertex*n_vertex, 1);
            break;
            case 't':
                tt_dim = atoi(&line[2]);
                n_vertex = dimension * tt_dim;
                n_edge = (tt_dim - 1) * dimension + tt_dim * (dimension - 1);
                
                //defining the size of the adjacence matrix
                adjacence_matrix.resize(n_vertex*n_vertex, 1);
            break;
            case 'e':
                if(tt_dim == 2) {
                    //grabing the edge
                    std::cout<<"! ";
                    flux >> vertex1 >> vertex2 >> weight;
                    {
                    int v1 = min(vertex1, vertex2);
                    int v2 = max(vertex1, vertex2);
                    //adding the edge to the list
                    //edge_list.push_back(edge_t(vertex1, vertex2, weight));
                    edge_list.push_back(std::make_pair(v1, v2));
                    }

                    //updating the adjacence matrix
                    adjacence_matrix[n_vertex*vertex1 + vertex2] = weight;
                    adjacence_matrix[n_vertex*vertex2 + vertex1] = weight;
                    //accumulating the weights
                    density+=weight;

                    std::cout<<"!!"<<std::endl;
                }
            break;
            default:
            break;
        }
    }
    ifile.close();

    //getting the average weight
    density/=n_edge;

    //setting adjacence_amtrix[i][i] to 0
    for(int i = 0; i < n_vertex; i ++){
        adjacence_matrix[n_vertex*i + i] = 0;
    }

    sort_edges();

    printf("Dimension : %d\n", dimension);
    printf("Density : %.1f\n\n", density);
}

void Network::reverse(){
    auto edge_list_copy(edge_list);
    auto adjacence_matrix_copy(adjacence_matrix);
    edge_list.clear();
    adjacence_matrix.clear();

    for(auto edge : edge_list_copy){
        edge_list.push_back(std::make_pair(n_vertex - edge.first - 1, n_vertex - edge.first - 1));
    }

    for(int i = 0; i < n_vertex; i++){
        for(int j = 0; j < n_vertex; j++){
            adjacence_matrix.push_back(adjacence_matrix_copy[n_vertex*(n_vertex - i - 1) + n_vertex - j - 1]);
        }
    }

    sort_edges();
}