#include "ConvexSplits.hpp"

cost_t ConvexSplits::solve(int i1, int i2, int j1, int j2, vector_cost_t const& ext_cost_tab){
    unsigned long long key = convert(i1, i2, j1, j2);
    if(m_cost_memo.find(key) == m_cost_memo.end()){
        //TODO: stocker ext_cost de ext_cost_tab dans AS[S.size()] ou quelque chose comme ça, de sorte que solve_diag et solve_vert puissent y accéder
        // cost_t S_cost;
        cost_t S_cost = ext_cost(i1, i2, j1, j2, ext_cost_tab);
        //affiche(i1, i2, j1, j2);
        //affiche_A(ext_cost_tab);
        //valeur par défaut, facilite les comparaisons
        //TODO:
        m_cost_memo[key] = std::numeric_limits<cost_t>::max();//best_cost;
        //coupes diagonales
        solve_diag(i1, i2, j1, j2, key, ext_cost_tab, S_cost);

        //coupes verticales
        solve_vertical(i1, i2, j1, j2,key, ext_cost_tab, S_cost);

        // printf("State %d %d %d %d\n", i1, i2, j1, j2);
        // printf("Computed cost : %d\n", m_cost_memo[key]);
    }

    return m_cost_memo[key];
}

/**
 * @brief iterates over all diagonal cuts int the shape
 * 
 * @param i1 
 * @param i2 
 * @param j1 
 * @param j2 
 * @param key the key of the shape
 * @param ext_cost_tab 
 * @param S_cost the outer-cost of the shape
 * @return cost_t 
 */
