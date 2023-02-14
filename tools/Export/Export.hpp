#ifndef IMPORT_HPP
#define IMPORT_HPP
#include <fstream>
#include "../../src/Components/Algorithm.hpp"

//default separator used in the csv file
static std::string default_separator=";";

//List of fields to put in the header (in this order)
static std::vector<std::string> field_list = {"Algorithm", "Dmin", "Dmax", /*"Base solution",*/ "Cost", /* "Order",*/ "Time", "Dimension", "Density", "File"};

std::ofstream open_output(const std::string& filename);

bool exists(const std::string& filename);

void generate_header(std::ofstream& file, const std::string separator=default_separator);

void export_entry(std::ofstream& file, const Algorithm& alg, const Network& netw, const std::string separator);

std::string grab_order(const Algorithm& alg, const std::string separator);

#endif