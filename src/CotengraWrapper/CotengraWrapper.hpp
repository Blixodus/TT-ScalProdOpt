/**
 * @brief 
 * Wrapper for the Cotengra library optimal algorithm
 * Type : exact algorithm
 */
#ifndef COTENGRA_WRAPPER_HPP
#define COTENGRA_WRAPPER_HPP
#define PYBIND11_DETAILED_ERROR_MESSAGES
#include "../Components/Algorithm.hpp"

#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

template <size_t tt_dim = 2>
class CotengraWrapper : public Algorithm {
    private:
    // Algorithm from Cotengra library to use as a solver
    std::string algorithm = "optimal";

    // Generalized network information
    Network2D<tt_dim> m_network_2d;

    std::vector<vertexID_t> m_node_ids[tt_dim]; // 2D grid of node ids for given row and column
    std::map<std::pair<vertexID_t, vertexID_t>, wchar_t> m_edge_symbols; // map of edge symbols
    std::map<wchar_t, cost_t> m_weights_map; // map of weights for each edge symbol
    std::vector<cost_t> m_weights; // vector of weights for each edge symbol (to look up during generation of subproblems)

    std::wstring cotengra_base_symbols = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Generate the symbol corresponding to index i
    // Compatible with the Cotengra get_symbol function: https://cotengra.readthedocs.io/en/main/autoapi/cotengra/index.html#cotengra.get_symbol
    wchar_t get_symbol(int i) {
        // Return symbol from a-z
        if(i < 52) {
            return this->cotengra_base_symbols[i];
        }

        // Return symbols starting from À
        i += 140;

        // Skip symbols chr(57343) - chr(55296)
        if(i >= 55296) {
            i += 2048;
        }

        return (wchar_t)i;
    }

    void generate_subproblem(const int dim_min, const int dim_max, const result_direction_e direction, std::vector<std::wstring>& input, std::wstring& output, std::map<wchar_t, cost_t>& weights, bool& input_node_included) {
        // Generate the input node (representing the part of TT which is already
        // contracted prior to this subproblem) and the output node (representing 
        // the part of TT which is result of this computation)

        // Add outer edges from the left size to include them in the cost
        if(dim_min > 0) {
            std::wstring edges;
            for(int i = 0; i < tt_dim; i++) {
                wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[i][dim_min - 1], this->m_node_ids[i][dim_min]}];
                edges += symbol;
                weights[symbol] = this->m_weights[symbol];
            }

