#include "CotengraOptimalWrapper.hpp"

int main(int argc, char* argv[]){
    std::cout << "CONTEGRA MAIN FILE!" << std::endl;
    CotengraOptimalWrapper solver;

    switch (argc){
    case 1:
        cerr << "Missing argument : instance file" << '\n';
        exit(-1);
        break;
    case 2:
        // display(argv[1]);
        solver.filename = argv[1];
        execfile<CotengraOptimalWrapper>(solver, std::string(argv[1]));
        break;
    case 3:
        if(argv[1] == string("d")){
            solver.filename = argv[2];
            execdir<CotengraOptimalWrapper>(solver, std::string(argv[2]));
        }
        break;
    default:
        cerr << "Call error : wrong argument" << '\n';
        exit(-1);
        break;
    }
    return 0;
}