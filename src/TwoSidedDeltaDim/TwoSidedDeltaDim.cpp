#include "TwoSidedDeltaDim.hpp"
#include <iostream>
#include <algorithm>

#define COST first
#define ORDER second

template <size_t delta, split_direction_e dir, size_t tt_dim>
void TwoSidedDeltaDim<delta, dir, tt_dim>::compute_splits() {
    // Calculate the optimal cost of all the possible windows starting from the left
    if constexpr(dir & START_LEFT) {
        // Iterate over windows of size t up to Δ
        for(int t = 1; t <= delta; t++) {
            // Iterate over all the possible starting points
            for(int s = 0; s < (this->dim - t + 1); s++) {
                // Solve the window [s, s+t-1] optimally and store the result
                // in the memoization table
                std::pair<cost_t, std::string> optimal_result = m_exact_solver.solve(s, s + t - 1, LR);
                this->m_cost[LR][t][s] = optimal_result.COST;
                this->m_order[LR][t][s] = optimal_result.ORDER;
                //std::cout<<"[Window LR: " << t << "]\t" << s << " ... " << s + t - 1 << " - cost: " << this->m_cost[LR][t][s]<<std::endl;
            }
        }

        // Iterate over subproblems larger than Δ up to the full dimension
        // In this case the algorithm performs the splits into two subproblems
        // of size t up Δ and D-t, and solves the first one optimally and the
        // second one recursively
        for(int t = delta + 1; t <= this->dim; t++) {
            // Iterate over all the possible starting points
            for(int s = 0; s < (this->dim - t + 1); s++) {
                // Solve the window [s, s+t-1] by splitting it into two subproblems
                for(int k = 1; k <= delta; k++) {
                    //if(this->m_cost[LR][k][s] != std::numeric_limits<cost_t>::max() &&
                    //   this->m_cost[LR][t - k][s + k] != std::numeric_limits<cost_t>::max()) {
                    cost_t cost = this->m_cost[LR][k][s] + this->m_cost[LR][t - k][s + k];
                    if(cost < this->m_cost[LR][t][s]) {
                        // Update the cost of the window [s, s+t-1]
                        this->m_cost[LR][t][s] = cost;

                        // Update the order of the splits by concatenating
                        // the two recursive results (right part assumes that
                        // the left part is already contracted - denoted by #,
                        // and now we are inserting the exact order to yield it)
                        this->m_order[LR][t][s] = this->m_order[LR][t - k][s + k];
                        this->m_order[LR][t][s].replace(this->m_order[LR][t][s].find("'#'"), 3, this->m_order[LR][k][s]);
                    }
                    //std::cout<<"[Extended window LR: " << t << "]\t" << s << " ... " << s + t - 1 << " - cost: " << this->m_cost[LR][k][s] + this->m_cost[LR][t - k][s + k] << " (" << this->m_cost[LR][k][s] << ", " << this->m_cost[LR][t - k][s + k] << ")" <<std::endl;
                    //std::cout<<"\t"<<this->m_cost[LR][6][0]<<std::endl;
                }
            }
        }
    }

    // Calculate the optimal cost of all the possible windows starting from the right
    if constexpr(dir & START_RIGHT) {
        //std::cout<<"\t"<<this->m_cost[LR][6][0]<<" "<<this->m_cost[RL][6][0]<<std::endl;
        // Iterate over windows of size t up to Δ
        for(int t = 1; t <= delta; t++) {
            // Iterate over all the possible starting splits
            for(int s = 0; s < (this->dim - t + 1); s++) {
                // Solve the window [s, s+t-1] optimally and store the result
                // in the memoization table
                std::pair<cost_t, std::string> optimal_result = m_exact_solver.solve(s, s + t - 1, RL);
                this->m_cost[RL][t][s] = optimal_result.COST;
                this->m_order[RL][t][s] = optimal_result.ORDER;
                //std::cout<<"[Window RL: " << t << "]\t" << s << " ... " << s + t - 1 << " - cost: " << this->m_cost[RL][t][s]<<std::endl;
            }
        }

        //std::cout<<"\t"<<this->m_cost[LR][6][0]<<" "<<this->m_cost[RL][6][0]<<std::endl;

        // Iterate over subproblems larger than Δ up to the full dimension
        // In this case the algorithm performs the splits into two subproblems
        // of size D-t and t up Δ, and solves the first one recursively and the
        // second one optimally
        for(int t = delta + 1; t <= this->dim; t++) {
            // Iterate over all the possible starting points
            for(int s = 0; s < (this->dim - t + 1); s++) {
                // Solve the window [s, s+t-1] by splitting it into two subproblems
                for(int k = 1; k <= delta; k++) {
                    cost_t cost = this->m_cost[RL][t - k][s] + this->m_cost[RL][k][s + t - k];
                    if(cost < this->m_cost[RL][t][s]) {
                        this->m_cost[RL][t][s] = cost;

                        // Update the order of the splits by concatenating
                        // the two recursive results (left part assumes that
                        // the right part is already contracted - denoted by #,
                        // and now we are inserting the exact order to yield it)
                        this->m_order[RL][t][s] = this->m_order[RL][t - k][s];
                        this->m_order[RL][t][s].replace(this->m_order[RL][t][s].find("'#'"), 3, this->m_order[RL][k][s + t - k]);
                    }
                    //std::cout<<"[Extended window RL: " << t << "]\t" << s << " ... " << s + t - 1 << " - cost: " << this->m_cost[RL][t - k][s] + this->m_cost[RL][k][s + t - k] << " (" << this->m_cost[RL][t - k][s] << ", " << this->m_cost[RL][k][s + t - k] << ")" <<std::endl;
                }
            }
        }
    }

    //std::cout<<"\t"<<this->m_cost[LR][6][0]<<std::endl;
}

