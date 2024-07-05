#include "Argparser.hpp"

Argparser::Argparser(int argc, char** argv){
    arg_count = argc;
    
    for(int i = 1; i < argc; i++)
        arg_list.push_back(std::string(argv[i]));

    sort_entries();
    grab_algorithms();
}

/**
 * @brief parses the program arguments and gathers the lists in separate vectors
 */
void Argparser::sort_entries(){
    state_e state = NONE;
    for(std::string arg : arg_list){
        if(!update_state(state, arg)){
            switch(state){
                case NONE:
                break;
                case ALGOS:
                    alg_entries_list.push_back(arg);
                break;
                case INSTANCES:
                    file_entries_list.push_back(Network(arg));
                break;
                case OUTPUT:
                    if(!output_file.empty()){
                        std::cerr << "[Error] Redefinition of output file" << std::endl;
                        break;
                    }else{
                        output_file = arg;
                    }
                break;
                default:
                break;
            }
        }
    }
}

/**
 * @brief update the state of the parser based on the flag encountered
 * 
 * @param state the state (last flag encountered) of the parser
 * @param token the current token
 * @return true if a flag was encountered
 * @return false otherwise
 */
bool update_state(state_e& state, std::string token){
    if(token == "-a"){
        state = ALGOS;
        return true;
    }else if(token == "-f"){
        state = INSTANCES;
        return true;
    }else if(token == "-o"){
        state = OUTPUT;
        return true;
    }else{
        return false;
    }
}

/**
 * @brief grabs the algorithm entries and place them in a list of dictionary
 * 
 */
void Argparser::grab_algorithms(){
    std::string sep(" ");
    //we iterate over every dictionnary
    for(std::string param_list : alg_entries_list){
        //we create a vector to store the current dictionary
        std::vector<std::pair<std::string, std::string>> entry_params;

        //we initiate a splitter
        Splitter split(param_list);
        do{
            //we grab 2 tokens
            entry_params.push_back(split.next_pair(sep));
        }while(!split.empty());

        //we store the vector
        alg_list.push_back(entry_params);
    }
}

/**
 * @brief returns a list of dictionary containing all the pairs [param_name, value]
 * with the values cast to the proper type. 
 * Values can be retrieved using std::any_cast<T>(map[param_name]). 
 * The map can be switch-case'd on using param_type_map defined in Argparser.hpp
 * 
 * @return std::vector<std::map<std::string, std::any>> 
 */
std::vector<std::map<std::string, std::any>> Argparser::grab_dictionary_list(){
    std::vector<std::map<std::string, std::any>> vector_maps;
    for(auto pair_list : alg_list){
        std::map<std::string, std::any> dictionary;

        for(auto& [k, v] : pair_list){
            cast_proper(dictionary, k, v);
        }
        vector_maps.push_back(dictionary);
    }
    return vector_maps;
}

/**
 * @brief converts a string into the proper type to store in a map, based on the key
 * the map used to switch on the values is param_type_map from Argparser.hpp
 * same goes for the enum type
 * 
 * @param map the map to put the pair into
 * @param key 
 * @param val 
 */
void cast_proper(std::map<std::string, std::any>& map, std::string key, std::string val){
    switch(param_type_map[key]){
        case DMIN:
        case DMAX:
        case TIME:
        case TEST:
            map[key] = std::stoi(val);
            break;
        case MAIN_ALG:
        case SUB_ALG:
        case START_SOL:
            map[key] = val;
            break;
        default:
            std::cerr << "[Warning] Unrecognised key : '" << key << "'" << std::endl;
            break;
    }
}

void display(std::vector<std::string> vs){
    for(std::string s : vs){
        std::cout << s << std::endl;
    }
    std::cout << "------" << std::endl;
}