cost_t ConvexSplits::solve_diag(int i1, int i2, int j1, int j2, unsigned long long key, vector_weight_t const& ext_cost_tab, cost_t const& S_cost){
    //les blocs sont un peu difficiles à gérer, mais n'ont pas de découpe illégale en soit
    //il faut borner le sous-bloc pour éviter de couper les branches
    //les branches sont à gérer à part, elles ne doivent être coupées que verticalement
    int start = max(i1, j1); //le côté gauche du sous-bloc
    int end = min(i2, j2); //le côté droit du sous-bloc

    //int ignored = m_edge_list.size()-1; //TODO: if that's a constant why is it here ???
    //size <-> size
    //ext_cost_tab[size] = 1
    //m_weight_list[m_edge_list.size()-1] = 1
    
    //on itère sur les arêtes centrales du sous-bloc
    //Si la forme est une ligne, on aura start > end et cette boucle sera passée 
    for(int ofsy = start; ofsy <= end; ofsy++){
        vector_weight_t ect_copy (ext_cost_tab);
        //on multiplie de manière permanente le poids des extrémités de ofs (même fonctionnement que pour y_cost)
        //on multiplie de manière temporaire le poids des extrémités de start et de end (qu'on reset après chaque solve)
        //x = en haut
        //z = en bas
        int y = dim-1+ofsy; //l'indice de l'arête
        //arête supérieure avant y, et arête inférieure avant y, initialisé à une valeur qui ne posera pas de problème
        int cx = y-dim;
        int cz = y+dim-1;
        //on vérifie que les arêtes sont dans le bloc, en comparant avec start
        //si elles le sont, on les prend en compte dans le coût de séparation

        //upward left piece
        int ula1=i1, ula2, ulb1=j1, ulb2=ofsy-1;
        //upward right piece
        int ura1, ura2=i2, urb1=j2, urb2=j2;
        //downward left piece
        int dla1=i1, dla2=ofsy-1, dlb1=j1, dlb2;
        //downward right piece
        int dra1=i2, dra2=i2, drb1, drb2=j2;

        //on ne les considère que si elles sont légales
        if(ofsy == start){
            ulb1 = -1;
            ulb2 = -1;
            dla1 = -1;
            dla2 = -1;

            dra1 = i1;
            urb1 = j1;

            cx = ignored;
            cz = ignored;
        }
        cost_t start_x_cost = m_weight_list.at(cx), start_z_cost = m_weight_list.at(cz);

        //variable dans laquelle on va accumuler le produit des arêtes centrales
        cost_t y_cost = 1;
        //on itère à nouveau sur les arêtes centrales, en se limitant aux dmax suivantes
        //TODO: It appears that in extremely rare cases, for a very specific value of dmax, the algorithm will break.
        //ex : dmax = 6, 11 for instance_30_random
        //I tested every other one and nothing happened...
        for(int ofs = ofsy; (ofs-ofsy+1 <= dmax) && ofs <= end; ofs ++){
            
            //extrémités dépendantes de ofs
            ura1 = ofs+1;
            drb1 = ofs+1;
            
            ula2 = ofs;
            urb1 = min(urb1, ofs);
            dlb2 = ofs;
            dra1 = min(dra1, ofs);

            //dernière arête centrale
            int last_y = dim-1+ofs;
            //arête finale en haut
            int x = ofs;
            //arête finale en bas
            int z = n_vertex-1 + ofs;
            //mise à jour du coût de séparation
            y_cost *= m_weight_list.at(last_y);
            //mise à jour de ect_copy
            ect_copy.at(m_edge_list[last_y].first) *= m_weight_list[last_y];
            ect_copy.at(m_edge_list[last_y].second) *= m_weight_list[last_y];

            cost_t end_x_cost = m_weight_list.at(x), end_z_cost = m_weight_list.at(z);
            //on ne les considères que si elles sont légales
            if(ofs==end){
                ura1 = -1;
                ula2 = i2;

                ura2 = -1;
                drb1 = -1;
                drb2 = -1;
                dlb2 = j2;

                x = ignored;
                z = ignored;
                end_x_cost = 1;
                end_z_cost = 1;
            }

            //dans les 4 cas, on vérifie que le solving est inférieur au meilleur coût
            //si ce n'est pas le cas, on set le coût à bestCost et on ne solve pas le deuxième ensemble
            //si on a rien trouvé de concluant à la fin de l'exécution, on set m_cost_memo[key] à bestCost (ou bestCost+1 pour être sûr)
            {//première coupe : '/'
                //coût de séparation
                cost_t cut = start_z_cost * y_cost * end_x_cost;
                unsigned long total = cut * S_cost;
                //on défini de nouvelles bornes i1, i2, j1, j2
                {//partie gauche
                    //on met à jour les poids sortant, si les coupures sont illégales, leur coût est à 1 -> pas de modification
                    ect_copy.at(m_edge_list.at(cz).first) *= start_z_cost;
                    ect_copy.at(m_edge_list.at(x).first) *= end_x_cost;
                    ect_copy.at(m_edge_list.at(cz).second) *= start_z_cost;
                    ect_copy.at(m_edge_list.at(x).second) *= end_x_cost;

                    total += solve(ula1, ula2, ulb1, ulb2, ect_copy);
                }
            
                //TODO: potential break-point
                //on ne rentre que si le coût jusqu'à présent est inférieur au meilleur coût trouvé pour cet ensemble
                if(total < m_cost_memo[key] && total > 0)
                {//partie droite 
                    total += solve(ura1, ura2, urb1, urb2, ect_copy);
                    if(total < m_cost_memo[key] && total > 0){
                        m_cost_memo[key] = total;
                    }
                }
            }

            //on remet ect_copy à un état antérieur
            ect_copy.at(m_edge_list.at(cz).first) /= m_weight_list.at(cz);
            ect_copy.at(m_edge_list.at(cz).second) /= m_weight_list.at(cz);
            ect_copy.at(m_edge_list.at(x).first) /= m_weight_list.at(x);
            ect_copy.at(m_edge_list.at(x).second) /= m_weight_list.at(x);

            {//deuxième coupe : '\'
            //coût de séparation
            cost_t cut = start_x_cost * y_cost * end_z_cost;
            cost_t total = cut * S_cost;
            //on défini de nouvelles bornes i1, i2, j1, j2
                {//partie gauche
                    ect_copy.at(m_edge_list.at(cx).first) *= start_x_cost;
                    ect_copy.at(m_edge_list.at(z).first) *= end_z_cost;
                    ect_copy.at(m_edge_list.at(cx).second) *= start_x_cost;
                    ect_copy.at(m_edge_list.at(z).second) *= end_z_cost;

                    total += solve(dla1, dla2, dlb1, dlb2, ect_copy);
                }

                //TODO: potential break-point
                //on ne rentre que si le coût jusqu'à présent est inférieur au meilleur coût trouvé pour cet ensemble
                if(total < m_cost_memo[key] && total > 0)
                {//partie droite
                    total += solve(dra1, dra2, drb1, drb2, ect_copy);

                    if(total < m_cost_memo[key] && total > 0){
                        m_cost_memo[key] = total;
                    }
                }
            }
        }   
    }
    return m_cost_memo[key];
}

/**
 * @brief iterates over all vertical cuts in the shape
 * 
 * @param i1 
 * @param i2 
 * @param j1 
 * @param j2 
 * @param key the key of the shape
 * @param ext_cost_tab 
 * @param S_cost the outer-cost of the shape
 * @return cost_t 
 */
