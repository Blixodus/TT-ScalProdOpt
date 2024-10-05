#ifndef TWOSIDEDSWEEPING_HPP
#define TWOSIDEDSWEEPING_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

// Type of the contraction
enum contraction_type_e {
    QC,
    CQ,
    PC,
    CP,
    PQ,
    QP
};

// Type of the remaining edge
enum edge_type_e {
    P = 0,       
    Q = 1,
    C = 2
};

class TwoSidedSweeping : public Algorithm {
    private:
    // Solution parameters
    split_direction_e m_direction;  // Direction for the start of the contraction

    // Network information
    Network<2> m_network;        // Generalized network information
    //std::vector<cost_t> cost[3]; // Cost of contracting subproblem [j, i] starting with edge P, Q or C

    std::vector<std::vector<cost_t>> DP_P, DP_Q; // Cost of contracting subproblem [1, i] with keeping edge P, Q or C
    std::vector<std::vector<std::string>> order_P, order_Q; // Order of contraction of subproblem [1, i] with keeping edge P, Q or C
     

    // Initializer
    void init_variables(result_direction_e direction) {
        DP_P.resize(this->m_network.dim + 2, std::vector<cost_t>(this->m_network.dim + 2, std::numeric_limits<cost_t>::max()));
        DP_Q.resize(this->m_network.dim + 2, std::vector<cost_t>(this->m_network.dim + 2, std::numeric_limits<cost_t>::max()));

        order_P.resize(this->m_network.dim + 2, std::vector<std::string>(this->m_network.dim + 2, ""));
        order_Q.resize(this->m_network.dim + 2, std::vector<std::string>(this->m_network.dim + 2, ""));
    }

    // Utility functions

    public:
    // Constructors
    TwoSidedSweeping(){}
    TwoSidedSweeping(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){
        // Direction of the contraction
        if(param_dictionary.find("dir") != param_dictionary.end()) {
            std::string dir = std::any_cast<string>(param_dictionary["dir"]);
            if(dir == "START_LEFT") {
                this->m_direction = split_direction_e::START_LEFT;
            } else if(dir == "START_RIGHT") {
                this->m_direction = split_direction_e::START_RIGHT;
            } else if(dir == "BOTH_SIDES") {
                this->m_direction = split_direction_e::BOTH_SIDES;
            } else if(dir == "ALL") {
                std::cerr<<"Warning! TwoSidedSweeping algorithm does not support ALL (splits) direction. Using BOTH_SIDES instead."<<std::endl;
                this->m_direction = split_direction_e::BOTH_SIDES;
            } else {
                std::cerr<<"Warning! Unknown direction '"<<dir<<"'. Using BOTH_SIDES instead."<<std::endl;
                this->m_direction = split_direction_e::BOTH_SIDES;
            }
        } else {
            this->m_direction = split_direction_e::BOTH_SIDES;
        }
    }

    // Initializer
    void init(std::string filename) {
        // Initialize network 2D
        this->m_network = Network<2>(filename);

        // Initialize algorithm variables
        this->init_variables(LR);
    }

    // Solvers
    cost_t contraction_cost(cost_t p, cost_t q, cost_t c, cost_t p_next, cost_t q_next, cost_t c_next, contraction_type_e type)
    {
        switch(type) {
            case QC:
                return q * c * c_next * q_next + c * p * c_next * q_next;
            case CQ:
                return c * p * q + q * p * c_next * q_next;
            case PC:
                return p * c * c_next * p_next + c * q * c_next * p_next;
            case CP:
                return c * p * q + p * q * c_next * p_next;
            case PQ:
                return p * c * c_next * p_next + q * c * c_next * q_next;
            case QP:
                return q * c * c_next * q_next + p * c * c_next * p_next;
            default:
                std::cerr<<"Error! Unknown contraction type: "<<type<<std::endl;
                return 0;
        }
    }

    cost_t get_edge(edge_type_e type, int column) {
        column--; // offset for 0-based indexing of network definition
        if (column < 0 || column >= this->m_network.dim) {
            return 1;
        }

        switch(type) {
            case P:
                if(column >= this->m_network.dim - 1) {
                    return 1;
                }
                return this->m_network[column, column + 1];
            case Q:
                if(column >= this->m_network.dim - 1) {
                    return 1;
                }
                return this->m_network[column + this->m_network.dim, column + this->m_network.dim + 1];
            case C:
                return this->m_network[column, column + this->m_network.dim];
            default:
                std::cerr<<"Error! Unknown edge type: "<<type<<std::endl;
                return 1;
        }
    }

