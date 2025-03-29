/**
 * @brief Defines the Splitter class
 * A class that can be used to more easily parse a string given a delimiter 
 * 
 */
#ifndef SPLITTER_HPP
#define SPLITTER_HPP
#include <string>
#include <vector>

struct Splitter{
    std::string lhs;
    
    std::string rhs;
    
    size_t delim_pos;
    
    Splitter(const std::string s, const std::string& sep);
    Splitter(const std::string s, const std::string&& sep);
    Splitter(const std::string s);
    
    Splitter& next_split(const std::string& sep);
    Splitter& next_split(const std::string&& sep);

    std::string next_token(const std::string& sep);
    std::vector<std::string> next_nth_token(const std::string& sep, int n_token, bool FILL=false);
    std::vector<std::string> all_token(const std::string& sep);
    std::pair<std::string, std::string> next_pair(const std::string& sep);

    void set_members(const std::string& s, size_t sep_size);
    
    /**
     * @return true if the splitter reached the end of the string
     */
    bool empty(){return delim_pos == std::string::npos;}
    //std::string tokens_as_string(const std::string& s, const std::string& sep, int token_amount);
    
    //std::vector<std::string> tokens_as_list(const std::string& s, const std::string& sep, int token_amount);

    std::pair<std::string, std::string> get_members();
};

std::string remove_limits(const std::string& s);

#endif