#ifndef NETWORK_HPP
#define NETWORK_HPP
#include "Components.hpp"
#include <string>

/**
 * @brief Network structure
 * Contains all relevant information about the imported network
 */
struct Network{
    //dimension
    unsigned int dimension;

    //size in edges
    unsigned int n_edge;

    //size in nodes
    unsigned int n_vertex;

    //Adjacence matrix
    Tab adjacence_matrix;

    //Edge list
    std::vector<std::pair<int, int>> edge_list;

    //Weight list
    Tab weights;

    //density
    float density=0.;

    //file name the network was imported from
    std::string filename;

    Network(std::string file);
};

#endif