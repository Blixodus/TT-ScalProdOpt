#include "Compute.hpp"

int main(int argc, char* argv[]){
    std::string calc_file;

    calc_file = (argc<2 ? "test.txt" : argv[1]);

    import_calc_file(calc_file);
}