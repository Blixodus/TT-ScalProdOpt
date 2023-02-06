#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP
#include "Components.hpp"

/**
 * @brief general algorithm class
 * 
 */
class Algorithm{
    public:
    // Minimum size of network the algorithm can handle, before delegating to sub_alg
    int dmin=0;
    //Max size of sub-networks to be created by the main algorithm
    int dmax=numeric_limits<Dim>::max()-1;
    //(Only useful for some algorithms)

    //Size of the network in number of dimensions
    int dim;
    //taille de l'instance en nombre de sommets
    //int size;
    //Size of the network in number of vertices
    int n_vertex;

    //The current best cost
    Cost best_cost=numeric_limits<Cost>::max()-1;

    //The current best order
    Tab best_order;

    //Final time
    chrono::duration<double> time;

    //Maximum alloted time before timeout
    std::chrono::minutes timeout_time;

    //We can deactivate the algorithm in the execution queue once it times out
    bool still_up = true;

    virtual void init(string file) {};
    virtual Cost call_solve() {return 0;};
    virtual void display_order() {};
};


#endif