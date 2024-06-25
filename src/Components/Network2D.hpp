#ifndef NETWORK2D_HPP
#define NETWORK2D_HPP
#include "Components.hpp"
#include <string>
#include <cassert>

/**
 * @brief Network structure
 * Contains all relevant information about the imported network
 */
template <size_t tt_dim = 2>
struct Network2D {
    //dimension
    dim_t dimension;

    //size in edges
    edgeID_t n_edge;

    //size in nodes
    vertexID_t n_vertex;

    //density
    float density=0.;

    // Filename the network was imported from
    std::string m_filename;

    vector<cost_t> m_egde_weight;

    // Constructors
    Network2D(){}
    Network2D(std::string file) {
        // Initialize the filename
        this->m_filename = file;

        // Open the network file
        //std::cout<<"!"<<std::endl;
        std::ifstream ifile(this->m_filename);
        //std::cout<<"!!"<<std::endl;

        if(!ifile) {
            std::cerr << "Could not open file : " << file << " at Network initialization" << std::endl;
            exit(-2);
        } else {
            std::cout<<"Instantiating network 2D : " << file << std::endl;
        }

        // Parse tensor train from file
        //std::cout<<"!!!"<<std::endl;
        std::string line;
        int vertex1, vertex2, weight;
        while(getline(ifile, line)) {
            istringstream flux(&line[2]);
            //std::cout << line << std::endl;
            switch(line[0]) {
                case 'd':
                    // Parse characteristics of the network
                    this->dimension = atoi(&line[2]);
                    this->n_vertex = this->dimension * tt_dim;
                    this->n_edge = (tt_dim - 1) * this->dimension + tt_dim * (this->dimension - 1); // dimensions + ranks

                    // Initialize the edge weight vector
                    this->m_egde_weight.resize(this->n_edge, 1);
                break;
                case 'e':
                    // Parse the edge
                    flux >> vertex1 >> vertex2 >> weight;
                    {
                        int v1 = min(vertex1, vertex2);
                        int v2 = max(vertex1, vertex2);
                        (*this)[v1, v2] = weight;
                    }
                break;
                default:
                break;
            }
        }

        // Display the network information
        std::cout << "TT dimension " << tt_dim << std::endl;
        std::cout << "Dimension " << this->dimension << std::endl;

        /*for(int i = 0; i < this->m_egde_weight.size(); i++) {
            std::cout << this->m_egde_weight[i] << " ";
        }
        std::cout << std::endl;*/

        // Close the file
        ifile.close();
        //std::cout<<"Finished\n"<<std::endl;
    }

    // Utility functions
    constexpr cost_t& operator [](const int nodeA, const int nodeB) {
        // Ensure that we calculate the indices based on the smaller node id
        assert(nodeA < nodeB);

        // Calculate position of node in 2D grid
        const int row = nodeA / this->dimension;    // row of node in 2D grid
        const int column = nodeA % this->dimension; // column of node in 2D grid

        // Calculate offset in the edge list (e.g. rrr|dddd#rrr|dddd...)
        // 0 if the nodes are adjacent (requested edge is rank)
        // dimension - 1 if the nodes are not adjacent (requested edge is dimension)
        const int offset = (nodeB - nodeA) == 1 ? 0 : this->dimension - 1;

        // Calculate the final index of edge in the edge list
        const int index = row * (2 * this->dimension - 1) + offset + column;

        //std::cout<<"Edge: " << nodeA<<" "<<nodeB<<" "<<index<<std::endl;
        assert(index < this->m_egde_weight.size());
        return this->m_egde_weight[index];
    }

    // Retrieve cost of the edge between two arbitrary nodes (for verification)
    // It handles the cases when two nodes are not adjacent, and provides
    // additional checks which are unnecessary in the computational code.
    cost_t operator[](int nodeA, int nodeB, bool dummy_arg) {
        // Fix order of nodes 
        if(nodeA > nodeB) std::swap(nodeA, nodeB);

        // Check if the nodes are adjacent and return the weight of edge
        // between them, or 0 otherwise
        if(nodeB - nodeA == 1 || nodeB - nodeA == this->dimension) {
            return (*this)[nodeA, nodeB];
        }
            
        return 0;
    }
};

#endif