    cost_t cost(int i, cost_t c, contraction_type_e type) {
        if(i == 0 && c == 11 && type == QP) {
            std::cout<<"\n\t"<<get_edge(P, i)<<" "<<get_edge(Q, i)<<" "<<c<<" "<<get_edge(P, i + 1)<<" "<<get_edge(Q, i + 1)<<" "<<get_edge(C, i + 1)<<" "<<type<<" "<<contraction_cost(get_edge(P, i), get_edge(Q, i), c, get_edge(P, i + 1), get_edge(Q, i + 1), get_edge(C, i + 1), type)<<std::endl;
        }
        return contraction_cost(get_edge(P, i), get_edge(Q, i), c, get_edge(P, i + 1), get_edge(Q, i + 1), get_edge(C, i + 1), type);
    }

    cost_t edge_cumulated(int i, int j, edge_type_e type)
    {
        cost_t weight = get_edge(type, j);
        for(int k = j + 1; k < i; k++) {
            if(weight >= std::numeric_limits<cost_t>::max() / (get_edge(C, k) * 2 * 1000 * 1000 * 1000))
                return 0;

            weight *= get_edge(C, k);
        }
        weight *= get_edge(C, i);
        return weight;
    }

    cost_t solve() {
        // Base case for contracting the first edge
        DP_P[0][0] = 0;
        DP_Q[0][0] = 0;
        DP_P[1][0] = 0;
        DP_Q[1][0] = 0;

        // Iterating over subnetworks [i...dim-1]
        for(int i = 2; i <= this->m_network.dim + 1; i++) { 
            std::cout<<"Iteration: "<<i<<std::endl;
            for(int j = 0; j < i; j++) {
                // We use P[i-1][j] to calculate next subproblems
                if(DP_P[i - 1][j] != std::numeric_limits<cost_t>::max()) {
                    // Contraction PC -> Q[i][i-1]
                    cost_t cost_PC = cost(i - 1, edge_cumulated(i - 1, j, P), PC);
                    std::cout<<"PC: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), PC)<<" "<<cost_PC<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_PC > 0 && DP_P[i - 1][j] + cost_PC < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] = DP_P[i - 1][j] + cost_PC;
                        order_Q[i][i - 1] = "PC";
                    }

                    cost_t cost_CP = cost(i - 1, edge_cumulated(i - 1, j, P), CP);
                    std::cout<<"CP: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), CP)<<" "<<cost_CP<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_CP > 0 &&  DP_P[i - 1][j] + cost_CP < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] =  DP_P[i - 1][j] + cost_CP;
                        order_Q[i][i - 1] = "CP";
                    }

                    // Contraction QC -> P[i][i-1]
                    cost_t cost_QC = cost(i - 1, edge_cumulated(i - 1, j, P), QC);
                    std::cout<<"QC: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), QC)<<" "<<cost_QC<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_QC > 0 && DP_P[i - 1][j] + cost_QC < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_P[i - 1][j] + cost_QC;
                        order_P[i][i - 1] = "QC";
                    }

                    cost_t cost_CQ = cost(i - 1, edge_cumulated(i - 1, j, P), CQ);
                    std::cout<<"CQ: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), CQ)<<" "<<cost_CQ<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_CQ > 0 && DP_P[i - 1][j] + cost_CQ < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_P[i - 1][j] + cost_CQ;
                        order_P[i][i - 1] = "CQ";
                    }

