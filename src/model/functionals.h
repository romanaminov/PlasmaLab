/*!
  \file
* В данном файле реализована работа с технологическими, физическими и другими требованиями и ограничениями на модель динамики полоидальных токов в токамаке ITER.
* Меодель представляет собой систему линейных дифф. уравнений 1 порядка составленых по правилу Кирхгофа.
* Результатом являются два контейнера. В одном хранятся значения полоидальных токов в контурах в каждый момент
* времени, в другом - производные этих токов.
*/

#ifndef FUNCTIONALS_H
#define FUNCTIONALS_H

#include "src/read_data/read_data.h"

namespace PlasmaLab {

    /*В классе происходит проверка условий пробоя и вычисление значений интегральных функционалов до Пробоя включительно*/
    class BeforeBD{
    protected:
      IsBreakdown bd_key; ///< ключ, были или не был пробой
      IsRequirements requiments_key; ///< соблюдены ли технологические и физические ограничения на систему
      bool init_key; ///< ключ, если false, значит первая итерация в методе Рунге-Кутта
      double u_loop; ///< напряжение на обходе
      vector<double> r_fields; ///< значения радиальной компоненты магнитного поля в контрольных точках
      vector<double> z_fields; ///< значения вертикальной компоненты магнитного поля в контрольных точках
      vec_d max_currents; ///< максимально допустимые токи в полоидальных катушках
      int number_coils; ///< кол-во полоидальных (управляющих) катушек
      double r_field_max, ///< максимальное значения радиальной компоненты магнитного поля в каждой контрольной точке
             z_field_max, ///< максимальное значения вертикальной компоненты магнитного поля в каждой контрольной точке
             nessesary_u_loop; ///< необходимое для пробоя значение напряжения на обходе контура

      void calc_requiments(const vec_d &weighting_factors,vec_d &functional_values,double prev_u_loop);
    private:
      void calc_conditions_bd(const vec_d &weighting_factors,vec_d &functional_values,int point,const vvec_d &currents, const vvec_d &derivative_of_current, const vvec_d &alfa_psi,
                              const vvec_d &alfa_r,const vvec_d &alfa_z);
    public:
      BeforeBD() = delete;
      /*единственно возможный конструктор*/
      BeforeBD(const ReadData &rd);
      /*проверка условий пробоя, выполнены ли они.*/
      IsBreakdown run(const vec_d &weighting_factors,vec_d &functional_values,IsRequirements &out_requiments_key,int point,const vvec_d &currents, const vvec_d &derivative_of_current, const vvec_d &alfa_psi,
                                  const vvec_d &alfa_r,const vvec_d &alfa_z);

      /*получить текущее значение напряжения на обходе*/
      double get_u_loop() const;
      /*получить текущее значение радиальной компоненты магнитного поля в контрольных точках*/
      double get_r_fields() const;
      /*получить текущее значение вертикальной компоненты магнитного поля в контрольных точках*/
      double get_z_fields() const;
      /*узнать был пробой или нет*/
      IsBreakdown get_bd_key() const;
      IsRequirements get_requiments_key() const;

    };


    class AfterBD : protected BeforeBD {
        IsBreakdown run(const vec_d &weighting_factors,vec_d &functional_values,IsRequirements &out_requiments_key,int point,const vvec_d &currents, const vvec_d &derivative_of_current, const vvec_d &alfa_psi,
                                    const vvec_d &alfa_r,const vvec_d &alfa_z);
    };

    class FunctionalModel{

        const uint size_func_idx = 10; ///< общее кол-во штрахных функций +1 общий функионал суммы
        BeforeBD beforeBD; ///< модель функционала до пробоя
        IsBreakdown bd_key; ///< был или не был пробой
        IsRequirements requirements_key; ///< соблюдены ли технологические и физические ограничения на систему
        vec_d weighting_factors; ///< веса для значений штрафных функций (функционалом)
        vec_d functional_values; ///< значения штрафных функций
    public:
        FunctionalModel() = delete;
        FunctionalModel(const ReadData &rd) : beforeBD(rd){}

        void run(const vec_d &weighting_factors,vec_d &functional_values,int point,const vvec_d &currents, const vvec_d &derivative_of_current, const vvec_d &alfa_psi,const vvec_d &alfa_r,const vvec_d &alfa_z);

        //const BeforeBD &get_BeforeBD() const { return beforeBD; }
        IsBreakdown get_IsBreakdown() {return bd_key;}
        IsRequirements get_IsRequirements() {return requirements_key;}
        const vec_d & get_weighting_factors() const {return weighting_factors;}
        const vec_d & get_functional_values() const {return functional_values;}

    };
}

#endif // FUNCTIONALS_H
