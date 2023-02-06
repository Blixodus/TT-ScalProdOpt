#ifndef IMPORT_HPP
#define IMPORT_HPP
#include <fstream>
#include "../../src/Components/Components.hpp"
#include "../../src/Components/Algorithm.hpp"
#include "../src/Components/Network.hpp"

//default separator used in the csv file
std::string default_separator=";";

//List of fields to put in the header (in this order)
std::vector<std::string> field_list = {"Algorithm", "Dmin", "Dmax", "Base solution", "Cost", "Order", "Time", "Dimension", "Density", "File"};

void generate_header(ofstream& file, const std::string separator);

void export_entry(ofstream& file, const Algorithm& alg, const Network& netw, const std::string separator);

std::string grab_order(const Algorithm& alg, const std::string separator);

#endif