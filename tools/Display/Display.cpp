#include "Display.hpp"
using namespace std;

//Code d'affichage des instances

void display(string file){
    cout << "---------------" << '\n';
    string path = "../instances/" + file;

    string filename(path);
    string line;

    string red("\033[0;31m");
    string blue("\033[0;34m");
    string reset("\033[0m");
    
    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        exit(-1);
    }

    char mot[200];
    int n = 0;
    int nL = 0;
    while(getline(input_file, line) && line[0] != 'e'){
        if(line.size() > 2){
            istringstream flux(&line[2]);
            switch(line[0]){
                case 'v':
                   while(flux >> mot){
                        if(mot[0] == '*'){
                            char* padding;
                            if(nL == 1){
                                padding = "       ";
                            }else{
                                padding = "   ";
                            }
                            cout << red << &mot[1] << blue << "(" << n << ")" << padding;
                            n++;
                        }else{
                            cout << reset << mot << "   ";
                        }
                    }
                    nL++;
                break;
                case 'p':
                    cout << "Sommets : " << &line[2];
                break;
                default:
                    cout << &line[2];
                break;
            }
            cout << '\n';
        }
    }
}

void display_dir(string dir){
    string base = "../instances/" + dir + "/";
    DIR* dp = NULL;
    struct dirent *file = NULL;
    dp = opendir(base.c_str());

    if(dp == NULL){
        cerr << "Could not open the directory : " << base << '\n';
        exit(-1);
    }
    
    file = readdir(dp);

    while(file != NULL){
        if(file->d_name[0] != '.'){
            string path = base + file->d_name;
            display(path);
        }
        file = readdir(dp);
    }
    closedir(dp);
}