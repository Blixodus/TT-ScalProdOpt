#include "Main.hpp"


/**
 * @brief Instantiates every algorithm and stores them in main_algorithm_list
 * 
 * @param dict_list a list generated using Argparser.grab_dictionnary_list()
 */
void init_algos(std::vector<std::map<std::string, std::any>> dict_list){
    //we iterate over the dictionaries
    for(auto& dict : dict_list){
        std::string algo_name = std::any_cast<std::string>(dict["main_alg"]);
        //we instantiate the right algorithm based on its name
        switch(algo_map[algo_name]){
            case ALLSPLITS:
                main_algorithm_list.push_back(new AllSplits(dict));
                break;
            case ALLEDGEBYEDGE:
                main_algorithm_list.push_back(new AllEdgeByEdge(dict));
                break;
            case CONVEXSPLITS:
                main_algorithm_list.push_back(new ConvexSplits(dict));
                break;
            case GREEDYEDGESORT:
                main_algorithm_list.push_back(new GreedyEdgeSort(dict));
                break;
            case ONESIDEDIMBYDIM:
                main_algorithm_list.push_back(new OneSideDBD(dict));
                break;
            case SHUFFLE:
                main_algorithm_list.push_back(new Shuffle(dict));
                break;
            case SPLITSDIMBYDIM:
                main_algorithm_list.push_back(new SplitsDBD(dict));
                break;
            default:
                std::cerr << "Unknown algorithm '" << algo_name << "'" << std::endl;
                break;
        }
    }
    printf("\n");
}

//TODO: remove delta from this code
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

        //Thread that will carry the execution
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
            exit(-1);
        }else{
            //Results export
            //TODO:

            //display
            std::cout << "Best cost : " << solver.best_cost << '\n';
            if(!solver.best_order.empty()){
                std::cout << "Best order : ";
                solver.display_order();
            }
            std::cout << std::scientific << "Temps : " << solver.time.count()  << "s" << '\n';
            std::cout << "--------------" << std::endl;
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
    std::cout << "Best cost : " << solver.best_cost << '\n';
    std::cout << "size : " << solver.best_order.size() << '\n';
    if(!solver.best_order.empty()){
        std::cout << "Best order : ";
        solver.display_order();
    }
    std::cout << std::scientific << "Temps : " << solver.time.count()  << "s" << '\n';
    std::cout << "--------------" << std::endl;
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
            printf("Algo. %d : '%s' :\n", i, network.m_filename.data());
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

    //fills main_algorithm_list with algorithms instantiated using the dictionary_list
    init_algos(parser.grab_dictionary_list());

    //sorted list of entries, from smallest to biggest
    main_network_list = parser.file_entries_list;
    std::sort(main_network_list.begin(), main_network_list.end(), 
    [](Network n1, Network n2){return n1.dimension < n2.dimension;});

    exec_all_on_all();
}