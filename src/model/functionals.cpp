#include "functionals.h"

namespace  PlasmaLab {
    BeforeBD::BeforeBD(const ReadData &rd) :
        number_coils(rd.get_coils_count()),
        requiments_key(IsRequirements::no),
        bd_key(IsBreakdown::no),
        init_key(false),
        u_loop(0),
        r_field_max(rd.get_r_field_max()),
        z_field_max(rd.get_z_field_max()),
        nessesary_u_loop(rd.get_required_loop_voltage())
    {
        for(uint64_t i=0;i < rd.get_control_points();++i){
            r_fields.push_back(0);
            z_fields.push_back(0);
        }
        rd.get_currents_max(max_currents);
        rd.get_voltages_max(max_voltages);
        rd.get_res_voltages_max(max_res_voltages);
        rd.get_resistance(resistance);
        system_size = rd.get_system_size();
    }
    IsBreakdown BeforeBD::run(const vec_d& weighting_factors,
                              vec_d& functional_values,
                              IsRequirements& out_requiments_key,
                              uint64_t      point,
                              const vvec_d& currents,
                              const vvec_d& derivative_of_current,
                              const vvec_d& alfa_psi,
                              const vvec_d& alfa_r,
                              const vvec_d& alfa_z,
                              const vec_d&  voltages_in_some_momente)
    {
        double prev_u_loop = u_loop;
        calc_conditions_bd(weighting_factors,functional_values,point,currents,derivative_of_current,alfa_psi,alfa_r,alfa_z);

        calc_requiments(weighting_factors,functional_values, point,currents, prev_u_loop,voltages_in_some_momente);

        out_requiments_key = requiments_key;

        return bd_key;
    }

    void BeforeBD::calc_conditions_bd(const vec_d &weighting_factors,
                                      vec_d &functional_values,
                                      uint64_t point,
                                      const vvec_d &currents,
                                      const vvec_d &derivative_of_current,
                                      const vvec_d &alfa_psi,
                                      const vvec_d &alfa_r,
                                      const vvec_d &alfa_z)
    {
        bd_key = IsBreakdown::yes;
        matrixMultiplier(u_loop,derivative_of_current[point], alfa_psi[0]);//напряжение на обходе
        for(uint i = 0;i < r_fields.size(); ++i){ //находим магнитное поле в контрольных точках
            matrixMultiplier(r_fields[i],currents[point], alfa_r[i]);
            matrixMultiplier(z_fields[i],currents[point], alfa_z[i]);
        }
        for(uint i = 0; i < r_fields.size(); ++i){
            if(fabs(r_fields[i]) >= r_field_max)
                bd_key =  IsBreakdown::no;//если радиальная компонента маг.поля больше МАХ
            if(fabs(z_fields[i]) >= z_field_max)
                bd_key =  IsBreakdown::no;//если вертикальная компонента маг.поля больше МАХ
        }
        if( fabs(u_loop) > (nessesary_u_loop+0.2) || fabs(u_loop) < (nessesary_u_loop - 0.2) )
            bd_key =  IsBreakdown::no; //если напряжение на обходе не равно заданному значению (+/- дельта)


    }

    void BeforeBD::calc_requiments(const vec_d &weighting_factors,
                                   vec_d &functional_values,
                                   uint64_t point,
                                   const vvec_d &currents,
                                   double prev_u_loop,
                                   const vec_d&  voltages_in_some_momente){
        if(bd_key == IsBreakdown::no && init_key == false)
            prev_u_loop = u_loop;//необходимо для вычисления производной напряжения на обходе

        if(bd_key == IsBreakdown::no && prev_u_loop > u_loop){ //значит производная напряжения на обходе < 0, что не допустимо
            requiments_key = IsRequirements::no;
            cout << "ERROR: prev_u_loop > u_loop\n";
        }

        for(uint i = 0; i < number_coils; ++i){
            //токи в катушках должны быть не больше максимальных значений
            if( (currents[point][i] < max_currents[i])&&(currents[point][i] > -max_currents[i]) )
                requiments_key = IsRequirements::yes;
            else{
                requiments_key = IsRequirements::no;
                cout << "ERROR: currents in active coils is more max values\n";
            }
            //напряжения в катушках должны быть не больше максимальных значений
            if( (voltages_in_some_momente[i] < max_voltages[i])&&(voltages_in_some_momente[i] > -max_voltages[i]) )
                requiments_key = IsRequirements::yes;
            else{
                requiments_key = IsRequirements::no;
                cout << "ERROR: voltages in active coils is more max values\n";
            }
        }
        for(uint64_t i = 0;i < coilsWithResistance.size(); i++){
        //максимальные значения резистивных напряжений для полоидальных катушек не должны превышать максимума
        uint64_t j = coilsWithResistance[i];
            double val = voltages_in_some_momente[j] * resistance[system_size*j+ j];
            if( (val < max_res_voltages[i])&&(val > -max_res_voltages[i]) )
                requiments_key = IsRequirements::yes;
            else{
                requiments_key = IsRequirements::no;
                cout << "ERROR: resistive voltage in active coils is more max values\n";
            }
        }
        //if(prev_u_loop == 0.0)
          //  cout << "=====\n";
    }

