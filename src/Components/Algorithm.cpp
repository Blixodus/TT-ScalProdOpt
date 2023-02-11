#include "Algorithm.hpp"

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
            printf("Found %d while true value is %d for order (%s)\n", best_cost, true_cost, best_order_as_string());
            return 1;
        }
    }
    return 0;
}
