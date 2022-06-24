#include "Main.hpp"

void init_algos(){
    algos.push_back(new MatrixSolver());
    algos.push_back(new Split());
    algos.push_back(new VSplit());
    algos.push_back(new ESplit());
    algos.push_back(new SideEx());
    algos.push_back(new SimpleG());
    algos.push_back(new TriScore());
    algos.push_back(new TriScoreM());
    algos.push_back(new NTS());
}

/**
 * @brief adds 'separator' to cost_file and time_file
 * 
 */
void add_separator(){
    cost_file << separator << flush;
    time_file << separator << flush;
}

void add_error_values(){
    cost_file << -1 << separator << flush;
    time_file << 0.0 << separator << flush;
}
/**
 * @brief exports solver's name into result_file
 * 
 * @tparam T the solver's type
 * @param solver 
 */
template<class T>
void export_header(T& solver){
    string class_name = typeid(solver).name();
    do{
        class_name.erase(class_name.begin());
    }while(isdigit(class_name[0]));
    cout << "--- "<< class_name <<" ---" << '\n';
    result_file << "--- " << class_name << " ---" << '\n';
    if(solver.delta != -1){
        result_file << "Delta : " << solver.delta << '\n'; 
    }
}

/**
 * @brief exports results into the different files
 * 
 * @tparam T the solver's type
 * @param solver 
 */
template<class T>
void export_results(T& solver){
    result_file << "Best cost : " << solver.bestCost << '\n';
    result_file << "Time : " << solver.time.count() << endl;

    cost_file << solver.bestCost;
    time_file << solver.time.count();
    add_separator();
}

/**
 * @brief executes solver.execfile inside a thread, and stops it after the time limit is reached 
 * 
 * @tparam T 
 * @param solver 
 * @param delta affects the behaviour of some heuristics
 */
template<class T>
void launch_exec(T& solver, int delta){
    int status;
    pid_t pid = fork();
    if(pid == 0){ //processus fils
        condition_variable cv;
        mutex mtx;

        std::thread t1([&solver, &cv, &delta](){
            solver->refdelta = delta;
            export_header(*solver);
            //Template défini dans components, permettant d'initialiser l'algorithme et de résoudre l'instance
            execfile(*solver, instance_file);
            export_results(*solver);
            cv.notify_all();
        });

        pthread_t tid = t1.native_handle();
        t1.detach();

        //un mutex lock le thread "principal" tant que l'autre n'a pas fini (ou jusqu'à la fin du délai)
        std::unique_lock<std::mutex> lock(mtx);
        auto status = cv.wait_for(lock, WAIT_TIME);

        // si timeout il y a, on tue le thread
        if (status == std::cv_status::timeout) {
            pthread_cancel(tid);
            solver->bestCost = -1;
            export_results(*solver);
            exit(-1);
        }
        
        exit(0);

    }else{ //processus parent
        wait(&status);
        if(status!=0){ //si timeout
            solver->still_up = false;
            //add_error_values();
        }
    }
}

/**
 * @brief initializes .csv's header
 * 
 */
void init_csv(){
    /*size, Matrix, Split, VSplit, EdgeSplit, SideEx, SimpleG, TriScore, MTS, NTS*/
    /*for(auto algo = algos.begin(); algo != algos.end(); algo++){
        cout << "in" << '\n';
        launch_exec(*algo);

    }*/
    string header = "size" + separator + 
    "Matrix" + separator + 
    "Split" + separator + 
    "VSplit" + separator + 
    "EdgeSplit" + separator +
    "SideEx" + separator + 
    "SimpleG" + separator +
    "TriScore" + separator +
    "TriScoreMargin" + separator +
    "TriScoreNaive"
    + '\n';
    cost_file << header;
    time_file << header;
}

/**
 * @brief puts the size of instances into the .csv
 * 
 */
void get_size(){
    string path = "../instances/" + instance_file;
    string filename(path);
    string line;
    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        exit(-1);
    }

     while(getline(input_file, line)){
        if(line.size() > 2){
            if(line[0] == 'p'){
                /*cost_file << inst_num;
                time_file << inst_num;
                add_separator();
                inst_num++;*/
                cost_file << &line[2];
                time_file << &line[2];
                add_separator();
                break;
            }
        }
     }

}

