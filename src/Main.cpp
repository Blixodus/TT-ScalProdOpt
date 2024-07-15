#include "Main.hpp"


/**
 * @brief Instantiates every algorithm and stores them in main_algorithm_list
 * 
 * @param dict_list a list generated using Argparser.grab_dictionnary_list()
 */
void init_algos(std::vector<std::map<std::string, std::any>> dict_list){
    // Iterate over the dictionaries
    for(auto& dict : dict_list){
        std::string algo_name = std::any_cast<std::string>(dict["main_alg"]);
        std::cout << "Instantiating : " << algo_name << std::endl;
        // Instantiate the right algorithm based on its name
        Algorithm* new_alg = instantiate(dict);
        
        // Verify that the algorithm exists
        if(new_alg != nullptr){
            main_algorithm_list.emplace_back(new_alg);

            // Initialization of sub_alg
            if(dict.find("sub_alg") != dict.end()){
                new_alg->sub_alg = instantiate(std::any_cast<std::string>(dict["sub_alg"]));
            }

            // Initialization of start_sol
            if(dict.find("start_sol") != dict.end()){
                new_alg->start_sol = instantiate(std::any_cast<std::string>(dict["start_sol"]));
            }
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * @brief Instantiates a new Algorithm based on a parameter dictionary
 * 
 * @param dictionary 
 * @return Algorithm* 
 */
Algorithm* instantiate(std::map<std::string, std::any>& dictionary){
    std::string algo_name(std::any_cast<std::string>(dictionary["main_alg"]));
    int delta = 0;
    int tt_dim = 0;

    if(dictionary.find("delta") != dictionary.end()){
        delta = std::stoi(std::any_cast<string>(dictionary["delta"]));
    } else if(dictionary.find("dmax") != dictionary.end()){
        // Backward compatibility
        delta = std::any_cast<int>(dictionary["dmax"]);
    }

    if(dictionary.find("tt_dim") != dictionary.end()) {
        tt_dim = std::stoi(std::any_cast<string>(dictionary["tt_dim"]));
    }

    if(ALGO_MAP.find(algo_name) == ALGO_MAP.end()){return nullptr;}
    switch(ALGO_MAP[algo_name]){
        case ONESIDEDONEDIM:
            return new OneSidedOneDim(dictionary);
            break;
        case TWOSIDEDDELTADIM:
            if (tt_dim == 2) {
                if(delta == 3) return new TwoSidedDeltaDim<3, ALL, 2>(dictionary);
                else if(delta == 4) return new TwoSidedDeltaDim<4, ALL, 2>(dictionary);
                else if(delta == 5) return new TwoSidedDeltaDim<5, ALL, 2>(dictionary);
                else if(delta == 6) return new TwoSidedDeltaDim<6, ALL, 2>(dictionary);
                else if(delta == 7) return new TwoSidedDeltaDim<7, ALL, 2>(dictionary);
                else if(delta == 8) return new TwoSidedDeltaDim<8, ALL, 2>(dictionary);
                else if(delta == 9) return new TwoSidedDeltaDim<9, ALL, 2>(dictionary);
                else if(delta == 10) return new TwoSidedDeltaDim<10, ALL, 2>(dictionary);
                else {
                    std::cerr<<"[Warning] Using default delta value of 3. Please add delta to template in Main.cpp in order to use it."<<std::endl;
                    return new TwoSidedDeltaDim<3, ALL, 2>(dictionary);
                }
            } else if(tt_dim == 3) {
                if(delta == 3) return new TwoSidedDeltaDim<3, ALL, 3>(dictionary);
                else if(delta == 4) return new TwoSidedDeltaDim<4, ALL, 3>(dictionary);
                else if(delta == 5) return new TwoSidedDeltaDim<5, ALL, 3>(dictionary);
                else if(delta == 6) return new TwoSidedDeltaDim<6, ALL, 3>(dictionary);
                else if(delta == 7) return new TwoSidedDeltaDim<7, ALL, 3>(dictionary);
                else if(delta == 8) return new TwoSidedDeltaDim<8, ALL, 3>(dictionary);
                else if(delta == 9) return new TwoSidedDeltaDim<9, ALL, 3>(dictionary);
                else if(delta == 10) return new TwoSidedDeltaDim<10, ALL, 3>(dictionary);
                else {
                    std::cerr<<"[Warning] Using default delta value of 3. Please add delta to template in Main.cpp in order to use it."<<std::endl;
                    return new TwoSidedDeltaDim<3, ALL, 3>(dictionary);
                }
            } else if(tt_dim == 4) {
                if(delta == 3) return new TwoSidedDeltaDim<3, ALL, 4>(dictionary);
                else if(delta == 4) return new TwoSidedDeltaDim<4, ALL, 4>(dictionary);
                else if(delta == 5) return new TwoSidedDeltaDim<5, ALL, 4>(dictionary);
                else if(delta == 6) return new TwoSidedDeltaDim<6, ALL, 4>(dictionary);
                else if(delta == 7) return new TwoSidedDeltaDim<7, ALL, 4>(dictionary);
                else if(delta == 8) return new TwoSidedDeltaDim<8, ALL, 4>(dictionary);
                else if(delta == 9) return new TwoSidedDeltaDim<9, ALL, 4>(dictionary);
                else if(delta == 10) return new TwoSidedDeltaDim<10, ALL, 4>(dictionary);
                else {
                    std::cerr<<"[Warning] Using default delta value of 3. Please add delta to template in Main.cpp in order to use it."<<std::endl;
                    return new TwoSidedDeltaDim<3, ALL, 3>(dictionary);
                }
            } else {
                std::cerr<<"[Warning] Using default tt_dim value of 2. Please add tt_dim to template in Main.cpp in order to use it."<<std::endl;
                return new TwoSidedDeltaDim<3, ALL, 2>(dictionary);
            }
            break;
        case COTENGRAWRAPPER:
            if(tt_dim == 2) {
                return new CotengraWrapper<2>(dictionary);
            } else if(tt_dim == 3) {
                return new CotengraWrapper<3>(dictionary);
            } else if(tt_dim == 4) {
                return new CotengraWrapper<4>(dictionary);
            } else {
                std::cerr<<"[Warning] Using default tt_dim value of 2. Please add tt_dim to template in Main.cpp in order to use it."<<std::endl;
                return new CotengraWrapper<2>(dictionary);
            }
            break;
        default:
            std::cerr<<"[Error] Unknown algorithm '"<<algo_name<<"'"<<std::endl;
            return nullptr;
            break;
    }
}

/**
 * @brief Instantiates a new Algorithm based on a name
 * 
 * @param algorithm_name 
 * @return Algorithm* 
 */
Algorithm* instantiate(const std::string& algorithm_name){
    std::map<std::string, std::any> buff_dict({{"main_alg", algorithm_name}});
    return instantiate(buff_dict);
}

void display_infos(Algorithm& solver){
    std::cout << "Best cost : " << solver.best_cost << '\n';
    if(!solver.best_order.empty()){
        std::cout << "Best order : ";
        solver.display_order();
    } else if(solver.best_order_str != ""){
        std::cout << "Best order : " << solver.best_order_str << '\n';
    }
    std::cout << std::scientific << "Execution time : " << solver.time.count()  << "s" << '\n';
    std::cout << "--------------" << std::endl;
}

/**
 * @brief executes solver.execfile inside a thread, and stops it after the time limit is reached 
 * 
 * @tparam T 
 * @param solver
 */
template<class T>
void launch_exec(T& solver, std::string network_file){
    int status;
    pid_t pid = fork();
    if(pid == 0){ //processus fils
        condition_variable cv;
        mutex mtx;

        // Thread that will carry the execution
        std::thread t1([&solver, &cv, &network_file](){
            execfile_no_display(solver, network_file);
            cv.notify_all();
        });

        pthread_t tid = t1.native_handle();
        t1.detach();

        //un mutex lock le thread "principal" tant que l'autre n'a pas fini (ou jusqu'à la fin du délai)
        std::unique_lock<std::mutex> lock(mtx);
        //TODO: it might not work, as solver may not be initialized at this stage
        auto status = cv.wait_for(lock, solver.timeout_time);

        // si timeout il y a, on tue le thread
        if (status == std::cv_status::timeout) {
            pthread_cancel(tid);
            solver.best_cost = -1;
            std::cout << "Timed out." << std::endl;
            exit(-3);
        }else{
            //display
            display_infos(solver);
        }
        
        exit(0);

    }else{ //processus parent
        wait(&status);
        if(status!=0){ //if timeout
            //the algorithm leaves the execution queue
            solver.still_up = false;
        }
    }
}

template<class T>
void launch_untimed_exec(T& solver, std::string network_file){
    execfile_no_display(solver, network_file);
    //display
    display_info(solver);
}

/**
 * @brief Executes all algorithms on a single network
 * 
 */
void exec_all_on_file(std::string network_file){
    for(int i = 0; i < main_algorithm_list.size(); i++){ //auto& algo : main_algorithm_list){
        Algorithm* algo = main_algorithm_list[i];
        if(algo->still_up){
            // algo->init(network);
            printf("Algo. %d (%s) : '%s' :\n", i, algo->algo_name.data());
            launch_exec(*algo, network_file);
            // launch_untimed_exec(*algo, network);
        }
    }
}

/**
 * @brief Executes all algorithms on all files
 * Iterates over the algorithms, then over the files
 * 
 */
void exec_all_on_all(){
    //iteration over the networks
    for(auto& network_file : main_network_list){
        exec_all_on_file(network_file);
    }
}

int main(int argc, char* argv[]){
    //gather the arguments into lists
    //a list of maps for the algorithms, and a list of list for files
    Argparser parser(argc, argv);

    //fills main_algorithm_list with algorithms instantiated using the dictionary_list
    init_algos(parser.grab_dictionary_list());

    //sorted list of entries, from smallest to biggest
    main_network_list = parser.file_entries_list;

    exec_all_on_all();
}