    AfterBD::AfterBD(const ReadData &rd) : BeforeBD (rd){}

    void AfterBD::calc_requiments_afterDB(const vec_d &weighting_factors,
                                          vec_d &functional_values,
                                          uint64_t point,
                                          double_t plasmaCurrent,
                                          const vvec_d &currents,
                                          const vvec_d &derivative_of_current,
                                          const vvec_d &alfa_r,
                                          const vvec_d &alfa_z){
        requiments_key =  IsRequirements::yes;
        for(uint i = 0;i < bdPointsCoordinates.size(); ++i){ //находим магнитное поле в контрольных точках
            matrixMultiplier(z_fields[i],currents[point], alfa_z[i]);
            matrixMultiplier(r_fields[i],currents[point], alfa_r[i]);
        }
        auto sum = 0.0;
        for(uint i = 0; i < bdPointsCoordinates.size(); ++i) {
            if(fabs(z_fields[i]) >= z_field_max)
                requiments_key =  IsRequirements::no;//если вертикальная компонента маг. поля больше МАХ
            sum += fabs(z_fields[i]);
        }
        auto shafranov = [](auto plasma, auto R){
            const double_t betaP = 0.2,
                           li = 0.85,
                           a = 1.6,
                           m_0  = 4* M_PI * std::pow(10,-7); //1.25663706 * std::pow(10,-6);
            return (- m_0 * plasma / (/*4 **/ M_PI * R) * ( log(8*R/a) + betaP + li/2 - 3/2 ));
            //нихрена не понял почему без множителя "4" все работатет. Шо за пиздец!
        };
        sum /= 5;
        auto shafranovField = fabs(shafranov(plasmaCurrent, std::get<0>(bdPointsCoordinates[0])));
        if( fabs(sum - shafranovField) > 0.002 ) {
            requiments_key =  IsRequirements::no;
            cout << "error: shapfranov = "<< fabs(shafranovField) << ", ness = " << sum << endl;
        }
    }


    IsRequirements AfterBD::run(const vec_d& weighting_factors,
                             vec_d& functional_values,
                             uint64_t      point,
                             double_t plasma_current,
                             const vvec_d& currents,
                             const vvec_d& derivative_of_current,
                             const vvec_d& alfa_psi,
                             const vvec_d& alfa_r,
                             const vvec_d& alfa_z,
                             const vec_d&  voltages_in_some_momente)
    {
        calc_requiments(weighting_factors,functional_values, point,currents, 0,voltages_in_some_momente);

        calc_requiments_afterDB(weighting_factors,functional_values, point,plasma_current,currents, derivative_of_current,alfa_r,alfa_z);

        return requiments_key;
    }

    IsBreakdown FunctionalModel::run(uint64_t point,
                              double_t plasma_current,
                              const vvec_d& currents,
                              const vvec_d& derivative_of_current,
                              const vvec_d& alfa_psi,
                              const vvec_d& alfa_r,
                              const vvec_d& alfa_z,
                              const vec_d&  voltages_in_some_momente)
    {
        if(bd_key == IsBreakdown::no){
             //cout << "nn" << endl;
            bd_key = beforeBD.run(weighting_factors,functional_values,requirements_key,point,
                                  currents,derivative_of_current, alfa_psi,alfa_r,alfa_z,voltages_in_some_momente);
        }else{
            requirements_key = afterBD.run(weighting_factors,functional_values,point, plasma_current,
                                  currents,derivative_of_current, alfa_psi,alfa_r,alfa_z,voltages_in_some_momente);
            bd_key = IsBreakdown::yes;
        }
        return bd_key;
    }

}