/**
 * @brief Solves a given state
 * 
 * @param state The dimensions in this state
 * @return cost_t the best cost for state
 */
template <size_t delta, split_direction_e dir, size_t tt_dim>
cost_t TwoSidedDeltaDim<delta, dir, tt_dim>::solve(const int dim_min, const int dim_max, result_direction_e direction){
    return this->m_cost[direction][dim_max - dim_min][dim_min];
}

template <size_t delta, split_direction_e dir, size_t tt_dim>
void TwoSidedDeltaDim<delta, dir, tt_dim>::init(Network& network){
    // Initialize network
    this->set_limit_dim(network.dimension);
    this->dim = network.dimension;
    this->n_vertex = network.n_vertex;

    // Initialize the memoization tables
    for(int type = 0; type < 2; type++) {
        this->m_cost[type].resize(this->dim + 1);
        this->m_order[type].resize(this->dim + 1);
        for(int t = 0; t <= this->dim; t++) {
            this->m_cost[type][t].resize(this->dim);
            this->m_order[type][t].resize(this->dim);
            for(int s = 0; s < this->dim; s++) {
                this->m_cost[type][t][s] = std::numeric_limits<cost_t>::max();
            }
        }
    }

    // Initialize the result variables
    this->best_cost = std::numeric_limits<cost_t>::max();

    // Initialize the exact solver
    this->m_exact_solver.init(network);
}

template <size_t delta, split_direction_e dir, size_t tt_dim>
cost_t TwoSidedDeltaDim<delta, dir, tt_dim>::call_solve(){
    // Compute the optimal cost of all the possible splits
    this->compute_splits();

    // Include the cost of starting from the left side of TT
    if constexpr(dir & START_LEFT) {
        if(this->m_cost[LR][this->dim][0] < this->best_cost) {
            this->best_cost = this->m_cost[LR][this->dim][0];
            this->best_order_str = this->m_order[LR][this->dim][0];
        }
    }

    // Include the cost of starting from the right side of TT
    if constexpr(dir & START_RIGHT) {
        if(this->m_cost[RL][this->dim][0] < this->best_cost) {
            this->best_cost = this->m_cost[RL][this->dim][0];
            this->best_order_str = this->m_order[RL][this->dim][0];
        }
    }

    // Include the cost of starting from both sides of TT
    if constexpr(dir & ALL) {
        // Iterate over all the possible starting splits
        for(int split = 1; split < dim - 1; split++) {
            // Retrieve the cost of the left part
            cost_t cost_left = this->m_cost[LR][split][0];

            // Retrieve the cost of the right part
            cost_t cost_right = this->m_cost[RL][dim - split][split];

            // Compute the cost of contracting the final two nodes together
            // (results of the two subproblems) 
            cost_t cost_connect = this->m_network->adjacence_matrix[(split - 1) * this->n_vertex + split] *
                                  this->m_network->adjacence_matrix[((split - 1) + this->n_vertex/2) * n_vertex + (split + n_vertex/2)];

            // Calculate the total cost of the split
            cost_t total_cost = cost_left + cost_right + cost_connect;
            //std::cout<<split<<": \t"<<total_cost<<"\t"<<cost_left<<"\t"<<cost_right<<"\t"<<cost_connect<<std::endl;

            // Update the best cost
            if(total_cost < this->best_cost) {
                this->best_cost = total_cost;
                this->best_order_str = "(" + this->m_order[LR][split][0] + ", " + this->m_order[RL][dim - split][split] + ")";
            }
        }
    }

    std::cout<<this->best_order_str<<std::endl;

    return this->best_cost;
}

// Instantiation of the template class
template class TwoSidedDeltaDim<3, ALL, 2>;
template class TwoSidedDeltaDim<4, ALL, 2>;