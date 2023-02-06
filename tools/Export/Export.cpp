#include "Export.hpp"


/**
 * @brief Exports the field name in an ofstream
 * 
 * @param file the file
 * @param separator the separator to use
 */
void generate_header(ofstream& file, const std::string separator=default_separator){
    for(std::string field : field_list){
        file << field << separator;
    }
    file << endl;
}

/**
 * @brief Exports the info of an algorithm,
 * as well as the results and the associated network info
 * 
 * @param file the csv to export into
 * @param alg the algorithm object used
 * @param netw the network
 * @param separator the separator to use
 */
void export_entry(ofstream& file, const Algorithm& alg, const Network& netw, const std::string separator=default_separator){
    
    //TODO: grab the algorithm name
    file << main_algorithm << separator
    << alg.dmin << separator
    << alg.dmax << separator
    << alg.starting_solution << separator
    << alg.time.count() << separator
    << alg.best_cost << separator
    //TODO: order << separator
    << netw.dimension << separator
    << netw.density << separator
    << netw.filename << endl;
}

/**
 * @brief Grabs best_order and returns it in the form of a string
 * format is [edge0;edge1;...;edgen_edge;]
 * Here ";" is the default separator
 * 
 * @param alg an algorithm
 * @param separator the separator to use
 * @return std::string 
 */
std::string grab_order(Algorithm& alg, const std::string separator=default_separator){
    //Selecting a list separator different from the csv separator
    std::string list_separator = (separator==std::string(";") ? "," : ";");

    //converting and storing the order into a string
    std::string string_order="[";
    for(auto& edge : alg.best_order){
        string_order += to_string(edge) + list_separator; 
    }

    string_order+="]";
    return string_order;
}