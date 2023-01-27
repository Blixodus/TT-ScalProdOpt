#include "VerticalSplitSolver.hpp"

/**
 * @brief Solves a given state
 * 
 * @param S The dimensions in this state
 * @return int the best cost for S
 */
Cost VSplit::solve(Tab S){
    //encodage de l'ensemble de sommets
    unsigned long long key = convert(S);

    if(C.find(key) == C.end() && S.size() > 1){
        C[key] = bestCost+1;

        Cost cost;
        
        Tab S1;
        Tab S2;

        Cost cout_sortant = produit_sortant(S, computeA(S));
    
        //on parcours tous les découpages de dimension DELTA et moins
        for(int i = 0; i < min(delta, (int) S.size()); i++){
            S1.clear();
            S2.clear();
            //on attribue les sommets de S1
            for(int k = 0; k <= i; k++){
                S1.push_back(S[k]);
                S1.push_back(S[k] + size/2);
            }
            //on attribue les sommets de S2
            for(int k = i+1; k < S.size(); k++){
                S2.push_back(S[k]);
            }
            //on résoud S1 de manière exacte, on découpe S2 (le reste du TT)
            cost = solverGreedy.solve(S1);
            if(!S2.empty() > 0){
                cost += + solve(S2) + cout_sortant*cut(S1, S2);
            } 
            //cout << cost << '\n';
            //cout << C[key] << '\n';
            if(cost > 0 && (cost < C[key] || C.find(key) == C.end())){
                C[key] = cost;
                P1[key] = convert(S1);
                P2[key] = convert(S2);
            }
        }
    }else if(S.size() == 1){
        Tab p = {S[0], S[0] + size/2};
        //overkill
        C[key] = solverGreedy.solve(p);
    }
    return C[key];
}

/**
 * @brief computes the S.size()-1'th column of A
 * 
 * @param S The tensors in this state
 * @return Tab an updated copy of A
 */
CostTab VSplit::computeA(Tab S){
    for(int i : S){
        //poids sortant de i dans l'ensemble de taille S.size() = G[ofs + i] / le poids des liaisons avec des sommets dans S
        A[size*(S.size()-1) + i] = G[size*size + i];
        A[size*(S.size()-1) + i+size/2] = G[size*size + i+size/2];
        
        for(int k : S){
            A[size*(S.size()-1) + i] /= G[size*i + k];
            A[size*(S.size()-1) + i] /= G[size*i + k+size/2];
            A[size*(S.size()-1) + i + size/2] /= G[size*(i+size/2) + k];
            A[size*(S.size()-1) + i + size/2] /= G[size*(i+size/2) + k + size/2];
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
Cost VSplit::cut(Tab S1, Tab S2){
    Cost res = 1;
    for(int i : S1){
        for(int j : S2){
            res *= G[size*i + j];
            res *= G[size*i + j + size/2];
        }
    }
    return res;
}

/**
 * @brief computes the product of all the edges leaving a state
 * 
 * @param S The tensors in this state
 * @param A The external-weight of all tensors for each states
 * @return int 
 */
Cost VSplit::produit_sortant(Tab S, Tab A){
    Cost res = 1;
    for(int i : S){
        res *= A[size*(S.size()-1) + i];
        res *= A[size*(S.size()-1) + i + size/2];
    }
    return res;
}

/**
 * @brief converts a state in a unique integer key
 * 
 * @param S The tensors in this state
 * @return int 
 */
long int VSplit::convert(Tab S){
    int res = 0;
    for(int i : S){
        if(i < size/2){
            res += pow(2, i);
        }
    }
    return res;
}

/**
 * @brief converts a key into a set of tensors
 * 
 * @param key a code generated from a state using convert(S)
 * @return Tab 
 */
Tab VSplit::recover(unsigned long long key){
    Tab res;
    for(int i = size/2; i >= 0; i--){
        int p = pow(2, i);
        if(key >= p){
            res.push_back(i);
            res.push_back(i+size/2);
            key -= p;
        }
    }
    return res;
}

Tab VSplit::recover_full(Tab S){
    Tab res;
    for(int i : S){
        res.push_back(i);
        res.push_back(i+size/2);
    }
    return res;
}

void VSplit::display_order(Tab S){
    if(S.size() >= 1){
        long int key = convert(S);
        if(key != -1){
            Tab p1K = recover(P1[key]);
            solverGreedy.display_order(p1K);
            if(!p1K.empty() && p1K.size() != S.size()){
                //display_order(recover(P1[key]));
                display_order(recover(P2[key]));
            }
            cout << "| ";
            for(int i : recover(key)){
                cout << i << " | ";
            }
            cout << endl;
        }
    }
}

/**
 * @brief dummy method to use in template
 * 
 */
void VSplit::display_order(){}

void VSplit::init(string file){
    S.clear();
    G.clear();
    A.clear();
    C.clear();
    P1.clear();
    P2.clear();

    bestCost = numeric_limits<Cost>::max()-1;

    ifstream ifile(file);
    string line;
    int i, j, w;
    while(getline(ifile, line)){
        istringstream flux(&line[2]);
        switch(line[0]){
            case 'p':
                size = atoi(&line[2]);
                delta = min(3, size/2);
                if(refdelta > 0){
                    delta = min(delta, refdelta);
                }
                S.resize(size/2);
                G.resize(size*(size+1), 1);
                A.resize(size*size, 1);
                /*C.resize(pow(2, size/2), -1);
                P1.resize(pow(2, size/2), -1);
                P2.resize(pow(2,size/2), -1);*/
            break;
            case 'e':
                flux >> i >> j >> w;
                G[size*i + j] = w;
                G[size*j + i] = w;
                G[size*size + i] *= w;
                G[size*size + j] *= w;
            break;
            default:
            break;
        }
    }

    for(int i = 0; i < size/2; i ++){
        S[i] = i;
    }

    solverGreedy.init(file);
}

Cost VSplit::call_solve(){
    return solve(S);
}