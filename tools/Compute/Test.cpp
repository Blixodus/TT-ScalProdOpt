#include "Test.hpp"

//Code de contraction d'un ordre sur une instance

/**
 * @brief Import the file containing the tests
 * 
 * @param test_file 
 */
void import_test(string test_file){
    path_test = "../calc/" + test_file;
    ifstream ifile(path_test);
    if(ifile.fail()){
        cout << "Could not open test file : " << test_file << '\n';
        exit(1);
    }
    string line;
    int i;
    while(getline(ifile, line)){
        switch(line[0]){
            case 'f':
                path_instance = "../instances/";
                path_instance.append(&line[2]);
                import_instance(path_instance);
                break;
            case 'o':
                O.clear();
                istringstream order(&line[2]);
                while(order >> i){
                    O.push_back(i);
                }
                cout << &line[2] << " : " << follow_order(O) << '\n';

                break;
        }        
    }
}

/**
 * @brief Import a network
 * 
 * @param file , file containing a network
 */
void import_instance(string file){
    G.clear();
    E.clear();

    ifstream ifile(file);
    if(ifile.fail()){
        cout << "Could not open instance file : " << file << endl;
        exit(1);
    }else{
        cout << file << '\n';
    }
    string line;
    int i, j, w;
    while(getline(ifile, line)){
        istringstream l(&line[2]);
        if(line[0] == 'v'){

            string red("\033[0;31m");
            string reset("\033[0m");
            for(auto& c : line){
                switch(c){
                    case '*':
                        cout << red;
                        break;
                    case ' ':
                        cout << reset << c;
                        break;
                    case '\n':
                        cout << reset << c;
                        break;
                    case '\0':
                        cout << reset << c;
                        break;
                    case 'v':
                        cout << reset;
                        break;
                    default:
                        cout << c;
                        break;
                }
            }
            cout << '\n';
        }
        if(line[0] == 'p'){
            //istringstream taille(&line [2]);
            l >> ::size;

            G.resize(::size*::size, 1);
            for(int i = 0; i < ::size; i++){
                G[::size*i + i] = 0;
            }
        }
        if(line[0] == 'e'){
            //istringstream edge(&line[2]);
            l >> i >> j >> w;

            G[::size*i + j] = w;
            G[::size*j + i] = w;
            E.push_back(make_pair(i, j));
        }
    }
    sort_edges(E);
}

void Graph::init(){
    V.resize(::size, -1);
    G = ::G;
}

/**
 * @brief Computes a given order
 * 
 * @param O 
 * @return Cost 
 */
Cost follow_order(Tab O){
    Graph graph;
    graph.init();
    Cost cost = 0;
    if(E.size() > 3*::size/2 - 2){
        cout << "[WARNING] Too many edges given, skipping order" << '\n';
    }else{
        for(int i : O){
            cost += graph.contract(i);
        }
    }
    return cost;
}

/**
 * @brief Iterates over the nodes to find the relevant one
 * When the edges contracts, the nodes merge, we store a link to the relevant node in a vector
 * Any non-relevant nodes will then direct to a relevant one
 * 
 * @param i 
 * @return int 
 */
int Graph::C(int i){
    while(V[i] != -1){
        i = V[i];
    }
    return i;
}

/**
 * @brief Contracts an edge and return the contraction cost
 * 
 * @param i 
 * @return Cost 
 */
Cost Graph::contract(int i){
    int a = C(E[i].first);
    int b = C(E[i].second);

    if(a != b){
        int res = G[::size*a + b];
        for(int j = 0; j < ::size; j++){
            if(a != j){
                res *= max(1, G[::size*b + j]);
            }
            if(b != j){ 
                res *= max(1, G[::size*a + j]);
            }
        }

        for(int j = 0; j < ::size; j++){
            G[::size*a + j] *= G[::size*b + j];
            G[::size*b + j] = 0;
            G[::size*j + b] = 0;
            G[::size*j + a] = G[::size*a + j];
        }
        V[b] = a;
        return res;
    }else{
        return 0;
    }
}

int main(int argc, char* argv[]){
    if(argc < 2){
        import_test("test.txt");
    }else{
        import_test(argv[1]);
    }
}