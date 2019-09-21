/*!
  \file
* В данном файле реализована работа с технологическими, физическими и другими требованиями и ограничениями на модель динамики полоидальных токов в токамаке ITER.
* Меодель представляет собой систему линейных дифф. уравнений 1 порядка составленых по правилу Кирхгофа.
* Результатом являются два контейнера. В одном хранятся значения полоидальных токов в контурах в каждый момент
* времени, в другом - производные этих токов.
*/

#ifndef FUNCTIONALS_H
#define FUNCTIONALS_H

//#include "src/read_data/read_data.h"
#include "../read_data/read_data.h"

namespace PlasmaLab {

    /*В классе происходит проверка условий пробоя и вычисление значений интегральных функционалов до Пробоя включительно*/
    class BeforeBD{
    protected:
      uint64_t system_size;                   ///размерность системы уравнений, до того как возникла плазма
      uint64_t number_coils; ///< кол-во полоидальных (управляющих) катушек
      IsRequirements requiments_key; ///< соблюдены ли технологические и физические ограничения на систему
      IsBreakdown bd_key; ///< ключ, был или не был пробой
      bool init_key; ///< ключ, если false, значит первая итерация в методе Рунге-Кутта
      double u_loop; ///< напряжение на обходе
      vec_d resistance; ///сопротивления в катушках
      vec_d r_fields; ///< значения радиальной компоненты магнитного поля в контрольных точках
      vec_d z_fields; ///< значения вертикальной компоненты магнитного поля в контрольных точках
      vec_d max_currents; ///< максимально допустимые токи в полоидальных катушках
      vec_d max_voltages; ///< максимально допустимые напряжения в полоидальных катушках с источником питания
      vec_d max_res_voltages; ///максимальные значения резистивных напряжений для полоидальных катушек, имеющих собственное сопротивление (Вольт)
      double r_field_max, ///< максимальное значения радиальной компоненты магнитного поля в каждой контрольной точке
             z_field_max, ///< максимальное значения вертикальной компоненты магнитного поля в каждой контрольной точке
             nessesary_u_loop; ///< необходимое для пробоя значение напряжения на обходе контура

      void calc_requiments(const vec_d&,vec_d&,uint64_t,const vvec_d&,double, const vec_d&);
    private:
      void calc_conditions_bd(const vec_d &weighting_factors,vec_d &functional_values,uint64_t point,const vvec_d &currents, const vvec_d &derivative_of_current, const vvec_d &alfa_psi,
                              const vvec_d &alfa_r,const vvec_d &alfa_z);
    public:
      BeforeBD() = delete;
      /*единственно возможный конструктор*/
      BeforeBD(const ReadData &rd);
      /*проверка условий пробоя, выполнены ли они.*/
      IsBreakdown run(const vec_d&,vec_d&,IsRequirements&,uint64_t,const vvec_d&, const vvec_d&,
                      const vvec_d&,const vvec_d&,const vvec_d&, const vec_d&);

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
        void calc_requiments_afterDB(const vec_d&,vec_d&,uint64_t,const vvec_d&,const vvec_d&,const vvec_d&);
    public:
        AfterBD() = delete;
        /*единственно возможный конструктор*/
        AfterBD(const ReadData &rd);
        IsBreakdown run(const vec_d&,vec_d&,IsRequirements&,uint64_t,const vvec_d&, const vvec_d&,
                        const vvec_d&,const vvec_d&,const vvec_d&, const vec_d&);
    };

    class FunctionalModel{

        const uint size_func_idx = FuncIdx::count; ///< общее кол-во штрахных функций +1 общий функионал суммы
        BeforeBD beforeBD; ///< модель функционала до пробоя
        AfterBD afterBD; ///< модель функционала после пробоя
        IsBreakdown bd_key; ///< был или не был пробой
        IsRequirements requirements_key; ///< соблюдены ли технологические и физические ограничения на систему
        vec_d weighting_factors; ///< веса для значений штрафных функций (функционалом)
        vec_d functional_values; ///< значения штрафных функций
    public:
        FunctionalModel() = delete;
        FunctionalModel(const ReadData &rd) : beforeBD(rd), afterBD(rd){
            bd_key = IsBreakdown::no;
        }

        IsBreakdown run(uint64_t,const vvec_d&, const vvec_d&, const vvec_d&,const vvec_d&,const vvec_d&,const vec_d&);

        //const BeforeBD &get_BeforeBD() const { return beforeBD; }
        IsBreakdown get_IsBreakdown() {return bd_key;}
        IsRequirements get_IsRequirements() {return requirements_key;}
        const vec_d & get_weighting_factors() const {return weighting_factors;}
        const vec_d & get_functional_values() const {return functional_values;}

    };
}

#endif // FUNCTIONALS_H
