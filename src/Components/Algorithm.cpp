#include "Algorithm.hpp"

Algorithm::Algorithm(std::map<std::string, std::any> map){
    //TODO: put proper display of what is happening
    for(const auto &[key, val] : map){
        alg_param_e param_name = param_map[key];
        switch(param_name){
            case MAIN_ALG:
                std::cout << "Instantiating : " << std::any_cast<std::string>(val) << std::endl;
                algo_name = std::any_cast<std::string>(val);
                break;
            case DMIN:
                refdmin = std::any_cast<int>(val);
                break;
            case DMAX:
                refdmax = std::any_cast<int>(val);
                break;
            case SUB_ALG:
                std::cout << "Sub alg not implemented yet" << std::endl;
                break;
            case START_SOL:
                std::cout << "Starting solution not implemented yet" << std::endl;
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
            printf("Found %d while true value is %d for order (%s)\n", best_cost, true_cost, best_order_as_string().data());
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
    dmax = std::min(std::max(1, refdmax), limit);
    dmin = std::min(std::max(1, refdmin), dmax);
}