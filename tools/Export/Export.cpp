#include "Export.hpp"

/**
 * @brief Opens file to append
 * 
 * @param filename 
 * @return std::FILE* the file in append mode
 */
std::ofstream  open_output(const std::string& filename){
    std::string file_path(filename);
    std::ofstream ofile;

    if(!std::filesystem::path(filename).is_absolute()){
        file_path = "results/" + filename;
    }

    bool exists_var = exists(file_path);

    ofile = std::ofstream(file_path, std::ofstream::app);

    if(!exists_var){
        create_header(ofile);
    }

    return ofile;
}

/**
 * @brief Tests if a file exist
 * 
 * @param filename 
 * @return true if it exists
 * @return false otherwise
 */
bool exists(const std::string& filename){
    std::ifstream ifile(filename);
    if(ifile){
        return true;
    }else{
        return false;
    }
}

/**
 * @brief Exports the field name in an ofstream
 * 
 * @param file the file
 * @param separator the separator to use
 */
void create_header(std::ofstream& file, const std::string separator){
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
void export_entry(std::ofstream& file, const Algorithm& alg, const Network& netw, const std::string separator=default_separator){
    
    if(!file){return;}

    //TODO: grab the algorithm name
    file << alg.algo_name << separator
    << alg.dmin << separator
    << alg.dmax << separator
    //TODO: not implemented yet
    //<< alg.sub_alg << separator
    //<< alg.starting_solution << separator
    << alg.best_cost << separator
    << alg.time.count() << separator
    //TODO: order << separator
    << netw.dimension << separator
    << netw.density << separator
    << netw.m_filename << endl;
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