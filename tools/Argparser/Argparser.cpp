#include "Argparser.hpp"

Argparser::Argparser(int argc, char** argv){
    arg_count = argc;
    for(int i = 1; i < argc; i++)
        arg_list.push_back(std::string(argv[i]));
}

std::vector<std::vector<std::pair<std::string, std::string>>> Argparser::grab_algo_entries(){

    std::smatch res;
    
    //We make a copy the list storing the dictionaries 
    std::string arg=this->arg_list[0];

    //A list of dictionaries
    std::vector<std::string> dict_list;

    //matches patterns of format {...}
    std::regex entry_regex("[^}{]{2,}");

    //first parsing, to gather the dictionaries (the entries)
    while(std::regex_search(arg, res, entry_regex)){
        dict_list.push_back(res[0]);
        arg = res.suffix();
    }

    //matches patterns of format 'key': val
    std::regex pair_regex("'[^,']+': [^,}]+");
    
    std::vector<std::vector<std::pair<std::string, std::string>>> entries;

    std::string param_delim = ", ";
    std::string pair_delim = ": ";

    //We iterate over the dictionaries
    for(std::string dict : dict_list){
        //custom structure to handle splitting strings properly
        Splitter dict_splitter(dict, param_delim);
        
        //A vector to store the current dictionary
        std::vector<std::pair<std::string, std::string>> current_entry;

        do{
            Splitter pair_splitter(dict_splitter.lhs, pair_delim);

            //We clean the "'"
            std::string key = remove_limits(pair_splitter.lhs);

            current_entry.push_back(std::make_pair(key, pair_splitter.rhs));
        }while(dict_splitter.next_split(param_delim).delim_pos != std::string::npos);
        
        //I could get rid of that bit, if I knew how to code.
        Splitter pair_splitter(dict_splitter.lhs, pair_delim);

        std::string key = remove_limits(pair_splitter.lhs);

        current_entry.push_back(std::make_pair(key, pair_splitter.rhs));
        entries.push_back(current_entry);
    }

    return entries;
}

std::vector<std::tuple<std::string, int, float>> Argparser::grab_network_entries(){

    //first parsing using regex
    std::smatch res;
    //isolates the tuples
    std::regex entry_regex("[^)(]{3,}");
    std::string args1 = this->arg_list[1];
    std::vector<std::string> tuple_list;
    
    while(std::regex_search(args1, res, entry_regex)){
        tuple_list.push_back(res[0]);
        args1 = res.suffix();
    }
    
    //delimiter
    std::string param_delim = ", ";
    std::vector<std::tuple<std::string, int, float>> network_entries;
    
    //iterates over the tuples
    for(std::string& t : tuple_list){
        //grab the 3 members
        Splitter entry_splitter(t, param_delim);
        
        //We remove the "'" characters
        std::string filename = entry_splitter.lhs.substr(1, entry_splitter.lhs.length()-2);
        entry_splitter.next_split(param_delim);
        
        //store the value after converting the strings to the proper type
        network_entries.push_back(std::make_tuple(filename, std::stoi(entry_splitter.lhs), std::stof(entry_splitter.rhs)));
    }

    return network_entries;
}

int main(int argc, char** argv){
    Argparser parser(argc, argv);

    auto list = parser.grab_algo_entries();

    for(auto& dict : list){
        for(auto& pair : dict){
            std::cout << pair.first << " " << pair.second << std::endl;
        }
        std::cout << "--------" << std::endl;
    }
    
    auto networks = parser.grab_network_entries();
    for(auto& tuple : networks){
        std::cout << std::get<0>(tuple) << " " << std::get<1>(tuple) << " " << std::get<2>(tuple) << std::endl;
    }
}