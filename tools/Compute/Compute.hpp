#ifndef TEST_HPP
#define TEST_HPP
// #include "../../src/Components/Components.hpp"
#include "../../src/Components/Network2D.hpp"
#include <fstream>

template <size_t tt_dim = 2>
class Cost_cpt {
    private:
    Network2D<tt_dim> m_network;
    vector_vertex_pair_t m_order;
    vector_vertexID_t m_representative;
    std::vector<std::vector<weight_t>> m_adjacence_matrix;

    public:
    Cost_cpt(std::string filename, vector_vertex_pair_t order) : m_network(filename), m_order(order) { 
        // Initialize the representatives vector
        // (where -1 means node is represented by itself)
        this->m_representative.resize(this->m_network.n_vertex, -1);

        // Initialize the adjacence matrix based on the network
        this->m_adjacence_matrix.resize(this->m_network.n_vertex);
        for(int i = 0; i < this->m_network.n_vertex; i++) {
            this->m_adjacence_matrix[i].resize(this->m_network.n_vertex);
            for(int j = 0; j < this->m_network.n_vertex; j++) {
                this->m_adjacence_matrix[i][j] = this->m_network[i, j, true];
            }
        }
    }

    const vertexID_t rep(vertexID_t vertex) {
        while(this->m_representative[vertex] != -1) {
            vertex = this->m_representative[vertex];
        }
        return vertex;
    }

    cost_t contract(vertex_pair_t edge) {
        vertexID_t v1 = this->rep(edge.first);
        vertexID_t v2 = this->rep(edge.second);

        // We always keep smaller node as representative of the contracted edge
        if(v1 > v2) std::swap(v1, v2);

        if(v1 != v1) {
            cost_t result = this->m_network[v1, v2];
            for(vertexID_t j = 0; j < this->m_network.n_vertex; j++) {
                if(v1 != j) {
                    result *= max((weight_t) 1, m_adjacence_matrix[v2][j]);
                }
                if(v2 != j) { 
                    result *= max((weight_t) 1, m_adjacence_matrix[v1][j]);
                }
            }

            for(vertexID_t j = 0; j < this->m_network.n_vertex; j++) {
                m_adjacence_matrix[v1][j] *= m_adjacence_matrix[v2][j];
                m_adjacence_matrix[v2][j] = 0;
                m_adjacence_matrix[j][v2] = 0;
                m_adjacence_matrix[j][v1] = m_adjacence_matrix[v1][j];
            }

            return result;
        }

        return 0;
    }

    cost_t compute_order_cost() {
        // Check if the order is valid
        if(this->m_order.size() > this->m_network.n_edge) {
            std::cout << "[Warning] Too many edges given, skipping order" << std::endl;
            return 0;
        }

        // Simulate the contractions one by one and compute the cost of each
        // operation to obtain the total cost of given order
        cost_t cost = 0;
        for(vertex_pair_t edge : this->m_order){
            cost += this->contract(edge);
        }
        return cost;
    }
};

#endif