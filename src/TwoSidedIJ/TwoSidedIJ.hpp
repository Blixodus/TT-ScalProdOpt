#ifndef TWOSIDEDDIJ_HPP
#define TWOSIDEDDIJ_HPP
#include "../Components/Components.hpp"
#include "../Components/Network.hpp"
#include "../CotengraWrapper/CotengraWrapper.hpp"

#define COST first
#define ORDER second

template <size_t delta = 3, split_direction_e dir = ALL>
class TwoSidedIJ : public Algorithm {
    public:
    // The memoization table for the costs of each state
    std::vector<std::vector<std::vector<std::vector<cost_t>>>> m_cost[2];

    // The memoization table for the order of the splits
    std::vector<std::vector<std::vector<std::vector<std::string>>>> m_order[2];

    // Exact solver for the subproblems (Cotengra optimal algorithm)
    CotengraWrapper<2> m_exact_solver;

    // Generalized network information
    Network<2> m_network;

    // Constructors
    TwoSidedIJ(){}
    TwoSidedIJ(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary) {}

    // Initializers
    void init(std::string filename) {
        // Initialize network 2D
        this->m_network = Network<2>(filename);

        // Initialize the memoization tables
        for(int type = 0; type < 2; type++) {
            this->m_cost[type].resize(this->m_network.dim);
            this->m_order[type].resize(this->m_network.dim);
            for(int t = 0; t < this->m_network.dim; t++) {
                this->m_cost[type][t].resize(this->m_network.dim);
                this->m_order[type][t].resize(this->m_network.dim);
                for(int s = 0; s < this->m_network.dim; s++) {
                    this->m_cost[type][t][s].resize(this->m_network.dim);
                    this->m_order[type][t][s].resize(this->m_network.dim);
                    for(int u = 0; u < this->m_network.dim; u++) {
                        this->m_cost[type][t][s][u].resize(this->m_network.dim);
                        this->m_order[type][t][s][u].resize(this->m_network.dim);
                        for(int v = 0; v < this->m_network.dim; v++) {
                            this->m_cost[type][t][s][u][v] = std::numeric_limits<cost_t>::max();
                        }
                    }
                }
            }
        }

        // Initialize the result variables
        this->best_cost = std::numeric_limits<cost_t>::max();
        this->best_order_str = "ORDER_NOT_FOUND";

        // Initialize the exact solver
        this->m_exact_solver.init(this->m_network);
    }
    
