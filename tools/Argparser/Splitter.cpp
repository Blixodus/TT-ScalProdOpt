#include "Splitter.hpp"

/**
 * @brief Construct a new Splitter:: Splitter object
 * 
 * @param s a string
 * @param sep a delimiter
 */
Splitter::Splitter(const std::string& s, const std::string& sep){
    delim_pos = s.find(sep);
    set_members(s, sep.size());
}

/**
 * @brief finds the next split given a delimiter
 * 
 * @param sep a delimiter
 * @return Splitter& a reference to the splitter
 */
Splitter& Splitter::next_split(const std::string& sep){
    delim_pos = rhs.find(sep);
    set_members(rhs, sep.size());
    return *this;
}

/**
 * @brief sets lhs and rhs based on delim_pos
 * 
 * @param s a string
 * @param sep_size the size of the separator
 */
void Splitter::set_members(const std::string& s, size_t sep_size){
    if(delim_pos != std::string::npos){
        lhs = s.substr(0, delim_pos);
        rhs = s.substr(delim_pos+sep_size, s.size());
    }else{
        //the delimiter has not been found
        lhs = s;
        rhs.clear();
    }
}

/**
 * @brief returns the right hand side and left hand side as a pair of strings
 * 
 * @return std::pair<std::string, std::string> 
 */
std::pair<std::string, std::string> Splitter::get_members (){
    return std::make_pair(lhs, rhs);
}

std::string remove_limits(const std::string& s){
    std::string news = s.substr(1, s.length()-2);
    return news;
}