#include "InstanceGenerator.hpp"
#include <vector>

//Problem generation
//remark : the base path for generated files is ../instances/

/**
 * @brief generates a single instance with specific properties :
 * -The central edges are of constant weight N
 * -The outer edges' weight grows toward the center of the network
 * 
 * @param dim the dimension of the network
 * @param N the weight of the central edges  
 * @param Rmin the minimum weight of the outer edges
 * @param Rmax the maximum weight of the outer edges
 * @param Rnoise the random variation in the weight of the outer edges
 * @param dir the directory the network will be created in
 */
void init_gaussian(int dim, int N, int Rmin, int Rmax, float Rnoise, string dir){
    //TODO:

    int nodes = dim*2;

    string name = path + dir + ".txt";
    ofstream file(name);
    if(file.is_open()){
            //informations about the networks
            //c name
            //c Source
            file << "c FILE: " << name << endl;
            file << "c SOURCE: init_gaussian" << endl;
            file << "c N : " << N << endl;
            file << "c Rmin : " << Rmin << endl;
            file << "c Rmax : " << Rmax << endl;
            file << "c Rnoise : " << Rnoise << endl;

            //TODO: the actual generation
    }

}

/**
 * @brief generates a single instance
 * 
 * @param dim the dimension of the network
 * @param min the minimum weight of an edge
 * @param max the maximum weight of an edge
 * @param dir the directory the network will be created in
 */
void init_instance(int dim ,int min, int max, string dir ){

    int n_nodes = dim*2;

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
            vector<int> weights(3*dim - 2); //nombre d'arêtes à générer
            //attribution des poids
            for(int i = 0; i < weights.size(); i++){
                weights[i] = min + rand()%(max+1 - min);
            }
        
            for(int i = 0; i < dim-1; i++){
                display[0].append(to_string(i) + "  *" + to_string(weights[i]) + "  "); //i "  " weights[i]
                display[1].append("*" + to_string(weights[dim-1+i]) + "     "); //weights[n/2-1 + i]
                display[2].append(to_string(i+dim) + "  *" + to_string(weights[n_nodes-1+i]) + "  "); //i+n/2 "  " wheights[n-1+i]
            }
            display[0].append(to_string(dim-1));
            display[1].append("*" + to_string(weights[n_nodes-2]));
            display[2].append(to_string(n_nodes-1));
        
            file << display[0] << '\n';
            file << display[1] << '\n';
            file << display[2] << '\n';

            //taille du TT
            //p carac
            file << "d " << dim << endl;

            //arêtes (sommet, sommet, poids)            
            //e edges
            for(int i = 0; i < dim - 1; i++){
                file << "e " << i << " " << i+1 << " " << weights[i] << endl;
                file << "e " << i << " " << i+dim << " " << weights[dim-1+i]  << endl;
                file << "e " << i+dim << " " << i+dim+1 << " " << weights[n_nodes-1+i] << endl;
            }
            file << "e " << dim - 1 << " " << n_nodes - 1 << " " << weights[n_nodes-2] << endl;
        }else{
            cout << "Could not open outfile : " << name << '\n';
        }
}

/**
 * @brief calls init_instance amount times to generate amount networks
 * Each network is numbered
 * 
 * @param dim the dimension of the networks
 * @param min the minimum weight of an edge
 * @param max the maximum weight of an edge
 * @param amount the amount of network to generate
 * @param dir the directory the networks will be create in
 */
void init_multiple(int dim, int min, int max, int amount, string dir){
    for(int i = 1; i <= amount; i++){
        string name = dir + "_" + to_string(i);
        init_instance(dim, min, max, name);
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
                dim = atoi(argv[i+1]);
                i++;
                break;
            case 'd': //répertoire
                dir = argv[i+1];
                i++;
                break;
        }
    }
    if(amount > 1){
        init_multiple(max(2, dim), max(1, m), M, amount, dir);
    }else{
        init_instance(max(2, dim), max(1, m), M, dir);
    }
    cout << "Generation complete" << endl;
}