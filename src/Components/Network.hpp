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
    dim_t dimension;

    //size in edges
    edgeID_t n_edge;

    //size in nodes
    vertexID_t n_vertex;

    //Adjacence matrix
    Tab adjacence_matrix;

    //Edge list
    //std::vector<edge_t> edge_list;
    std::vector<std::pair<vertexID_t, vertexID_t>> edge_list;

    //density
    float density=0.;

    //file name the network was imported from
    std::string m_filename;

    //string storing the display
    std::string m_display="";

    Network(std::string file);
    Network(){};
    //Network(char* file){Network(std::string(file));};
    Network(char* file) : Network(std::string(file)) {}
    const void display(){std::cout << m_display << std::flush;}
    void sort_edges();
};

#endif