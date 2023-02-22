#include "Algorithm.hpp"

Algorithm::Algorithm(std::map<std::string, std::any> map){
    //TODO: put proper display of what is happening
    for(const auto &[key, val] : map){
        alg_param_e param_name = param_map[key];
        switch(param_name){
            case MAIN_ALG:
                algo_name = std::any_cast<std::string>(val);
                break;
            case DMIN:
                refdmin = std::any_cast<int>(val);
                break;
            case DMAX:
                refdmax = std::any_cast<int>(val);
                break;
            case SUB_ALG:
                //initialization is handled in Main.cpp
                break;
            case START_SOL:
                //initialization is handled in Main.cpp
                break;
            case TIME:
                timeout_time = std::chrono::minutes(std::any_cast<int>(val));
                break;
            case TEST:
                to_test = std::any_cast<int>(val);//std::any_cast<bool>(val);
                break;
            default:
                std::cout << "Unknown parameter : '" << key << "'" << std::endl;
                break;
        }
    }
}

/**
 * @brief returns best_order as a string
 * 
 * @return std::string 
 */
std::string Algorithm::best_order_as_string() const{
    std::string sep = ", ";
    std::stringstream result;
    std::copy(best_order.begin(), best_order.end(), std::ostream_iterator<int>(result, sep.c_str()));

    return result.str().erase(result.str().size()-sep.size());;
}

/**
 * @brief Compares the computed value with the real value we should get for a given order
 * 
 * @return const int 
 */
const int Algorithm::verify() {
    if(to_test)
    {
        cost_t true_cost = compute_order(*m_network, best_order);

        if(best_cost == true_cost)
        {
            printf("Check passed.\n");
        }
        else
        {
            std::cout << "Found " << best_cost << " while true value is " << true_cost << " for order (" << best_order_as_string() << ")" << std::endl;
            // printf("Found %d while true value is %d for order (%s)\n", best_cost, true_cost, best_order_as_string().data());
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Limits dmin and dmax
 * dmax is set to refdmax, or to a strictly positive value that is also smaller or equal to max 
 * dmin is set to refdmin, or to a positive value smaller or equal to dmax
 * 
 * @param limit the limiting factor in the network, for the execution of the algorithm
 */
void Algorithm::set_limit_dim(int limit){
    dmax = (refdmax <= 0) ? limit : std::min(std::max(1, refdmax), limit);
    dmin = std::min(std::max(1, refdmin), dmax);
}

/**
 * @brief Initializes the starting solution
 * 
 * @param network 
 */
void Algorithm::set_start_sol(Network& network){
    if(start_sol != nullptr){
        execfile_no_display(*start_sol, network);
        best_cost = start_sol->best_cost;
        best_order = start_sol->best_order;
        time += start_sol->time;
    }
}