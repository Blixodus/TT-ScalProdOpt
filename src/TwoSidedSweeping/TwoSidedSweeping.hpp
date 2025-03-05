#ifndef TWOSIDEDSWEEPING_HPP
#define TWOSIDEDSWEEPING_HPP
#include "../Components/Components.hpp"
#include "../Components/Algorithm.hpp"

#define NOT_CALCULATED -1

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

using operation_t = std::pair<int, std::pair<edge_type_e, contraction_type_e>>;

class TwoSidedSweeping : public Algorithm {
    private:
    // Solution parameters
    split_direction_e m_direction;  // Direction for the start of the contraction

    // Network information
    Network<2> m_network;        // Generalized network information
    //std::vector<cost_t> cost[3]; // Cost of contracting subproblem [j, i] starting with edge P, Q or C

    std::vector<std::vector<long double>> DP_P, DP_Q; // Cost of contracting subproblem [1, i] with keeping edge P, Q or C
    std::vector<std::vector<operation_t>> order_P, order_Q; // Order of contraction of subproblem [1, i] with keeping edge P, Q or C
    std::vector<std::vector<long double>> edge[3];

    // Initializer
    void init_variables(result_direction_e direction) {
        // Clear the variables
        DP_P.clear();
        DP_Q.clear();
        DP_P.resize(this->m_network.dim + 2, std::vector<long double>(this->m_network.dim + 2, std::numeric_limits<long double>::max()));
        DP_Q.resize(this->m_network.dim + 2, std::vector<long double>(this->m_network.dim + 2, std::numeric_limits<long double>::max()));

        order_P.clear();
        order_Q.clear();
        order_P.resize(this->m_network.dim + 2, std::vector<operation_t>(this->m_network.dim + 2, {0, {P, QC}}));
        order_Q.resize(this->m_network.dim + 2, std::vector<operation_t>(this->m_network.dim + 2, {0, {P, QC}}));

        edge[P].clear();
        edge[Q].clear();
        edge[C].clear();
        edge[P].resize(this->m_network.dim + 2, std::vector<long double>(this->m_network.dim + 2, NOT_CALCULATED));
        edge[Q].resize(this->m_network.dim + 2, std::vector<long double>(this->m_network.dim + 2, NOT_CALCULATED));
        edge[C].resize(this->m_network.dim + 2, std::vector<long double>(this->m_network.dim + 2, NOT_CALCULATED));

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

            //std::cout<<"Network:"<<std::endl;
            for(int i = 0; i < 2*this->m_network.dim; i++) {
                for(int j = 0; j < 2*this->m_network.dim; j++) {
                    //std::cout<<i<<" "<<j<<" "<<this->m_network[i, j, true]<<std::endl;
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
        /*if(i == 2 && c == 51 && type == CQ) {
            //std::cout<<"\n\t"<<get_edge(P, i)<<" "<<get_edge(Q, i)<<" "<<c<<" "<<get_edge(P, i + 1)<<" "<<get_edge(Q, i + 1)<<" "<<get_edge(C, i + 1)<<" "<<type<<" "<<contraction_cost(get_edge(P, i), get_edge(Q, i), c, get_edge(P, i + 1), get_edge(Q, i + 1), get_edge(C, i + 1), type)<<std::endl;
        }*/
        return contraction_cost(get_edge(P, i), get_edge(Q, i), c, get_edge(P, i + 1), get_edge(Q, i + 1), get_edge(C, i + 1), type);
    }

    cost_t edge_cumulated(int i, int j, edge_type_e type)
    {
        if(this->edge[type][i][j] != NOT_CALCULATED) {
            return this->edge[type][i][j];
        }

        cost_t weight = get_edge(type, j);
        for(int k = j + 1; k < i; k++) {
            weight *= get_edge(C, k);
        }
        weight *= get_edge(C, i);

        this->edge[type][i][j] = weight;
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

    std::string generate_order(int i, int j, edge_type_e edge, contraction_type_e type, result_direction_e direction) {
        //std::cout<<"Generate order: "<<i<<" "<<j<<" "<<edge<<" "<<type<<" "<<direction<<std::endl;

        // Return empty order for artifical part of the tensor-train
        if(i <= 1) {
            return "";
        }

        // Retrieve the contraction used to obtain the current edge
        auto order = this->order_P[i][j];
        if(edge == Q) {
            order = this->order_Q[i][j];
        }
        
        // Get order of previous subproblem
        std::string order_str = this->generate_order(i - 1, order.first, order.second.first, order.second.second, direction);

        // Don't generate operations for artifical vertices beyond the network size
        std::pair<vertexID_t, vertexID_t> operation1 = generate_operation(i - 1, order.second.second / 10, direction);
        std::pair<vertexID_t, vertexID_t> operation2 = generate_operation(i - 1, order.second.second % 10, direction);

        if(i == this->m_network.dim + 1) {
            if(direction == LR) {
                return "(" + order_str + ", (" + std::to_string(this->m_network.dim - 1) + ", " + std::to_string(2 * this->m_network.dim - 1) + "))";
            } else {
                return "(" + order_str + ", (" + std::to_string(0) + ", " + std::to_string(this->m_network.dim) + "))";
            }
        }

        // Generate the order string
        if(order_str != "") {
            order_str += ", (" + std::to_string(operation1.first) + ", " + std::to_string(operation1.second) + ")";
        } else {
            order_str = "(" + std::to_string(operation1.first) + ", " + std::to_string(operation1.second) + ")";
        }
        order_str += ", (" + std::to_string(operation2.first) + ", " + std::to_string(operation2.second) + ")";

        return order_str;
    }

    cost_t solve(result_direction_e direction) {
        // Base case for contracting the first edge
        DP_P[0][0] = 0;
        DP_Q[0][0] = 0;
        DP_P[1][0] = 0;
        DP_Q[1][0] = 0;

        // Iterating over subnetworks [i...dim-1]
        for(int i = 2; i <= this->m_network.dim + 1; i++) { 
            //std::cout<<"Iteration: "<<i<<std::endl;
            for(int j = 0; j < i; j++) {
                // We use P[i-1][j] to calculate next subproblems
                if(DP_P[i - 1][j] != std::numeric_limits<long double>::max()) {
                    // Contraction PC -> Q[i][i-1]
                    //std::cout<<"---------- P ----------"<<std::endl;
                    cost_t cost_PC = cost(i - 1, edge_cumulated(i - 1, j, P), PC);
                    //std::cout<<"PC: "<<i<<" "<<j<<" "<<(long long)DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), PC)<<" "<<cost_PC<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_PC > 0 && DP_P[i - 1][j] + cost_PC < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] = DP_P[i - 1][j] + cost_PC;
                        order_Q[i][i - 1] = {j, {P, PC}};
                    }

                    cost_t cost_CP = cost(i - 1, edge_cumulated(i - 1, j, P), CP);
                    //std::cout<<"CP: "<<i<<" "<<j<<" "<<(long long)DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), CP)<<" "<<cost_CP<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_CP > 0 &&  DP_P[i - 1][j] + cost_CP < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] =  DP_P[i - 1][j] + cost_CP;
                        order_Q[i][i - 1] = {j, {P, CP}};
                    }

                    // Contraction QC -> P[i][i-1]
                    cost_t cost_QC = cost(i - 1, edge_cumulated(i - 1, j, P), QC);
                    //std::cout<<"QC: "<<i<<" "<<j<<" "<<(long long)DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), QC)<<" "<<cost_QC<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_QC > 0 && DP_P[i - 1][j] + cost_QC < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_P[i - 1][j] + cost_QC;
                        order_P[i][i - 1] = {j, {P, QC}};
                    }

                    cost_t cost_CQ = cost(i - 1, edge_cumulated(i - 1, j, P), CQ);
                    //std::cout<<"CQ: "<<i<<" "<<j<<" "<<(long long)DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), CQ)<<" "<<cost_CQ<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_CQ > 0 && DP_P[i - 1][j] + cost_CQ < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_P[i - 1][j] + cost_CQ;
                        order_P[i][i - 1] = {j, {P, CQ}};
                    }

                    // Contraction PQ -> P[i][j]
                    cost_t cost_PQ = cost(i - 1, edge_cumulated(i - 1, j, P), PQ);
                    //std::cout<<"PQ: "<<i<<" "<<j<<" "<<(long long)DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), PQ)<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_PQ > 0 && DP_P[i - 1][j] + cost_PQ < DP_P[i][j]) {
                        DP_P[i][j] = DP_P[i - 1][j] + cost_PQ;
                        order_P[i][j] = {j, {P, PQ}};
                    }

                    cost_t cost_QP = cost(i - 1, edge_cumulated(i - 1, j, P), QP);
                    //std::cout<<"QP: "<<i<<" "<<j<<" "<<(long long)DP_P[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, P), QP)<<" "<<edge_cumulated(i - 1, j, P)<<std::endl;
                    if(cost_QP > 0 && DP_P[i - 1][j] + cost_QP < DP_P[i][j]) {
                        DP_P[i][j] = DP_P[i - 1][j] + cost_QP;
                        order_P[i][j] = {j, {P, QP}};
                    }
                }
                // We use Q[i-1][j] to calculate next subproblems
                if(DP_Q[i - 1][j] != std::numeric_limits<long double>::max()) {
                    // Contraction PC -> Q[i][i-1]
                    //std::cout<<"---------- Q ----------"<<std::endl;
                    cost_t cost_PC = cost(i - 1, edge_cumulated(i - 1, j, Q), PC);
                    //std::cout<<"PC: "<<i<<" "<<j<<" "<<(long long)DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), PC)<<" "<<cost_PC<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_PC > 0 && DP_Q[i - 1][j] + cost_PC < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] = DP_Q[i - 1][j] + cost_PC;
                        order_Q[i][i - 1] = {j, {Q, PC}};
                    }

                    cost_t cost_CP = cost(i - 1, edge_cumulated(i - 1, j, Q), CP);
                    //std::cout<<"CP: "<<i<<" "<<j<<" "<<(long long)DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), CP)<<" "<<cost_CP<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_CP > 0 && DP_Q[i - 1][j] + cost_CP < DP_Q[i][i - 1]) {
                        DP_Q[i][i - 1] = DP_Q[i - 1][j] + cost_CP;
                        order_Q[i][i - 1] = {j, {Q, CP}};
                    }