            if(direction == LR) {
                input.push_back(edges);
                input_node_included = true;
            } else {
                output += edges;
            }
        }

        // Add outer edges from the right size to include them in the cost
        if(dim_max + 1 < this->m_network_2d.dimension) {
            std::wstring edges;
            for(int i = 0; i < tt_dim; i++) {
                wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[i][dim_max], this->m_node_ids[i][dim_max + 1]}];
                edges += symbol;
                weights[symbol] = this->m_weights[symbol];
            }

            if(direction == LR) {
                output += edges;
            } else {
                input.push_back(edges);
                input_node_included = true;
            }
        }

        // Set the upper left/right, middle and lower left/right indices
        //   |
        // --O--
        //   |
        int move_x[] = {0, 1, 0, -1};
        int move_y[] = {1, 0, -1, 0};

        for(int i = 0; i < tt_dim; i++) {
            for(int j = dim_min; j <= dim_max; j++) {
                std::wstring edges;
                for(int k = 0; k < 4; k++) {
                    int x = i + move_x[k];
                    int y = j + move_y[k];
                    if(x >= 0 && x < tt_dim && y >= max(0, dim_min - 1) && y <= min(this->m_network_2d.dimension - 1, dim_max + 1)) {
                        wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[i][j], this->m_node_ids[x][y]}];
                        edges += symbol;
                        weights[symbol] = this->m_weights[symbol];
                    }
                }
                input.push_back(edges);
            }
        }
    }

        void generate_subproblem(const int i_min, const int j_min, const int i_max, const int j_max, const result_direction_e direction, std::vector<std::wstring>& input, std::wstring& output, std::map<wchar_t, cost_t>& weights, bool& input_node_included) {
        // Generate the input node (representing the part of TT which is already
        // contracted prior to this subproblem) and the output node (representing 
        // the part of TT which is result of this computation)

        // Add outer edges from the left size to include them in the cost
        std::wstring left_edges;

        if(i_min > 0) {
            // Upper rank edge
            wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[0][i_min - 1], this->m_node_ids[0][i_min]}];
            left_edges += symbol;
            weights[symbol] = this->m_weights[symbol];
        }

        if(j_min > 0) {
            // Lower rank edge
            wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[1][j_min - 1], this->m_node_ids[1][j_min]}];
            left_edges += symbol;
            weights[symbol] = this->m_weights[symbol];
        }

        if(i_min != j_min) {
            // Mode edges
            for(int i = min(i_min, j_min); i < max(i_min, j_min); i++) {
                wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[0][i], this->m_node_ids[1][i]}];
                left_edges += symbol;
                weights[symbol] = this->m_weights[symbol];
            }
        }

        if(left_edges != L"") {
            if(direction == LR) {
                input.push_back(left_edges);
                input_node_included = true;
            } else {
                output += left_edges;
            }
        }

        // Add outer edges from the right size to include them in the cost
        std::wstring right_edges;

        if(i_max + 1 < this->m_network_2d.dimension) {
            // Upper rank edge
            wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[0][i_max], this->m_node_ids[0][i_max + 1]}];
            right_edges += symbol;
            weights[symbol] = this->m_weights[symbol];
        }

        if(j_max + 1 < this->m_network_2d.dimension) {
            // Lower rank edge
            wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[1][j_max], this->m_node_ids[1][j_max + 1]}];
            right_edges += symbol;
            weights[symbol] = this->m_weights[symbol];
        }

        if(i_max != j_max) {
            // Mode edges
            for(int i = min(i_max, j_max) + 1; i <= max(i_max, j_max); i++) {
                wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[0][i], this->m_node_ids[1][i]}];
                right_edges += symbol;
                weights[symbol] = this->m_weights[symbol];
            }
        }

        if(right_edges != L"") {
            if(direction == LR) {
                output += right_edges;
            } else {
                input.push_back(right_edges);
                input_node_included = true;
            }
        }
    
        // Set the upper left/right, middle and lower left/right indices
        //   |
        // --O--
        //   |
        int move_x[] = {0, 1, 0, -1};
        int move_y[] = {1, 0, -1, 0};

        for(int i = 0; i < tt_dim; i++) {
            const int dim_min = (i == 0) ? i_min : j_min;
            const int dim_max = (i == 0) ? i_max : j_max;
            for(int j = dim_min; j <= dim_max; j++) {
                std::wstring edges;
                for(int k = 0; k < 4; k++) {
                    int x = i + move_x[k];
                    int y = j + move_y[k];
                    if((x == 0 && y >= i_min && y <= i_max) || (x == 1 && y >= j_min && y >= j_max)) {
                        wchar_t symbol = this->m_edge_symbols[{this->m_node_ids[i][j], this->m_node_ids[x][y]}];
                        edges += symbol;
                        weights[symbol] = this->m_weights[symbol];
                    }
                }
                input.push_back(edges);
            }
        }
    }

    public:
    // Constructors
    CotengraWrapper(){}
    CotengraWrapper(std::map<std::string, std::any> param_dictionary) : Algorithm(param_dictionary){
        // Set the algorithm Cotengra algorithm to use
        if(param_dictionary.find("ctg_algorithm") != param_dictionary.end()) {
            std::cout<<"Cotengra using algorithm "<<std::any_cast<std::string>(param_dictionary["ctg_algorithm"])<<std::endl;
            this->algorithm = std::any_cast<std::string>(param_dictionary["ctg_algorithm"]);
        }
    }

    // Initializers
    void init(Network& network) {
        // Initialize network 2D
        this->m_network_2d = Network2D<tt_dim>(network.m_filename);

        // Initialize wrapper using the network
        this->init(this->m_network_2d);
    }

    void init(Network2D<tt_dim>& network) {
        // Initialize network
        this->m_network_2d = network;

        // Generate node ids for each row and column
        for(int row = 0; row < tt_dim; row++) {
            for(int column = 0; column < this->m_network_2d.dimension; column++) {
                this->m_node_ids[row].push_back(row * this->m_network_2d.dimension + column);
            }
        }

        // Generate symbol for each edge
        int move_x[] = {0, 1, 0, -1};
        int move_y[] = {1, 0, -1, 0};

        int symbol_count = 0;
        for(int row = 0; row < tt_dim; row++) {
            for(int column = 0; column < this->m_network_2d.dimension; column++) {
                for(int i = 0; i < 4; i++) {
                    int x = row + move_x[i];
                    int y = column + move_y[i];

                    if(x >= 0 && x < tt_dim && y >= 0 && y < this->m_network_2d.dimension && this->m_edge_symbols.count({this->m_node_ids[row][column], this->m_node_ids[x][y]}) == 0) {
                        wchar_t symbol = get_symbol(symbol_count);
                        this->m_edge_symbols[{this->m_node_ids[row][column], this->m_node_ids[x][y]}] = symbol;
                        this->m_edge_symbols[{this->m_node_ids[x][y], this->m_node_ids[row][column]}] = symbol;
                        this->m_weights_map[symbol] = this->m_network_2d[this->m_node_ids[row][column], this->m_node_ids[x][y], true];
                        symbol_count++;
                    }
                }
            }
        }

        // Generate list of edge weights to speed up the lookup during the
        // subproblem generation (to avoid using the map)
        wchar_t max_symbol_id = this->m_weights_map.rbegin()->first;
        this->m_weights.resize(max_symbol_id + 1, -1);
        for(auto& [symbol, weight] : this->m_weights_map) {
            this->m_weights[symbol] = weight;
        }

        // Initialize Python interpreter
        py::initialize_interpreter();

        // Initialize the result variable
        best_cost = numeric_limits<cost_t>::max();
    }

    // Solvers
    cost_t call_solve() {
        auto result = solve(0, this->m_network_2d.dimension - 1, LR);
        this->best_cost = result.first;
        this->best_order_str = result.second;

        return this->best_cost;
    }

    std::pair<cost_t, std::string> solve(const int dim_min, const int dim_max, const result_direction_e direction) {
        // Prepare input, output, sizes_dict arguments for calling Cotengra on
        // requested subpart of the network
        std::vector<std::wstring> inputs;
        std::wstring output;
        std::map<wchar_t, cost_t> weights;
        bool input_node_included = false;
        generate_subproblem(dim_min, dim_max, direction, inputs, output, weights, input_node_included);

        // Call the Cotengra wrapper script to solve the subpart of the network
        // using the optimal algorithm from the Cotengra library
        int dim = this->m_network_2d.dimension;

        auto python_script = py::module::import("cotengra_wrapper");
        auto resultobj = python_script.attr("cotengra_wrapper_solve_with_args")(this->algorithm, inputs, output, weights, dim, dim_min, dim_max, input_node_included);
        auto result = resultobj.cast<py::tuple>();

        return make_pair(result[0].cast<cost_t>(), result[1].cast<std::string>());
    }

    std::pair<cost_t, std::string> solve(const int i_min, const int j_min, const int i_max, const int j_max, const result_direction_e direction) {
        // Prepare input, output, sizes_dict arguments for calling Cotengra on
        // requested subpart of the network
        std::vector<std::wstring> inputs;
        std::wstring output;
        std::map<wchar_t, cost_t> weights;
        bool input_node_included = false;
        generate_subproblem(i_min, j_min, i_max, j_max, direction, inputs, output, weights, input_node_included);

        // Call the Cotengra wrapper script to solve the subpart of the network
        // using the optimal algorithm from the Cotengra library
        int dim = this->m_network_2d.dimension;

        auto python_script = py::module::import("cotengra_wrapper");
        auto resultobj = python_script.attr("cotengra_wrapper_solve_ij")(this->algorithm, inputs, output, weights, dim, i_min, j_min, i_max, j_max, input_node_included);
        auto result = resultobj.cast<py::tuple>();

        return make_pair(result[0].cast<cost_t>(), result[1].cast<std::string>());
    }
};

#endif