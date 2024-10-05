#ifndef TWOSIDEDSWEEPING_HPP
#define TWOSIDEDSWEEPING_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

// Type of the contraction
enum contraction_type_e {
    QC = 12,
    CQ = 21,
    PC =  2,
    CP = 20,
    PQ =  1,
    QP = 10
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
        // Clear the variables
        DP_P.resize(this->m_network.dim + 2, std::vector<cost_t>(this->m_network.dim + 2, std::numeric_limits<cost_t>::max()));
        DP_Q.resize(this->m_network.dim + 2, std::vector<cost_t>(this->m_network.dim + 2, std::numeric_limits<cost_t>::max()));

        order_P.resize(this->m_network.dim + 2, std::vector<std::string>(this->m_network.dim + 2, ""));
        order_Q.resize(this->m_network.dim + 2, std::vector<std::string>(this->m_network.dim + 2, ""));

        // Reverse the network if the direction is from the right
        if(direction == RL) {
            const int end_up = this->m_network.dim - 1;
            const int end_down = 2 * this->m_network.dim - 1;

            for(int i = 0; i < this->m_network.dim / 2; i++) {
                // Reverse edge c_i
                std::swap(this->m_network[i, this->m_network.dim + i], this->m_network[end_up - i, end_down - i]);

                // Reverse edge p_i
                if(i + 1 < this->m_network.dim) {
                    std::swap(this->m_network[i, i + 1], this->m_network[end_up - i - 1, end_up - i]);
                }

                // Reverse edge q_i
                if(i + 1 < this->m_network.dim) {
                    std::swap(this->m_network[this->m_network.dim + i, this->m_network.dim + i + 1], this->m_network[end_down - i - 1, end_down - i]);
                }
            }

            std::cout<<"Network:"<<std::endl;
            for(int i = 0; i < 2*this->m_network.dim; i++) {
                for(int j = 0; j < 2*this->m_network.dim; j++) {
                    std::cout<<i<<" "<<j<<" "<<this->m_network[i, j, true]<<std::endl;
                }
            }
        }
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

    std::pair<int, int> generate_operation(int s, int operation_id, result_direction_e direction) {
        s--; // offset for 0-based indexing of network definition
        std::pair<vertexID_t, vertexID_t> operation;

        const int end_up = this->m_network.n_vertex / 2 - 1;
        const int end_down = this->m_network.n_vertex - 1;

        switch(operation_id){
            case P:
                if(direction == LR) operation = {s, s + 1};
                else                operation = {end_up - s, end_up - (s + 1)};
            break;
            case Q:
                if(direction == LR) operation = {this->m_network.n_vertex / 2 + s, this->m_network.n_vertex / 2 + (s + 1)};
                else                operation = {end_down - s, end_down - (s + 1)};
            break;
            case C:
                if(direction == LR) operation = {s, this->m_network.n_vertex / 2 + s};
                else                operation = {end_up - s, end_down - s};
                
            break;
        }

        return operation;
    }

    std::string build_order(int i, contraction_type_e type, std::string order, result_direction_e direction) {
        // Don't generate operations for artifical vertices beyond the network size
        std::pair<vertexID_t, vertexID_t> operation1 = generate_operation(i, type / 10, direction);
        std::pair<vertexID_t, vertexID_t> operation2 = generate_operation(i, type % 10, direction);

        // Generate the order string
        if(order != "") {
            order += ", (" + std::to_string(operation1.first) + ", " + std::to_string(operation1.second) + ")";
        } else {
            order = "(" + std::to_string(operation1.first) + ", " + std::to_string(operation1.second) + ")";
        }

        // Add second operation if it is not artifical contraction at the end of the tensor-train
        if(operation2.first < this->m_network.n_vertex && operation2.second < this->m_network.n_vertex) {
            order += ", (" + std::to_string(operation2.first) + ", " + std::to_string(operation2.second) + ")";
        }

        return order;
    }

