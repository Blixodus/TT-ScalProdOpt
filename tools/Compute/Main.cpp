#include "Compute.hpp"

int main(int argc, char* argv[]){
    // Read tensor train dimension
    int tt_dim = 2;
    cin>>tt_dim;

    // Read network file and initialize network
    std::string network_file;
    std::cin>>network_file;

    // Parse contraction list and compute cost
    int edge_list_size;
    vector_vertex_pair_t order;
    std::cin>>edge_list_size;
    for(int i = 0; i < edge_list_size; i++) {
        int vertex1, vertex2;
        std::cin>>vertex1>>vertex2;
        order.push_back({vertex1, vertex2});
    }

    // Calculate the cost of given contraction order
    cost_t cost = 0;
    if(tt_dim == 2) {
        Cost_cpt<2> cost_calc(network_file, order);
        cost = cost_calc.compute_order_cost();
    } else if(tt_dim == 3) {
        Cost_cpt<3> cost_calc(network_file, order);
        cost = cost_calc.compute_order_cost();
    } else if(tt_dim == 4) {
        Cost_cpt<4> cost_calc(network_file, order);
        cost = cost_calc.compute_order_cost();
    } else {
        std::cerr<<"[Error] Unsupported tt_dim value. Please add tt_dim to template in tools/Compute/Main.cpp in order to use it."<<std::endl;
        cost = -1;
    }


    std::cout<<"Cost : "<<cost<<std::endl;
    return 0;
}