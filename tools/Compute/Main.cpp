#include "Compute.hpp"

int main(int argc, char* argv[]){
    std::string network_file;
    std::cin>>network_file;
    Network network(network_file);

    int edge_list_size;
    vector_vertex_pair_t order;
    std::cin>>edge_list_size;
    for(int i = 0; i < edge_list_size; i++) {
        int vertex1, vertex2;
        std::cin>>vertex1>>vertex2;
        order.push_back({vertex1, vertex2});
    }

    std::cout<<"Cost : "<<compute_order(network, order)<<std::endl;
    return 0;
}