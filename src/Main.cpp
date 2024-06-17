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
    if(ALGO_MAP.find(algo_name) == ALGO_MAP.end()){return nullptr;}
    switch(ALGO_MAP[algo_name]){
        case ALLSPLITS:
            return new AllSplits(dictionary);
            break;
        case ALLEDGEBYEDGE:
            return new AllEdgeByEdge(dictionary);
            break;
        case CONVEXSPLITS:
            return new ConvexSplits(dictionary);
            break;
        case GREEDYEDGESORT:
            return new GreedyEdgeSort(dictionary);
            break;
        case SHUFFLE:
            return new Shuffle(dictionary);
            break;
        case TWOSIDEDDELTADIM:
            return new TwoSidedDeltaDim<2, 4, BOTH_SIDES>(dictionary);
            break;
        case COTENGRAOPTIMALWRAPPER:
            return new CotengraOptimalWrapper(dictionary);
            break;
        default:
            std::cerr << "Unknown algorithm '" << algo_name << "'" << std::endl;
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
    }
    std::cout << std::scientific << "Temps : " << solver.time.count()  << "s" << '\n';
    std::cout << "--------------" << std::endl;
}

/**
 * @brief executes solver.execfile inside a thread, and stops it after the time limit is reached 
 * 
 * @tparam T 
 * @param solver
 */
template<class T>
void launch_exec(T& solver, Network& network){
    int status;
    pid_t pid = fork();
    if(pid == 0){ //processus fils
        condition_variable cv;
        mutex mtx;

        // Thread that will carry the execution
        std::thread t1([&solver, &cv, &network](){
            execfile_no_display(solver, network);
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
            exit(-1);
        }else{
            //Results export
            export_entry(result_file, solver, network, csv_separator);

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
void launch_untimed_exec(T& solver, Network& network){
    execfile_no_display(solver, network);
    //display
    display_info(solver);

    export_entry(result_file, solver, network, csv_separator);
}

/**
 * @brief Executes all algorithms on a single network
 * 
 */
void exec_all_on_file(Network& network){
    for(int i = 0; i < main_algorithm_list.size(); i++){ //auto& algo : main_algorithm_list){
        Algorithm* algo = main_algorithm_list[i];
        if(algo->still_up){
            // algo->init(network);
            printf("Algo. %d (%s) : '%s' :\n", i, algo->algo_name.data(), network.m_filename.data());
            launch_exec(*algo, network);
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
    for(auto& network : main_network_list){
        exec_all_on_file(network);
    }
}

int main(int argc, char* argv[]){
    //gather the arguments into lists
    //a list of maps for the algorithms, and a list of list for files
    Argparser parser(argc, argv);

    //open the output file if exists
    if(!parser.output_file.empty()){
        result_file = open_output(parser.output_file);
    }

    //fills main_algorithm_list with algorithms instantiated using the dictionary_list
    init_algos(parser.grab_dictionary_list());

    //sorted list of entries, from smallest to biggest
    main_network_list = parser.file_entries_list;
    std::sort(main_network_list.begin(), main_network_list.end(), 
    [](Network n1, Network n2){return n1.dimension < n2.dimension;});

    exec_all_on_all();
}