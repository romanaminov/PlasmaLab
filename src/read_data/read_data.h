/*!
  \file
* В данном файле происходит загрузка из файлов необходимых для работы программы данных.
*/
#ifndef READ_DATA_H
#define READ_DATA_H

#include <QString>
#include "src/common.h"

namespace PlasmaLab {

/*!
     * \brief The TypeVariable enum
     */
    enum class TypeVariable
    {
        type_double = 0,
        type_string = 1
    };
    /*!
     * \brief The ReadData class
     *
     * Класс для считываения начальных данных. Все файлы имеют специфическую структура, потому менять что-либо в методах
     * класса не рекомендуется.
     */
    class ReadData{
        int system_size;                        ///размерность системы уравнений, до того как возникла плазма
        int coils_count;                        ///количество управляющих катушек
        int short_step;                         ///чтобы не грузить систему, полученные данные записываем в файл с шагом short_step умноженное на integration_step
        int control_points_count;               ///количество контрольных точек на контуре плазмы
        int work_mode;                          ///режим работы программы

        double required_loop_voltage;           ///необходимое для пробоя плазмы напряжение на обходе
        double r_field_max, z_field_max;        ///максимальные значения радиальной и вертикальной составляющих магнит. поля в контрольных точках в момент пробоя
        double epsilon;                         ///погрешность вычислений
        double integration_step;                ///шаг интегрирования в методе рунге-кутта 4 порядка
        double work_time;                       ///время интегрирования системы (моделирования)

        vvec_d voltage_in_coils;                ///напряжения в управляющих катушках  на протяжении работы системы (кусочно-линейная функция)
        vvec_d test_currents_in_coils;          ///токи в управляющих катушках  на протяжении работы системы (проверочные значения из файла j.dat)

        vec_d voltages_max;                     ///максимальные значения напряжений в полоидальных(управляющих) катушках  на протяжении работы системы
        vec_d currents_max;                     ///максимальные значения токов в полоидальных(управляющих) катушках  на протяжении работы системы
        vec_d mag_fields_max;                   ///максимальные значения магнитных полей в полоидальных(управляющих) катушках на протяжении работы системы
        vec_d weight_coefficient;               ///весовые коэффициенты для функционалов
        vec_d required_current_plasma;          ///заданный режим поднятия тока плазмы
        vec_d inductance;                       ///индуктивности в контурах
        vec_d resistance;                       ///сопротивления в катушках
        vec_d initial_currents;                 ///начальные токи во внешних катушках
        vec_d alfa_r;                           ///константные коэффициенты, необходимые для расчета радиальных компонент магнитного поля
        vec_d alfa_z;                           ///константные коэффициенты, необходимые для расчета вертикальных компонент магнитного поля
        vec_d alfa_psi;                         ///константные коэффициенты, необходимые для расчета магнитного потока в центре области пробоя

