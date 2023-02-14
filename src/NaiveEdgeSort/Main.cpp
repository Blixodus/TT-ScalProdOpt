#include "NaiveEdgeSort.hpp"

int main(int argc, char* argv[]){
    NaiveEdgeSort solver;
    switch(argc){
        case 1:
            cerr << "Missing argument : instance file" << endl;
            exit(-1);
            break;
        case 2:
            //display(argv[1]);
            execfile<NaiveEdgeSort>(solver, std::string(argv[1]));
            break;
        case 3:
            if(argv[1] == string("d")){
                execdir<NaiveEdgeSort>(solver, std::string(argv[2]));
            }
            break;
        default:
            cerr << "Call error : wrong argument" << endl;
            exit(-1);
            break;
    }
    return 0;
}