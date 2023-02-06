#ifndef SPLITTER_HPP
#define SPLITTER_HPP
#include <string>

struct Splitter{
    std::string lhs;
    
    std::string rhs;
    
    size_t delim_pos;
    
    Splitter(const std::string& s, const std::string& sep);
    
    Splitter& next_split(const std::string& sep);
    
    void set_members(const std::string& s, size_t sep_size);
    
    std::pair<std::string, std::string> get_members();
};

std::string remove_limits(const std::string& s);

#endif