    public:
        ReadData(void);
        ~ReadData(void);
    private:
    /*!
     * функции для загрузки и обработки данных из файлов
     */
        inline int load_voltage_in_coils(const string & );
        inline int load_inductance(const string &,const string &);
        inline int load_resistance(const string &,const string &);
        inline int load_initial_currents(const string &,const string &);
        inline int load_required_current_plasma(const string &, const string &);
        inline int load_one_coil(const string &, const string &, int);
        inline int load_alfa_xxx(const string &, const string &);
        inline void load_alfa_one(ifstream&, vec_d&);
        inline int load_test_currents_in_coils(const string &, const string &);//загрузка проверочных значений для токов в катушках
    public:
        /*!
        Основная функция данного класса. В ней происходит инициализация объекта
        \param[in] const string & Путь до конфигурационного файла
        */
        int load_data(const string & );
        /*!
        Возвращет кусочно-линейные функции напряжений в управляющих катушках
        \param[out] vvec_d &vvec вектор, в который записываются данные значения
        */
        inline void get_voltage_in_coils(vvec_d& vvec) const{                  vvec = voltage_in_coils;}
        /*!
        Возвращет проверочные значения полоидальных токов.
        \param[out] vvec_d &vvec вектор, в который записываются данные значения
        */
        inline void get_test_currents_in_coils(vvec_d& vvec) const{            vvec = test_currents_in_coils;}
        /*!
        Возвращет максимальные значения допустимых токов в полоидальных катушках.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_currents_max(vec_d& vec) const{                        vec = currents_max; }
        /*!
        Возвращет максимальные значения допустимых напряжений в полоидальных катушках.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_voltages_max(vec_d& vec) const{                        vec = voltages_max; }
        /*!
        Возвращет максимальные значения допустимых магнитных полей в полоидальных катушках.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_mag_fields_max(vec_d& vec) const{                      vec = mag_fields_max; }
        /*!
        Возвращет весовые коэффициенты функционала, используются в режиме оптимизации.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_weight_coefficient(vec_d& vec) const{                  vec = weight_coefficient; }
        /*!
        Возвращет сопротивления в контурах.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_resistance(vec_d& vec) const{                          vec = resistance;}
        /*!
        Возвращет матрицу собственных и взаимных индуктивностей контуров системы. Без учета плазменного шнура.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_inductance(vec_d& vec) const{                          vec = inductance;}
        /*!
        Возвращет начальные токи в полоидальных катушках.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_initial_currents(vec_d& vec) const{                    vec = initial_currents;}
        /*!
        Возвращет заранее посчитанные коэффициенты.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_alfa_psi(vec_d& vec) const{                            vec = alfa_psi;}
        /*!
        Возвращет заранее посчитанные коэффициенты.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_alfa_r(vec_d& vec) const{                              vec = alfa_r;}
        /*!
        Возвращет заранее посчитанные коэффициенты.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_alfa_z(vec_d& vec) const{                              vec = alfa_z;}
        /*!
        Возвращет кусочно-линейную функцию тока плазмы. В качестве времени используются приращения от предыдущего момента времени.
        \param[out] vec_d &vec вектор, в который записываются данные значения
        */
        inline void get_required_current_plasma(vec_d& vec) const{             vec = required_current_plasma;}
        /*!
        Возвращет время моделирования системы
        \result double work_time
        */
        inline double get_work_time() const{                                   return work_time;}
        /*!
        Возвращет шаг интегрированя системы линейных дифф. уравнений в методе Рунге-Кутта 4 порядка точности
        \result double integration_step
        */
        inline double get_integration_step() const{                            return integration_step;}
        /*!
        Возвращет погрешность вычислений
        \result double epsilon
        */
        inline double get_epsilon() const{                                     return epsilon; }
        /*!
        Возвращет максимальные значения радиальной (в полярной системе координат) составляющей магнит. поля в контрольных точках в момент пробоя
        \result double r_field_max
        */
        inline double get_r_field_max() const{                                 return r_field_max; }
        /*!
        Возвращет максимальные значения вертикальной (в полярной системе координат) составляющей магнит. поля в контрольных точках в момент пробоя
        \result double z_field_max
        */
        inline double get_z_field_max() const{                                 return z_field_max; }
        /*!
        Возвращет необходимое для пробоя напряжение на обходе.
        \result double required_loop_voltage
        */
        inline double get_required_loop_voltage() const{                       return required_loop_voltage; }
        /*!
        Возвращет тип режима работы программы (моделирование либо метод оптимизации)
        \result int work_mode
        */
        inline int get_work_mode() const{                                      return work_mode; }
        /*!
        Возвращет размерность решаемой системы линейных дифф. уравнений 1го порядка (а также кол-во контуров в системе)
        \result int system_size
        */
        inline int get_system_size() const{                                    return system_size;}
        /*!
        чтобы не грузить систему, полученные данные записываем в файл с шагом short_step умноженное на integration_step
        \result int short_step
        */
        inline int get_short_step() const{                                     return short_step; }
        /*!
        Возвращает кол-во контрольных точек в плазменном шнуре.
        \result int control_points_count
        */
        inline int get_control_points() const{                                 return control_points_count; }
        /*!
        Возвращает кол-во полоидальных катушек.
        \result int coils_count
        */
        inline int get_coils_count() const{                                    return coils_count; }
        /*!
        Изменяет начальный ток в какой-либо управляющей катушке.
        \param[in] int, double Номер катушки и добавляемое значение.
        \result int 0, если все хорошо.
        */
        inline int change_initial_current_for_coil(int, double);
        /*!
        Изменяет сопротивление в каком-либо контуре.
        \param[in] int, double Номер контура и добавляемое значение.
        \result int 0, если все хорошо.
        */
        inline int change_resistance_for_coil(int, double);
        /*!
        Изменяет напряжение в какой-либо управляющей катушке.
        \param[in] int, double Номер катушки и добавляемое значение.
        \result int 0, если все хорошо.
        */
        inline int change_voltage_for_coil(int, int, double);

    //функции-утилиты
        inline int text_parser(const string &, TypeVariable, string&, double&);
        inline int data_clear();
    };
}
#endif // READ_DATA_H
