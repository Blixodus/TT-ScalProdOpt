#include "MatrixSolver.hpp"

int main(int argc, char* argv[]){
    MatrixSolver solver;
    
    switch(argc){
        case 1:
            cerr << "Missing argument : instance file" << '\n';
            exit(-1);
            break;
        case 2:
            // display(argv[1]);
            execfile<MatrixSolver>(solver, argv[1]);
            break;
        case 3:
            if(argv[1] == string("d")){
                execdir<MatrixSolver>(solver, argv[2]);
            }
            break;
        default:
            cerr << "Call error : wrong argument" << '\n';
            exit(-1);
            break;
    }
    return 0;
}