#include "InstanceGenerator.hpp"
#include <vector>

//Code de génération d'instance

void init_instance(int n ,int min, int max, string dir ){
    if(n%2!= 0){
        n--;
    }
    string name = path + dir + ".txt";
        ofstream file(name);
        if(file.is_open()){
            //informations sur l'instances
            //c name
            //c Source
            file << "c FILE: " << name <<endl;
            file << "c SOURCE: initInstances" << endl;
            file << "c min : " << min << endl;
            file << "c max : " << max << endl;

            //représentation du TT
            vector<string> display(3, "v ");
            vector<int> weights(3*(n/2) - 2); //nombre d'arêtes à générer
            //attribution des poids
            for(int i = 0; i < weights.size(); i++){
                weights[i] = min + rand()%(max+1 - min);
            }
        
            for(int i = 0; i < n/2-1; i++){
                display[0].append(to_string(i) + "  *" + to_string(weights[i]) + "  "); //i "  " weights[i]
                display[1].append("*" + to_string(weights[n/2-1+i]) + "     "); //weights[n/2-1 + i]
                display[2].append(to_string(i+n/2) + "  *" + to_string(weights[n-1+i]) + "  "); //i+n/2 "  " wheights[n-1+i]
            }
            display[0].append(to_string(n/2-1));
            display[1].append("*" + to_string(weights[n-2]));
            display[2].append(to_string(n-1));
        
            file << display[0] << '\n';
            file << display[1] << '\n';
            file << display[2] << '\n';

            //taille du TT
            //p carac
            file << "p " << n << endl;

            //arêtes (sommet, sommet, poids)            
            //e edges
            for(int i = 0; i < n/2 - 1; i++){
                file << "e " << i << " " << i+1 << " " << weights[i] << endl;
                file << "e " << i << " " << i+n/2 << " " << weights[n/2-1+i]  << endl;
                file << "e " << i+n/2 << " " << i+n/2+1 << " " << weights[n-1+i] << endl;
            }
            file << "e " << n/2 - 1 << " " << n - 1 << " " << weights[n-2] << endl;
        }else{
            cout << "Could not open outfile : " << name << '\n';
        }
}

/**
 * @brief calls init_instance amount times to generate amount instances, each one having a different number
 * 
 * @param n 
 * @param min 
 * @param max 
 * @param amount 
 * @param dir 
 */
void init_multiple(int n, int min, int max, int amount, string dir){
    for(int i = 1; i <= amount; i++){
        string name = dir + "_" + to_string(i);
        init_instance(n, min, max, name);
        cout << dir << " generated" << '\n';
    }
}


int main(int argc, char* argv[]){
    chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds >(
    chrono::system_clock::now().time_since_epoch()
    );
    srand(ms.count());
    if(argc < 3){
        cout << "s : number of nodes (default 6)" << '\n';
        cout << "d : path (default test/instance)" << '\n';
        cout << "n : number of instances (default 1)" << '\n';
        cout << "m : minimum weight (default 1)" << '\n';
        cout << "M : maximum weight (default 9)" << '\n';
        exit(0);
    }
    for(int i = 1; i < argc - 1; i++){
        switch(*argv[i]){
            case 'm': //poids min
                m = atoi(argv[i+1]);
                i++;
                break;
            case 'M': //poids max
                M = atoi(argv[i+1]);
                i++;
                break;
            case 'n': //nombre d'instances
                amount = atoi(argv[i+1]);
                i++;
                break;
            case 's': //nombre de sommets
                size = atoi(argv[i+1]);
                i++;
                break;
            case 'd': //répertoire
                dir = argv[i+1];
                i++;
                break;
        }
    }
    if(amount > 1){
        init_multiple(max(4, size), max(1, m), M, amount, dir);
    }else{
        init_instance(max(4, size), max(1, m), M, dir);
    }
    cout << "Generation complete" << endl;
}