/**
 * @brief output the display into result_file
 * 
 */
void export_display(){
    string path = "../instances/" + instance_file;
    string filename(path);
    string line;
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
                            result_file << mot << "(" << n << ")" << padding;
                            n++;
                        }else{
                            result_file << mot << "   ";
                        }
                    }
                    nL++;
                    result_file << '\n';
                break;
                case 'p':
                    result_file << '\n' << "Sommets : " << &line[2] << '\n';
                break;
                default:
                break;
            }
        }
    }
    result_file << '\n';
}

/**
 * @brief exports a contraction order into result_file
 * 
 * @param O a vector<int> representing a series of edges
 */
void export_order(Tab O){
    result_file << "Best order : ";
    for(int i : O){
        result_file << i << " ";
    }
    result_file << '\n';
}

/**
 * @brief initializes various files
 * 
 */
void init_files(){
    result_file << instance_file << '\n';
    display(instance_file);
    export_display();
    get_size();
}

/**
 * @brief main method, executes every algorithm defined inside on instance_file
 * 
 */
void execfile_on_all(){
    init_files();
    
    for(auto algo = algos.begin(); algo != algos.end(); algo++){
        cout << typeid((*algo)).name() << '\n';
        cout << (*algo)->still_up << '\n';
        if((*algo)->still_up){
            cout << "in exec" << '\n';
            launch_exec(*algo);
        }else{
            cout << "in trash" << '\n';
            //cost_file << "-1";
            //time_file << "0" + separator;
            add_error_values();
        }
    }
    /*launch_exec(matrix);
    launch_exec(split);
    launch_exec(vsplit, 3);
    launch_exec(esplit);
    launch_exec(sideex);
    launch_exec(simpleg);
    launch_exec(triscore);
    launch_exec(mts, 3);
    launch_exec(nts);*/

    cost_file << endl;
    time_file << endl;
    result_file << "----------" << "\n\n";
}

/**
 * @brief sorts every file in instance_dir
 * 
 * @return vector<string> 
 */
vector<string> sort_files(){
    vector<string> file_list;
    string directory = "../instances/" + instance_dir + "/";
    DIR* dp = NULL;
    struct dirent *file = NULL;
    dp = opendir(directory.c_str());
    if(dp == NULL){
        cerr << "Could not open directory : " << directory << '\n';
        exit(-1);
    }
    file = readdir(dp);
    
    while(file != NULL){
        if(file->d_name[0] != '.'){
            //instance_file = directory + file->d_name;
            //execfile_on_all();
            file_list.push_back(directory + file->d_name);
        }
        file = readdir(dp);
    }
    closedir(dp);
    //un sort pour ordonner les instances (marche que si elles contiennent un indice)
    sort(file_list.begin(), file_list.end(), [](const string& x, const string& y){
            return x.length() < y.length() || (x.length() == y.length() && x < y);
        });
    return file_list;
}

/**
 * @brief iterates through every instances in instance_dir
 * 
 */
void execdir(){
    for(auto& n : sort_files()){
        instance_file = n;
        execfile_on_all();
    }
}

int main(int argc, char* argv[]){
    switch (argc){
    case 1:
        cout << "Missing argument : instance file" << '\n';
        cout << "Use 'h' to get infos" << '\n';
        return -1;
        break;
    case 2:
        if(argv[1] == string("h")){
            cout << "d directory : executes all solvers on every instance of the directory" << '\n';
            cout << "v file : displays the graph represented by this instance" << '\n';
            cout << "vd directory : displays all instances in the directory" << '\n';
            cout << "filename : execute all the algorithms on this instance" << '\n';
            cout << "Best costs marked with '*' are the optimal costs calculated using greedy algorithms" << '\n';
            cout << "The root directory's name is 'instances'" << '\n';
        }else{
            instance_file = argv[1];
            init_algos();
            init_csv();
            execfile_on_all();
        }
        break;
    case 3:
        if(argv[1] == string("d")){
            instance_dir = argv[2];
            init_algos();
            init_csv();
            execdir();
        }else if(argv[1] == string("vd")){
            display_dir(argv[2]);
        }else if(argv[1] == string("v")){
            display(argv[2]);
        }

        break;
    default:
        cout << "Error : check arguments list" << endl;
        break;
    }
    return 0;
}