    cost_t solve(result_direction_e direction = LR) {
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
                        order_Q[i][i - 1] = this->build_order(i - 1, PC, order_P[i - 1][j], direction);
                    }

                    cost_t cost_CP = cost(i - 1, edge_cumulated(i - 1, j, P), CP);
                    std::cout<<"CP: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), CP)<<" "<<cost_CP<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_CP > 0 &&  DP_P[i - 1][j] + cost_CP < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] =  DP_P[i - 1][j] + cost_CP;
                        order_Q[i][i - 1] = this->build_order(i - 1, CP, order_P[i - 1][j], direction);
                    }

                    // Contraction QC -> P[i][i-1]
                    cost_t cost_QC = cost(i - 1, edge_cumulated(i - 1, j, P), QC);
                    std::cout<<"QC: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), QC)<<" "<<cost_QC<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_QC > 0 && DP_P[i - 1][j] + cost_QC < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_P[i - 1][j] + cost_QC;
                        order_P[i][i - 1] = this->build_order(i - 1, QC, order_P[i - 1][j], direction);
                    }

                    cost_t cost_CQ = cost(i - 1, edge_cumulated(i - 1, j, P), CQ);
                    std::cout<<"CQ: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), CQ)<<" "<<cost_CQ<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_CQ > 0 && DP_P[i - 1][j] + cost_CQ < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_P[i - 1][j] + cost_CQ;
                        order_P[i][i - 1] = this->build_order(i - 1, CQ, order_P[i - 1][j], direction);
                    }

                    // Contraction PQ -> P[i][j]
                    cost_t cost_PQ = cost(i - 1, edge_cumulated(i - 1, j, P), PQ);
                    std::cout<<"PQ: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), PQ)<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_PQ > 0 && DP_P[i - 1][j] + cost_PQ < DP_P[i][j]) {
                        DP_P[i][j] = DP_P[i - 1][j] + cost_PQ;
                        order_P[i][j] = this->build_order(i - 1, PQ, order_P[i - 1][j], direction);
                    }

                    cost_t cost_QP = cost(i - 1, edge_cumulated(i - 1, j, P), QP);
                    std::cout<<"QP: "<<i<<" "<<j<<" "<<DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), QP)<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_QP > 0 && DP_P[i - 1][j] + cost_QP < DP_P[i][j]) {
                        DP_P[i][j] = DP_P[i - 1][j] + cost_QP;
                        order_P[i][j] = this->build_order(i - 1, QP, order_P[i - 1][j], direction);
                    }
                }
                // We use Q[i-1][j] to calculate next subproblems
                if(DP_Q[i - 1][j] != std::numeric_limits<cost_t>::max()) {
                    // Contraction PC -> Q[i][i-1]
                    cost_t cost_PC = cost(i - 1, edge_cumulated(i - 1, j, Q), PC);
                    std::cout<<"PC: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), PC)<<" "<<cost_PC<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_PC > 0 && cost_PC < DP_Q[i - 1][j] + DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] = DP_Q[i - 1][j] + cost_PC;
                        order_Q[i][i - 1] = this->build_order(i - 1, PC, order_Q[i - 1][j], direction);
                    }

                    cost_t cost_CP = cost(i - 1, edge_cumulated(i - 1, j, Q), CP);
                    std::cout<<"CP: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), CP)<<" "<<cost_CP<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_CP > 0 && DP_Q[i - 1][j] + cost_CP < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] = DP_Q[i - 1][j] + cost_CP;
                        order_Q[i][i - 1] = this->build_order(i - 1, CP, order_Q[i - 1][j], direction);
                    }

                    // Contraction QC -> P[i][i-1]
                    cost_t cost_QC = cost(i - 1, edge_cumulated(i - 1, j, Q), QC);
                    std::cout<<"QC: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), QC)<<" "<<cost_QC<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_QC > 0 && DP_Q[i - 1][j] + cost_QC < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_Q[i - 1][j] + cost_QC;
                        order_P[i][i - 1] = this->build_order(i - 1, QC, order_Q[i - 1][j], direction);
                    }

                    cost_t cost_CQ = cost(i - 1, edge_cumulated(i - 1, j, Q), CQ);
                    std::cout<<"CQ: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), CQ)<<" "<<cost_CQ<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_CQ > 0 && DP_Q[i - 1][j] + cost_CQ < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_Q[i - 1][j] + cost_CQ;
                        order_P[i][i - 1] = this->build_order(i - 1, CQ, order_Q[i - 1][j], direction);
                    }

                    // Contraction PQ -> Q[i][j]
                    cost_t cost_PQ = cost(i - 1, edge_cumulated(i - 1, j, Q), PQ);
                    std::cout<<"PQ: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), PQ)<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_PQ > 0 && DP_Q[i - 1][j] + cost_PQ < DP_Q[i][j]) {
                        DP_Q[i][j] = DP_Q[i - 1][j] + cost_PQ;
                        order_Q[i][j] = this->build_order(i - 1, PQ, order_Q[i - 1][j], direction);
                    }

                    cost_t cost_QP = cost(i - 1, edge_cumulated(i - 1, j, Q), QP);
                    std::cout<<"QP: "<<i<<" "<<j<<" "<<DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), QP)<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_QP > 0 && DP_Q[i - 1][j] + cost_QP < DP_Q[i][j]) {
                        DP_Q[i][j] = DP_Q[i - 1][j] + cost_QP;
                        order_Q[i][j] = this->build_order(i - 1, QP, order_Q[i - 1][j], direction);
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
        // Initialize the cost variables
        cost_t cost_LR = std::numeric_limits<cost_t>::max();
        cost_t cost_RL = std::numeric_limits<cost_t>::max();
        std::string order_LR = "", order_RL = "";

        // Compute cost of contraction starting from left side
        if(this->m_direction & START_LEFT) {
            this->solve(LR);
            for(int j = 0; j < this->m_network.dim + 1; j++) {
                if(DP_P[this->m_network.dim + 1][j] < cost_LR) {
                    cost_LR = DP_P[this->m_network.dim + 1][j];
                    order_LR = "(" + order_P[this->m_network.dim + 1][j] + ")";
                }

                if(DP_Q[this->m_network.dim + 1][j] < cost_LR) {
                    cost_LR = DP_Q[this->m_network.dim + 1][j];
                    order_LR = "(" + order_Q[this->m_network.dim + 1][j] + ")";
                }
            }

            std::cout<<"Cost_LR: "<<cost_LR<<std::endl;
            std::cout<<"Order_LR: "<<order_LR<<std::endl;
        }

        // Find the best cost and corresponding order
        if(this->m_direction & START_RIGHT & false) {
            // Reverse the network
            this->init_variables(RL);

            // Compute the cost and retrieve order
            this->solve(RL);
            for(int j = 0; j < this->m_network.dim + 1; j++) {
                if(DP_P[this->m_network.dim + 1][j] < cost_RL) {
                    cost_RL = DP_P[this->m_network.dim + 1][j];
                    order_RL = "(" + order_P[this->m_network.dim + 1][j] + ")";
                }

                if(DP_Q[this->m_network.dim + 1][j] < cost_RL) {
                    cost_RL = DP_Q[this->m_network.dim + 1][j];
                    order_RL = "(" + order_Q[this->m_network.dim + 1][j] + ")";
                }
            }

            std::cout<<"Cost_RL: "<<cost_RL<<std::endl;
            std::cout<<"Order_RL: "<<order_RL<<std::endl;
        }

        // Find the best cost and corresponding order
        if(cost_LR <= cost_RL) {
            this->best_cost = cost_LR - 1;
            this->best_order_str = order_LR;
        } else {
            this->best_cost = cost_RL - 1;
            this->best_order_str = order_RL;
        }

        return this->best_cost;
    }
};

#endif