                    // Contraction QC -> P[i][i-1]
                    cost_t cost_QC = cost(i - 1, edge_cumulated(i - 1, j, Q), QC);
                    //std::cout<<"QC: "<<i<<" "<<j<<" "<<(long long)DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), QC)<<" "<<cost_QC<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_QC > 0 && DP_Q[i - 1][j] + cost_QC < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_Q[i - 1][j] + cost_QC;
                        order_P[i][i - 1] = {j, {Q, QC}};
                    }

                    cost_t cost_CQ = cost(i - 1, edge_cumulated(i - 1, j, Q), CQ);
                    //std::cout<<"CQ: "<<i<<" "<<j<<" "<<(long long)DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), CQ)<<" "<<cost_CQ<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_CQ > 0 && DP_Q[i - 1][j] + cost_CQ < DP_P[i][i - 1]) {
                        DP_P[i][i - 1] = DP_Q[i - 1][j] + cost_CQ;
                        order_P[i][i - 1] = {j, {Q, CQ}};
                    }

                    // Contraction PQ -> Q[i][j]
                    cost_t cost_PQ = cost(i - 1, edge_cumulated(i - 1, j, Q), PQ);
                    //std::cout<<"PQ: "<<i<<" "<<j<<" "<<(long long)DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), PQ)<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_PQ > 0 && DP_Q[i - 1][j] + cost_PQ < DP_Q[i][j]) {
                        DP_Q[i][j] = DP_Q[i - 1][j] + cost_PQ;
                        order_Q[i][j] = {j, {Q, PQ}};
                    }

                    cost_t cost_QP = cost(i - 1, edge_cumulated(i - 1, j, Q), QP);
                    //std::cout<<"QP: "<<i<<" "<<j<<" "<<(long long)DP_Q[i - 1][j]<<" "<<cost(i - 1, edge_cumulated(i - 1, j, Q), QP)<<" "<<edge_cumulated(i - 1, j, Q)<<std::endl;
                    if(cost_QP > 0 && DP_Q[i - 1][j] + cost_QP < DP_Q[i][j]) {
                        DP_Q[i][j] = DP_Q[i - 1][j] + cost_QP;
                        order_Q[i][j] = {j, {Q, QP}};
                    }
                }
            }
            /*
            //std::cout<<"!!!!!!!!! Results for iteration i = "<<i<<std::endl;
            //for (int j = 0; j < i; j++) {
                //std::cout<<"j = "<<j<<" :"<<(long long)DP_P[i][j]<<" "<<(long long)DP_Q[i][j]<<std::endl;
            //}
            */
        }

        // Return the minimum cost
        long double min_cost = std::numeric_limits<long double>::max();
        for(int j = 0; j < this->m_network.dim; j++) {
            min_cost = std::min(min_cost, DP_P[this->m_network.dim + 1][j]);
            min_cost = std::min(min_cost, DP_Q[this->m_network.dim + 1][j]);
        }
        return min_cost;
    }

    cost_t call_solve() {
        // Initialize the cost variables
        long double cost_LR = std::numeric_limits<long double>::max();
        long double cost_RL = std::numeric_limits<long double>::max();
        std::string order_LR = "", order_RL = "";

        // Compute cost of contraction starting from left side
        if(this->m_direction & START_LEFT) {
            this->solve(LR);
            for(int j = 0; j < this->m_network.dim + 1; j++) {
                if(DP_P[this->m_network.dim + 1][j] < cost_LR) {
                    cost_LR = DP_P[this->m_network.dim + 1][j];
                    order_LR = this->generate_order(this->m_network.dim + 1, j, P, PQ, LR);
                }

                if(DP_Q[this->m_network.dim + 1][j] < cost_LR) {
                    cost_LR = DP_Q[this->m_network.dim + 1][j];
                    order_LR = this->generate_order(this->m_network.dim + 1, j, Q, PQ, LR);
                }
            }

            std::cout<<"Cost_LR: "<<cost_LR<<std::endl;
            std::cout<<"Order_LR: "<<order_LR<<std::endl;
        }

        // Find the best cost and corresponding order
        if(this->m_direction & START_RIGHT) {
            // Reverse the network
            this->init_variables(RL);

            // Compute the cost and retrieve order
            this->solve(RL);
            for(int j = 0; j < this->m_network.dim + 1; j++) {
                if(DP_P[this->m_network.dim + 1][j] < cost_RL) {
                    cost_RL = DP_P[this->m_network.dim + 1][j];
                    order_RL = this->generate_order(this->m_network.dim + 1, j, P, PQ, RL);
                }

                if(DP_Q[this->m_network.dim + 1][j] < cost_RL) {
                    cost_RL = DP_Q[this->m_network.dim + 1][j];
                    order_RL = this->generate_order(this->m_network.dim + 1, j, Q, PQ, RL);
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