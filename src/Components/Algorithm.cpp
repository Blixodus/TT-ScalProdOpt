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
 * @brief Compares the computed value with the real value we should get for a given order
 * 
 * @return const int 
 */
const int Algorithm::verify() {
    // DEPRECATED - We have changed the method of storing and veryfying solution
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