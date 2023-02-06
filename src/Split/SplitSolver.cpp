#include "SplitSolver.hpp"

/**
 * @brief Solves a given state
 * 
 * @param S The tensors in this state
 * @return int the best cost for S
 */
Cost Split::solve(Tab S){
    //encodage de l'ensemble de sommets
    unsigned long long key = convert(S);

    //si il reste plus d'1 sommet et que le coût n'a pas encore été calculé
    if(C.find(key) == C.end() && S.size() > 1){
        C[key] = best_cost+1;
        Cost cost;
        
        Tab S1;
        Tab S2;

        Cost cout_sortant = produit_sortant(S, computeA(S));
        
        //pour toutes les séparations de S
        do{
            //on déplace la "barre" où on fait la coupure S1/S2 dans S
            for(int i = 0; i < S.size() - 1; i++){
                S1.clear();
                S2.clear();
                //on attribue les sommets de S1
                for(int k = 0; k <= i; k++){
                    S1.push_back(S[k]);
                }
                //on attribue les sommets de S2
                for(int k = i+1; k < S.size(); k++){
                    S2.push_back(S[k]);
                }
                cost = solve(S1);
                if(!S2.empty()){ 
                    cost += solve(S2) + cout_sortant*cut(S1, S2);
                }
                if(cost < C[key] && cost > 0){
                    C[key] = cost;
                    //P1[key] = convert(S1);
                    //P2[key] = convert(S2);
                }
            }
        }while(next_permutation(S.begin(), S.end()));
    }else if(S.size() == 1){
        C[key] = 0;
    }
    return C[key];
}

/**
 * @brief computes the S.size()-1'th column of A
 * 
 * @param S The tensors in this state
 * @return Tab an updated copy of A
 */
Tab Split::computeA(Tab S){
    for(int i : S){
        A[n_vertex*(S.size()-1)+i] = G[n_vertex*n_vertex + i];
        for(int k : S){
            A[n_vertex*(S.size()-1)+i] /= G[n_vertex*i + k];
        }   
    }
    return A;
}

/**
 * @brief computes the product of all edges linking 2 states together
 * 
 * @param S1 A state
 * @param S2 A state
 * @return int 
 */
Cost Split::cut(Tab S1, Tab S2){
    Cost res = 1;
    for(int i : S1){
        for(int j : S2){
            res *= G[n_vertex*i + j];
        }
    }
    return res;
}

/**
 * @brief computes the product of all the edges leaving a state
 * 
 * @param S The tensors in this state
 * @param A The external-weights of all tensors for each states
 * @return int 
 */
Cost Split::produit_sortant(Tab S, Tab A){
    Cost res = 1;
    for(int i : S){
        res *= A[n_vertex*(S.size()-1) + i];
    }
    return res;
}

/**
 * @brief converts a state in a unique integer key
 * 
 * @param S The tensors in this state
 * @return int 
 */
unsigned long long Split::convert(Tab S){
    unsigned long long res = 0;
    for(int i : S){
        res += pow(2, i);
    }
    return res;
}

/**
 * @brief converts a key into a set of tensors
 * 
 * @param key a code generated from a state using convert(S)
 * @return Tab 
 */
Tab Split::recover(unsigned long long key){
    Tab res;
    for(int i = n_vertex; i >= 0; i--){
        int p = pow(2, i);
        if(key >= p){
            res.push_back(i);
            key -= p;
        }
    }
    return res;
}

void Split::display_order(Tab S){//dégueulasse
    if(S.size() > 1){
        int key = convert(S);
        display_order(recover(P1[key]));
        display_order(recover(P2[key]));
        cout << "| ";
        for(int i : S){
            cout << i << " | ";
        }
        cout << endl;
    }
}

/**
 * @brief dummy methods to use in template
 * 
 */
void Split::display_order(){}

void Split::init(string file){
    S.clear();
    G.clear();
    A.clear();
    C.clear();
    //P1.clear();
    //P2.clear();
    best_cost = numeric_limits<Cost>::max() - 1;

    ifstream ifile(file);
    string line;
    int i, j, w;
    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
            case 'p':
                n_vertex = atoi(&line[2]);
                
                S.resize(n_vertex);
                G.resize(n_vertex*(n_vertex+1), 1);
                A.resize(n_vertex*n_vertex, 1);
                //C.resize(pow(2, size), -1);
                //P1.resize(pow(2, size), -1);
                //P2.resize(pow(2,size), -1);
            break;
            case 'e':
                flux >> i >> j >> w;
                G[n_vertex*i + j] = w;
                G[n_vertex*j + i] = w;
                G[n_vertex*n_vertex + i] *= w;
                G[n_vertex*n_vertex + j] *= w;
            break;
            default:
            break;
        }
    }

    for(int k = 0; k < n_vertex; k ++){
        S[k] = k;
    }
}

Cost Split::call_solve(){
    return solve(S);
}