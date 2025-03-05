#include "Algorithm.hpp"

Algorithm::Algorithm(std::map<std::string, std::any> map){
    //TODO: put proper display of what is happening
    for(const auto &[key, val] : map){
        alg_param_e param_name = param_map[key];
        switch(param_name){
            case MAIN_ALG:
                algo_name = std::any_cast<std::string>(val);
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
            default:
                std::cout << "Unknown parameter : '" << key << "'" << std::endl;
                break;
        }
    }
}