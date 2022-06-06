#include "TriScore.hpp"

int main(int argc, char* argv[]){
    TriScore solver;
    switch(argc){
        case 1:
            cout << "Missing argument : instance file" << endl;
            return -1;
            break;
        case 2:
            display(argv[1]);
            solver.execfile(argv[1]);
            break;
        case 3:
            if(*argv[1] == 'd'){
                solver.execdir(argv[2]);
            }
            break;
        default:
            cout << "Call error : wrong argument" << endl;
            return -1;
            break;
    }
    return 0;
}