#include "CotengraWrapper.hpp"

int main(int argc, char* argv[]){
    CotengraWrapper<2> solver;

    cerr << "Warning! CotengraWrapper is not intended for standalone use." << std::endl;

    switch (argc){
    case 1:
        cerr << "Missing argument : instance file" << std::endl;
        exit(-1);
        break;
    case 2:
        execfile<CotengraWrapper<2>>(solver, std::string(argv[1]));
        break;
    case 3:
        if(argv[1] == string("d")){
            execdir<CotengraWrapper<2>>(solver, std::string(argv[2]));
        }
        break;
    default:
        cerr << "Call error : wrong argument" << std::endl;
        exit(-1);
        break;
    }
    return 0;
}