#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>

struct NetworkDisplay{
    //TODO:

    std::string filename;

    NetworkDisplay(std::string file);
};

void display(std::string file);

void display_dir(std::string file);

#endif