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
#include <tuple>



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

    const vector<uint8_t> coilsWithResistance = {0, 1, 2, 3, 4, 5, 10}; ///<  cs3u,cs2u,cs1,cs2l,cs3l.pf1,pf6 (катушки с внешним сопротивлением)
    const vector<std::tuple<double,double>> bdPointsCoordinates = {
        {5.65,0},
        {7.25,0},
        {4.05,0},
        {5.65,0.20},
        {5.65,-0.20}
    };
    /*dr=20  dz=20
     565	0	2.5e-008
*/
    enum class WorkMode
    {
        noOptimization                   = 0,
        alternatingVariableDescentMethod = 1,
        particleSwarmMethod              = 2,
        methodOfSteepestDescent          = 3
    };

    enum class IsBreakdown
    {
        no  = 0, ///< пробоя не было
        yes = 100 ///< пробой состоялся
    };
    enum class IsRequirements
    {
        yes   = 110, ///< ограничения и требования налагаемые на модель выполены
        no    = 0 ///< физические и технологические ограничения и требования были нарушены
    };
    enum FuncIdx{
        idxTotal                            = 0, ///< общий функционал (равный сумме всех штрафных функций)
        idxLoopVoltageBeforeBD              = 1, ///< значений штрафной функции для напряжения на обходе до пробоя
        idxRFields                          = 2, ///< значений штрафной функции для радиальной компоненты маг.поля на всем этапе моделирования
        idxZFieldsBeforeBD                  = 3, ///< значений штрафной функции для вертикальной компоненты маг.поля до пробоя
        idxMaxFluxBeforeBD                  = 4, ///< значений штрафной функции для маг.потока в центре области пробоя до пробоя
        idxLoopVoltageDerivativeBeforeBD    = 5, ///< значений штрафной функции для производной напряжения на обходе до пробоя
        idxMaxCurrents                      = 6, ///< значений штрафной функции для максимальных значений токов в полоидальных катушках
        idxMaxVoltages                      = 7, ///< значений штрафной функции для максимальных значений напряжений в полоидальных катушках
        idxMaxResVoltages                   = 8, ///< значений штрафной функции для резистивных напряжений в полоидальных катушках
        idxZFieldsAfterBD                   = 9, ///< значений штрафной функции для вертикальной компоненты маг.поля в контрольных точках после пробоя
        count                               = 10
    };

#ifdef Q_OS_WIN32
    const string pathForInputData                            = "input_data\\";
    const string pathForOutputData                           = "output_data\\";
#endif

#ifdef Q_OS_LINUX
    const string pathForInputData                            = "input_data/";
    const string pathForOutputData                           = "output_data/";
#endif
    const string fileNameForCurrentsResult              = "data_currents.txt";
    const string fileNameForDerivativeCurrentsResult    = "data_derivative_current.txt";
    const string fileNameForTestCurrents                = "j.dat";
    const string fileNameForInductance                  = "matr_ind_univ.dat";
    const string fileNameForResistance                  = "matr_res_univ.dat";
    const string fileNameForRequiredCurrentPlasma       = "Ip.dat";
    const string fileNameForAlfaZ                       = "bz_cache.dat";
    const string fileNameForInitialCurrents             = "I0.txt";
    const string fileNameForAlfaPsiAlfaRAlfaZ           = "matr_field_univ.dat";
    const vector<string> fileNameForCoils               = { "cs3u.txt",
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

    void matrixMultiplier(const vec_d&, const vec_d&, vec_d&, uint64_t, uint64_t);
    void matrixMultiplier(double&, const vec_d&, const vec_d&);

}

#endif // COMMON_H