                    // Contraction PQ -> P[i][j]
                    cost_t cost_PQ = cost(i - 1, edge_cumulated(i - 1, j, P), PQ);
                    std::cout<<"PQ: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), PQ)<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_PQ > 0 && DP_P[i - 1][j] + cost_PQ < DP_P[i][j]) {
                        DP_P[i][j] = DP_P[i - 1][j] + cost_PQ;
                        order_P[i][j] = "PQ";
                    }

                    cost_t cost_QP = cost(i - 1, edge_cumulated(i - 1, j, P), QP);
                    std::cout<<"QP: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), QP)<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_QP > 0 && DP_P[i - 1][j] + cost_QP < DP_P[i][j]) {
                        DP_P[i][j] = DP_P[i - 1][j] + cost_QP;
                        order_P[i][j] = "QP";
                    }
                }
                // We use Q[i-1][j] to calculate next subproblems
                if(DP_Q[i - 1][j] != std::numeric_limits<cost_t>::max()) {
                    // Contraction PC -> Q[i][i-1]
                    cost_t cost_PC = cost(i - 1, edge_cumulated(i - 1, j, Q), PC);
                    std::cout<<"PC: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), PC)<<" "<<cost_PC<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_PC > 0 && cost_PC < DP_Q[i - 1][j] + DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] = DP_Q[i - 1][j] + cost_PC;
                        order_Q[i][i - 1] = "PC";
                    }

                    cost_t cost_CP = cost(i - 1, edge_cumulated(i - 1, j, Q), CP);
                    std::cout<<"CP: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), CP)<<" "<<cost_CP<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_CP > 0 && DP_Q[i - 1][j] + cost_CP < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] = DP_Q[i - 1][j] + cost_CP;
                        order_Q[i][i - 1] = "CP";
                    }

                    // Contraction QC -> P[i][i-1]
                    cost_t cost_QC = cost(i - 1, edge_cumulated(i - 1, j, Q), QC);
                    std::cout<<"QC: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), QC)<<" "<<cost_QC<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_QC > 0 && DP_Q[i - 1][j] + cost_QC < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_Q[i - 1][j] + cost_QC;
                        order_P[i][i - 1] = "QC";
                    }

                    cost_t cost_CQ = cost(i - 1, edge_cumulated(i - 1, j, Q), CQ);
                    std::cout<<"CQ: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), CQ)<<" "<<cost_CQ<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_CQ > 0 && DP_Q[i - 1][j] + cost_CQ < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_Q[i - 1][j] + cost_CQ;
                        order_P[i][i - 1] = "CQ";
                    }

                    // Contraction PQ -> Q[i][j]
                    cost_t cost_PQ = cost(i - 1, edge_cumulated(i - 1, j, Q), PQ);
                    std::cout<<"PQ: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), PQ)<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_PQ > 0 && DP_Q[i - 1][j] + cost_PQ < DP_Q[i][j]) {
                        DP_Q[i][j] = DP_Q[i - 1][j] + cost_PQ;
                        order_Q[i][j] = "PQ";
                    }

                    cost_t cost_QP = cost(i - 1, edge_cumulated(i - 1, j, Q), QP);
                    std::cout<<"QP: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), QP)<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_QP > 0 && DP_Q[i - 1][j] + cost_QP < DP_Q[i][j]) {
                        DP_Q[i][j] = DP_Q[i - 1][j] + cost_QP;
                        order_Q[i][j] = "QP";
                    }
                }
            }
            std::cout<<"!!!!!!!!! Results for iteration i = "<<i<<std::endl;
            for (int j = 0; j < i; j++) {
                std::cout<<"j = "<<j<<" :"<<DP_P[i][j]<<" "<<DP_Q[i][j]<<std::endl;
            }
        }

        // Return the minimum cost
        cost_t min_cost = std::numeric_limits<cost_t>::max();
        for(int j = 0; j < this->m_network.dim; j++) {
            min_cost = std::min(min_cost, DP_P[this->m_network.dim + 1][j]);
            min_cost = std::min(min_cost, DP_Q[this->m_network.dim + 1][j]);
        }
        return min_cost;
    }

    cost_t call_solve() {
        // Calculate the dynamic programming table
        this->solve();

        // Return minimum cost out all the possible contractions
        this->best_cost = std::numeric_limits<cost_t>::max();
        for(int j = 0; j < this->m_network.dim + 1; j++) {
            if(DP_P[this->m_network.dim + 1][j] < this->best_cost) {
                this->best_cost = DP_P[this->m_network.dim + 1][j];
                this->best_order_str = order_P[this->m_network.dim + 1][j];
            }

            if(DP_Q[this->m_network.dim + 1][j] < this->best_cost) {
                this->best_cost = DP_Q[this->m_network.dim + 1][j];
                this->best_order_str = order_Q[this->m_network.dim + 1][j];
            }
        }

        return this->best_cost;
    }
};

#endif