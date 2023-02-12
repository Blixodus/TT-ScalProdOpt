#include "Splitter.hpp"

/**
 * @brief Construct a new Splitter:: Splitter object
 * 
 * @param s a string
 * @param sep a delimiter
 */
Splitter::Splitter(const std::string s, const std::string& sep){
    delim_pos = s.find(sep);
    set_members(s, sep.size());
}

/**
 * @brief Construct a new Splitter:: Splitter object
 * 
 * @param s a string
 * @param sep a delimiter
 */
Splitter::Splitter(const std::string s, const std::string&& sep){
    delim_pos = s.find(sep);
    set_members(s, sep.size());
}

/**
 * @brief Construct a new Splitter:: Splitter object
 *  with no left hand side
 * @param s 
 */
Splitter::Splitter(const std::string s){
    delim_pos = 0;
    set_members(s, 0);
}

/**
 * @brief finds the next split given a delimiter
 * rhs is empty if the delimiter was not found
 * advances the splitter
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
 * @brief finds the next split given a delimiter
 * rhs is empty if the delimiter was not found
 * advances the splitter
 * 
 * @param sep a delimiter
 * @return Splitter& a reference to the splitter
 */
Splitter& Splitter::next_split(const std::string&& sep){
    delim_pos = rhs.find(sep);
    set_members(rhs, sep.size());
    return *this;
}

/**
 * @brief Returns the next token delimited by sep
 * Advances the splitter
 * 
 * @param sep 
 * @return std::string 
 */
std::string Splitter::next_token(const std::string& sep){
    return next_split(sep).lhs;
}

/**
 * @brief Returns a list containing at most nth token delimited by sep
 * if FILL=true, the resulting vector will contain empty strings in the event where
 * n_token is greater than the number of token in the string
 * 
 * @param sep a delimiter
 * @param n_token the amount of token to gather
 * @param FILL whether or not to complete the resulting vector with empty strings
 * @return std::vector<std::string> 
 */
std::vector<std::string> Splitter::next_nth_token(const std::string& sep, int n_token, bool FILL){
    std::vector<std::string> string_list;
    while(n_token > 0 && !empty()){
        string_list.push_back(next_token(sep));
        n_token--;
    }

    if(FILL){
        while(n_token > 0){
            string_list.push_back("");
            n_token--;
        }
    }

    return string_list;
}

/**
 * @brief Gathers all token delimited by sep
 * 
 * @param sep a delimiter
 * @return std::vector<std::string> 
 */
std::vector<std::string> Splitter::all_token(const std::string& sep){
    std::vector<std::string> string_list;
    while(!empty()){
        string_list.push_back(next_token(sep));
    }
    return string_list;
}

/**
 * @brief grabs the next 2 tokens and forms a pair
 * 
 * @param sep a delimiter
 * @return std::pair<std::string, std::string> 
 */
std::pair<std::string, std::string> Splitter::next_pair(const std::string& sep){
    auto v = next_nth_token(sep, 2, true);
    return std::make_pair(v[0], v[1]);
}

/**
 * @brief sets lhs and rhs based on delim_pos
 * rhs is empty if the delimiter was not found
 * 
 * @param s a string
 * @param sep_size the size of the separator
 */
void Splitter::set_members(const std::string& s, size_t sep_size){
    if(!empty()){
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