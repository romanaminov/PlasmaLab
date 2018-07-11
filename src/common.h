/*!
  \file
* В данном файле собраны все константы, используемые в работе программы. Из названий переменных можно понять для чего они.
*/


#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <map>
#include <future>



namespace PlasmaLab {

    using std::string;
    using std::vector;
    using std::ifstream;
    using std::ofstream;
    using std::fstream;
    using std::cout;
    using std::cin;
    using std::endl;
    using std::ios;
    using std::pair;

    using vec_d  = vector<double>;
    using vvec_d = vector<vector<double>>;

    enum class WorkMode
    {
        no_optimization = 0,
        alternating_variable_descent_method = 1,
        particle_swarm_method = 2,
        method_of_steepest_descent = 3
    };

    enum class IsBreakdown
    {
        no  = 0, ///< пробоя не было
        yes = 100 ///< пробой состоялся
    };
    enum class IsRequirements
    {
      yes = 110, ///< ограничения и требования налагаемые на модель выполены
      no = 0 ///< физические и технологические ограничения и требования были нарушены
    };
    enum FuncIdx{
        idx_total = 0, ///< общий функционал (равный сумме всех штрафных функций)
        idx_loop_voltage_before_bd = 1, ///< значений штрафной функции для напряжения на обходе до пробоя
        idx_r_fields = 2, ///< значений штрафной функции для радиальной компоненты маг.поля на всем этапе моделирования
        idx_z_fields_before_bd = 3, ///< значений штрафной функции для вертикальной компоненты маг.поля до пробоя
        idx_mux_flux_before_bd = 4, ///< значений штрафной функции для маг.потока в центре области пробоя до пробоя
        idx_loop_voltage_derivative_before_bd = 5, ///< значений штрафной функции для производной напряжения на обходе до пробоя
        idx_max_currents = 6, ///< значений штрафной функции для максимальных значений токов в полоидальных катушках
        idx_max_voltages = 7, ///< значений штрафной функции для максимальных значений напряжений в полоидальных катушках
        idx_max_res_voltages = 8, ///< значений штрафной функции для резистивных напряжений в полоидальных катушках
        idx_z_fields_after_bd =9 ///< значений штрафной функции для вертикальной компоненты маг.поля в контрольных точках после пробоя
    };

#ifdef Q_OS_WIN32
    const string path_for_input_data                            = "input_data\\";
    const string path_for_output_data                           = "output_data\\";
#endif

#ifdef Q_OS_LINUX
    const string path_for_input_data                            = "input_data/";
    const string path_for_output_data                           = "output_data/";
#endif
    const string file_name_for_currents_result                  = "data_currents.txt";
    const string file_name_for_derivative_currents_result       = "data_derivative_current.txt";
    const string file_name_for_test_currents                    = "j.dat";
    const string file_name_for_inductance                       = "matr_ind_univ.dat";
    const string file_name_for_resistance                       = "matr_res_univ.dat";
    const string file_name_for_required_current_plasma          = "Ip.dat";
    const string file_name_for_alfa_z                           = "bz_cache.dat";
    const string file_name_for_initial_currents                 = "I0.txt";
    const string file_name_for_alfa_psi_alfa_r_alfa_z           = "matr_field_univ.dat";
    const vector<string> file_name_for_coils                    = { "cs3u.txt",
                                                                    "cs2u.txt",
                                                                    "cs1.txt",
                                                                    "cs2l.txt",
                                                                    "cs3l.txt",
                                                                    "pf1.txt",
                                                                    "pf2.txt",
                                                                    "pf3.txt",
                                                                    "pf4.txt",
                                                                    "pf5.txt",
                                                                    "pf6.txt"};

    void matrix_multiplier(const vec_d &, const vec_d &,vec_d &, int, int);
    void matrix_multiplier(double &, const vec_d &,const vec_d &);

}

#endif // COMMON_H