    // Computation functions
    void compute_splits() {
        // Calculate the optimal cost of all the possible windows
        if constexpr(dir & START_LEFT) {
            // Iterate over windows of size t up to Δ
            for(int t = 1; t <= delta; t++) {
                // Iterate over all the possible starting points and solve
                // ones with size below Δ threshold
                for(int i1 = 0; i1 < this->m_network.dim; i1++) {
                    for(int i2 = i1; i2 < this->m_network.dim; i2++) {
                        for(int j1 = 0; j1 < this->m_network.dim; j1++) {
                            for(int j2 = j1; j2 < this->m_network.dim; j2++) {
                                // Check if the ranges overlap and the size of the window is below Δ
                                if(i1 <= j2 && j1 <= i2 && i2 - i1 + j2 - j1 + 2 <= delta) {
                                    std::pair<cost_t, std::string> optimal_result = m_exact_solver.solve(i1, i2, j1, j2, LR);
                                    this->m_cost[LR][i1][i2][j1][j2] = optimal_result.COST;
                                    this->m_order[LR][i1][i2][j1][j2] = optimal_result.ORDER;
                                }
                            }
                        }
                    }
                }
            }

            // Iterate over subproblems larger than Δ up to the full dimension
            // In this case the algorithm performs the splits into two subproblems
            // of size t up Δ and D-t, and solves the first one optimally and the
            // second one recursively
            for(int len_up = 1; len_up <= delta; len_up++) {
                for(int len_down = 1; len_down <= delta; len_down++) {
                    // Iterate over all the possible windows
                    for(int i1 = 0; i1 < this->m_network.dim - len_up; i1++) {
                        int i2 = i1 + len_up - 1;
                        for(int j1 = 0; j1 < this->m_network.dim - len_down; j1++) {
                            int j2 = j1 + len_down - 1;
                            // Solve the window [i1, i2][j1, j2] by splitting it into two subproblems
                            if(i1 <= j2 && j1 <= i2) {
                                for(int i3 = i1; i3 <= i2; i3++) {
                                    for(int j3 = j1; j3 <= j2; j3++) {
                                        if(i3 == i2 && j3 == j2) continue;
                                        auto cost_L = this->m_cost[LR][i1][i3][j1][j3];
                                        auto order_L = this->m_order[LR][i1][i3][j1][j3];
                                        auto cost_R = this->m_cost[LR][i3 + 1][i2][j3 + 1][j2];
                                        auto order_R = this->m_order[LR][i3 + 1][i2][j3 + 1][j2];
                                        if(cost_L != std::numeric_limits<cost_t>::max() && cost_R != std::numeric_limits<cost_t>::max()) {
                                            cost_t cost = cost_L + cost_R;
                                            if(cost < this->m_cost[LR][i1][i2][j1][j2]) {
                                                // Update the cost of the window [s, s+t-1]
                                                this->m_cost[LR][i1][i2][j1][j2] = cost;

                                                // Update the order of the splits by concatenating
                                                // the two recursive results (right part assumes that
                                                // the left part is already contracted - denoted by #,
                                                // and now we are inserting the exact order to yield it)
                                                this->m_order[LR][i1][i2][j1][j2] = order_R;
                                                this->m_order[LR][i1][i2][j1][j2].replace(this->m_order[LR][i1][i2][j1][j2].find("'#'"), 3, order_L);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Calculate the optimal cost of all the possible windows starting from the right
        if constexpr(dir & START_RIGHT) {
            // Iterate over windows of size t up to Δ
            for(int t = 1; t <= delta; t++) {
                // Iterate over all the possible starting points and solve
                // ones with size below Δ threshold
                for(int i1 = 0; i1 < this->m_network.dim; i1++) {
                    for(int i2 = i1; i2 < this->m_network.dim; i2++) {
                        for(int j1 = 0; j1 < this->m_network.dim; j1++) {
                            for(int j2 = j1; j2 < this->m_network.dim; j2++) {
                                // Check if the ranges overlap and the size of the window is below Δ
                                if(i1 <= j2 && j1 <= i2 && i2 - i1 + j2 - j1 + 2 <= delta) {
                                    std::pair<cost_t, std::string> optimal_result = m_exact_solver.solve(i1, i2, j1, j2, RL);
                                    this->m_cost[RL][i1][i2][j1][j2] = optimal_result.COST;
                                    this->m_order[RL][i1][i2][j1][j2] = optimal_result.ORDER;
                                }
                            }
                        }
                    }
                }
            }

            // Iterate over subproblems larger than Δ up to the full dimension
            // In this case the algorithm performs the splits into two subproblems
            // of size D-t and t up Δ, and solves the first one recursively and the
            // second one optimally
            for(int len_up = 1; len_up <= delta; len_up++) {
                for(int len_down = 1; len_down <= delta; len_down++) {
                    // Iterate over all the possible windows
                    for(int i1 = 0; i1 < this->m_network.dim - len_up; i1++) {
                        int i2 = i1 + len_up - 1;
                        for(int j1 = 0; j1 < this->m_network.dim - len_down; j1++) {
                            int j2 = j1 + len_down - 1;
                            // Solve the window [i1, i2][j1, j2] by splitting it into two subproblems
                            if(i1 <= j2 && j1 <= i2) {
                                for(int i3 = i1; i3 <= i2; i3++) {
                                    for(int j3 = j1; j3 <= j2; j3++) {
                                        if(i3 == i2 && j3 == j2) continue;
                                        auto cost_L = this->m_cost[RL][i1][i3][j1][j3];
                                        auto order_L = this->m_order[RL][i1][i3][j1][j3];
                                        auto cost_R = this->m_cost[RL][i3 + 1][i2][j3 + 1][j2];
                                        auto order_R = this->m_order[RL][i3 + 1][i2][j3 + 1][j2];
                                        if(cost_L != std::numeric_limits<cost_t>::max() && cost_R != std::numeric_limits<cost_t>::max()) {
                                            cost_t cost = cost_L + cost_R;
                                            if(cost < this->m_cost[RL][i1][i2][j1][j2]) {
                                                // Update the cost of the window [s, s+t-1]
                                                this->m_cost[RL][i1][i2][j1][j2] = cost;

                                                // Update the order of the splits by concatenating
                                                // the two recursive results (left part assumes that
                                                // the right part is already contracted - denoted by #,
                                                // and now we are inserting the exact order to yield it)
                                                this->m_order[RL][i1][i2][j1][j2] = order_L;
                                                this->m_order[RL][i1][i2][j1][j2].replace(this->m_order[RL][i1][i2][j1][j2].find("'#'"), 3, order_R);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cost_t solve(result_direction_e direction) {
        return this->m_cost[direction][0][this->m_network.dim - 1][0][this->m_network.dim - 1];
    }

    cost_t call_solve() {
        // Compute the optimal cost of all the possible splits
        this->compute_splits();

        // Include the cost of starting from the left side of TT
        if constexpr(dir & START_LEFT) {
            if(this->m_cost[LR][0][this->m_network.dim - 1][0][this->m_network.dim - 1] < this->best_cost) {
                this->best_cost = this->m_cost[LR][0][this->m_network.dim - 1][0][this->m_network.dim - 1];
                this->best_order_str = this->m_order[LR][0][this->m_network.dim - 1][0][this->m_network.dim - 1];
            }
        }

        // Include the cost of starting from the right side of TT
        if constexpr(dir & START_RIGHT) {
            if(this->m_cost[RL][0][this->m_network.dim - 1][0][this->m_network.dim - 1] < this->best_cost) {
                this->best_cost = this->m_cost[RL][0][this->m_network.dim - 1][0][this->m_network.dim - 1];
                this->best_order_str = this->m_order[RL][0][this->m_network.dim - 1][0][this->m_network.dim - 1];
            }
        }

        // Include the cost of starting from both sides of TT
        if constexpr(dir & ALL) {
            // Iterate over all the possible starting splits
            for(int split_i = 0; split_i < this->m_network.dim; split_i++) {
                for(int split_j = 0; split_j < this->m_network.dim; split_j++) {
                    if(split_i == 0 && split_j == 0) continue;
                    if(split_i == this->m_network.dim - 1 && split_j == this->m_network.dim - 1) continue;

                    // Retrieve the cost of the left part
                    cost_t cost_left = this->m_cost[LR][0][split_i][0][split_j];

                    // Retrieve the cost of the right part
                    cost_t cost_right = this->m_cost[RL][split_i + 1][this->m_network.dim - 1][split_j + 1][this->m_network.dim - 1];

                    // Compute the cost of contracting the final two nodes together
                    // (results of the two subproblems)
                    cost_t cost_connect = 1;
                    cost_connect *= this->m_network[split_i - 1, split_i];
                    cost_connect *= this->m_network[this->m_network.dim + (split_j - 1), this->m_network.dim + split_j];

                    // Calculate the total cost of the split
                    cost_t total_cost = cost_left + cost_right + cost_connect;

                    // Update the best cost
                    if(total_cost < this->best_cost) {
                        this->best_cost = total_cost;
                        this->best_order_str = "(" + this->m_order[LR][0][split_i][0][split_j] + ", " + this->m_order[RL][split_i + 1][this->m_network.dim - 1][split_j + 1][this->m_network.dim - 1] + ")";
                    }
                }
            }
        }

        return this->best_cost;
    }
};

#endif