cost_t ConvexSplits::solve_vertical(int i1, int i2, int j1, int j2, unsigned long long key, vector_weight_t ext_cost_tab, cost_t const& S_cost){
    int start = min(i1, j1);
    //Si on a plus d'1 sommet
    if((start >= 0) || i1!=i2 || j1!=j2){
        // int ignored = m_edge_list.size()-1;
        //cout << i1 << " " << i2 << " " << j1 << " " << j2 << '\n';
        for(int i = start; i < max(i2, j2); i++){
            //il ne faut pas couper (multiplier par le coût) des arêtes inutiles dans le cas des branches
            //on doit vérifier sur quelle ligne se trouve l'unique arête, et agir en conséquence
            //arête du haut
            int x = ignored;
            //arête du bas
            int z = ignored;

            //TODO:
            cost_t cut = 1;//1UL; ??

            //cas on ne dépasse pas de la ligne i1 i2
            if(i >= i1 && i < i2){
                x = i;
                cut *= m_weight_list.at(x);
                ext_cost_tab.at(m_edge_list.at(x).first) *= m_weight_list.at(x);
                ext_cost_tab.at(m_edge_list.at(x).second) *= m_weight_list.at(x);
            }

            //cas on ne dépasse pas de la ligne j1 j2
            if(i >= j1 && i < j2){
                z = n_vertex-1+i;
                cut *= m_weight_list.at(z);
                ext_cost_tab.at(m_edge_list.at(z).first) *= m_weight_list.at(z);
                ext_cost_tab.at(m_edge_list.at(z).second) *= m_weight_list.at(z);
            }

            cost_t total = cut * S_cost;

            {//partie gauche
                int a1=i1, a2=min(i, i2), b1=j1, b2=min(i, j2);

                //cas pas de ligne haute
                //mettre a2 à -1 garanti de passer la boucle précédente puisque min(-1, b2)=-1 > max(-1, b1)
                //de même on garanti i >= -1 && i < -1 = false ce qui permet d'ignorer l'arête
                if(i < i1){
                    a1 = -1;
                    a2 = -1;
                }
                //cas pas de ligne basse
                else if(i < j1){
                    b1 = -1;
                    b2 = -1;
                }

                //TODO: this can return numeric_limits
                total += solve(a1, a2, b1, b2, ext_cost_tab);
                
            }

            {//partie droite
                int a1=max(i+1, i1), a2=i2, b1=max(i+1, j1), b2=j2;

                //cas pas de ligne haute
                if(i >= i2){
                    a1 = -1;
                    a2 = -1;
                }
                //cas pas de de ligne basse
                else if(i >= j2){
                    b1 = -1;
                    b2 = -1;
                }

                total += solve(a1, a2, b1, b2, ext_cost_tab);
            }

            //TODO: potential break-point
            if(total < m_cost_memo[key] && total > 0){
                m_cost_memo[key] = total;
            }

            //on remet ext_cost_tab à l'état précédent
            ext_cost_tab.at(m_edge_list.at(x).first) /= m_weight_list.at(x);
            ext_cost_tab.at(m_edge_list.at(x).second) /= m_weight_list.at(x);
            ext_cost_tab.at(m_edge_list.at(z).first) /= m_weight_list.at(z);
            ext_cost_tab.at(m_edge_list.at(z).second) /= m_weight_list.at(z);
        }
    }else{
        m_cost_memo[key] = 0;
    }
    return m_cost_memo[key];
}

/**
 * @brief computes the exterior cost of a shape given the exterior cost of its vertices
 * 
 * @param i1 
 * @param i2 
 * @param j1 
 * @param j2 
 * @param ext_cost_tab 
 * @param s out
 */
cost_t ConvexSplits::ext_cost(int i1, int i2, int j1, int j2, vector_weight_t const& ext_cost_tab){
    cost_t s = 1;
    if(i1 >= 0){
        for(int i = i1; i <= i2; i++){
            s *= ext_cost_tab[i];
        }
    }
    if(j1 >= 0){
        for(int i = j1; i <= j2; i++){
            s *= ext_cost_tab[i+dim];
        }
    }
    return s;
}

/**
 * @brief converts a shape into a unique hexadecimal key
 * 
 * @param i1 
 * @param i2 
 * @param j1 
 * @param j2 
 * @return unsigned long long 
 */
unsigned long long ConvexSplits::convert(int i1, int i2, int j1, int j2){
    unsigned long long res = 0;
    res |= (unsigned long long) i1 << 48;
    res |= ((unsigned long long) i2&0x0000FFFF) << 32;
    res |= ((unsigned long long) j1&0x0000FFFF) << 16;
    res |= (j2&0X0000FFFF);
    return res;
}

/**
 * @brief dummy method for template
 * 
 */
void ConvexSplits::display_order(){

}

void ConvexSplits::init(Network& network){
    set_limit_dim(network.dimension);
    //network caracteristics
    dim = network.dimension;
    n_vertex = network.n_vertex;

    m_edge_list = network.edge_list;
    //small manipulation to prevent out-of-bound access
    m_edge_list.push_back(make_pair(n_vertex, n_vertex));

    m_cost_memo.clear();

    //List of weights
    m_weight_list.clear();
    m_weight_list.resize(network.n_edge+1, 1);
    for(edgeID_t i = 0; i < network.n_edge; i++){
        auto& [v1, v2] = m_edge_list[i];
        m_weight_list[i] = network.adjacence_matrix[n_vertex*v1 + v2];
    }

    //a short because it needs to fit into the map, we could update it,
    //it would require changing the encoding tho
    //TODO: as expected this breaks if the cost goes beyond
    //It also breaks the result if we use int
    //what
    best_cost = std::numeric_limits<cost_t>::max();
    best_order.clear();

    //m_cost_memo[-1], a trash bin
    m_cost_memo[0xFFFFFFFFFFFFFFFF] = best_cost;
}

/**
 * @brief dummy function called by exec_file 
 * 
 * @return cost_t 
 */
cost_t ConvexSplits::call_solve(){
    return solve(0, dim-1, 0, dim-1, vector_cost_t (n_vertex+1, 1));
}