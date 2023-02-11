#include "Shuffle.hpp"

int main(int argc, char* argv[]){
    Shuffle solver;
    const char* path;
    bool sf = true;

    if(argc == 1){
        cerr << "Missing argument : instance file" << '\n';
        exit(-1);
    }else{
        for(int i = 1; i < argc; i++){
            if(argv[i] == string("d")){
                if(i == argc-1){
                    cerr << "Missing argument : directory" << '\n';
                    exit(-1);
                }
                path = argv[i+1];
                sf = false;
                i++;
            }else if(argv[i] == string("r")){
                if(i == argc-1){
                    cerr << "Missing argument : range" << '\n';
                    exit(-1);
                }
                solver.refdelta = atoi(argv[i+1]);
                i++;
            }else{
                path = argv[i];
            }
        }   
    }
    if(sf){
        display(path);
        execfile<Shuffle>(solver, path);
    }else{
        execdir<Shuffle>(solver, path);
    }
    return 0;
}