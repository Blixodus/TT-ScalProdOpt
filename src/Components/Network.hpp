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
struct Network {
    // Network information
    dim_t dim;
    edgeID_t n_edge; // number of edges
    vertexID_t n_vertex; // number of nodes

    // Filename the network was imported from
    std::string m_filename;

    // Container for storing the edge weights
    // e.g. rrr|dddd#rrr|dddd...
    vector<cost_t> m_egde_weight;

    // Constructors
    Network(){}
    Network(std::string file) {
        // Initialize the filename
        this->m_filename = file;

        // Open the network file
        std::ifstream ifile(this->m_filename);

        if(!ifile) {
            std::cerr << "Could not open file : " << file << " at Network initialization" << std::endl;
            exit(-2);
        } else {
            std::cout<<"Instantiating network : " << file << std::endl;
        }

        // Parse tensor train from file
        std::string line;
        int vertex1, vertex2, weight, file_tt_dim;
        while(getline(ifile, line)) {
            istringstream flux(&line[2]);
            //std::cout << line << std::endl;
            switch(line[0]) {
                case 'd':
                    // Parse characteristics of the network
                    this->dim = atoi(&line[2]);
                    this->n_vertex = this->dim * tt_dim;
                    this->n_edge = (tt_dim - 1) * this->dim + tt_dim * (this->dim - 1); // dimensions + ranks

                    // Initialize the edge weight vector
                    this->m_egde_weight.resize(this->n_edge, 1);
                break;
                case 't':
                    // Parse tt_dim from file (to double check provided runtime parameter)
                    file_tt_dim = atoi(&line[2]);
                    if(file_tt_dim != tt_dim) {
                        std::cerr << "[Error] Mismatch in tt_dim between file (" << file_tt_dim << ") and runtime parameter (" << tt_dim << ")." << std::endl;
                        exit(-2);
                    }
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
        std::cout << "Dimension " << this->dim << std::endl;

        // Close the file
        ifile.close();
    }

    // Utility functions
    constexpr cost_t& operator [](const int nodeA, const int nodeB) {
        // Ensure that we calculate the indices based on the smaller node id
        assert(nodeA < nodeB);

        // Calculate position of node in 2D grid
        const int row = nodeA / this->dim;    // row of node in 2D grid
        const int column = nodeA % this->dim; // column of node in 2D grid

        // Calculate offset in the edge list (e.g. rrr|dddd#rrr|dddd...)
        // 0 if the nodes are adjacent (requested edge is rank)
        // dimension - 1 if the nodes are not adjacent (requested edge is dimension)
        const int offset = (nodeB - nodeA) == 1 ? 0 : this->dim - 1;

        // Calculate the final index of edge in the edge list
        const int index = row * (2 * this->dim - 1) + offset + column;

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

        // Check if subsequent nodes are in the same row
        if (nodeB - nodeA == 1 && nodeA / this->dim != nodeB / this->dim) {
            return 0;
        }

        // Check if the nodes are adjacent and return the weight of edge
        // between them, or 0 otherwise
        if(nodeB - nodeA == 1 || nodeB - nodeA == this->dim) {
            return (*this)[nodeA, nodeB];
        }
            
        return 0;
    }
};

#endif