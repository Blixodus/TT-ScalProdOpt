#include "SideLimSolver.hpp"

int main(int argc, char* argv[]){
    SLim solver;
    const char* path;
    bool sf = true;

    if(argc == 1){
        cerr << "Missing argument : instance file" << endl;
    }else{
        for(int i = 1; i < argc; i++){
            if(argv[i] == string("d")){
                if(i == argc-1){
                    cerr << "Missing argument : directory" << '\n';
                }
                path = argv[i+1];
                sf = false;
                i++;
            }else if(argv[i] == string("r")){
                if(i == argc-1){
                    cerr << "Missing argument : range" << '\n';
                }
                solver.delta = atoi(argv[i+1]);
                i++;
            }else{
                path = argv[i];
            }
        }   
    }
    if(sf){
        display(path);
        solver.execfile(path);
    }else{
        solver.execdir(path);
    }
    return 0;
}