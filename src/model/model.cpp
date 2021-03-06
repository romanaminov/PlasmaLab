#include "model.h"

namespace  PlasmaLab {
    Model::Model(void)
    {
        breakdown_key               = IsBreakdown::no;
        work_mode                   = WorkMode::noOptimization;

        system_size                 = 0;
        coils_count                 = 0;
        short_step                  = 0;
        control_points_count        = 0;

        required_loop_voltage       = 0;
        r_field_max                 = 0;
        z_field_max                 = 0;
        epsilon                     = 0;
        integration_step            = 0;
        work_time                   = 0;
        breakdown_time              = 0;
        number_of_steps_in_method   = 0;

    }


    Model::~Model(void)
    {
    }

    int Model::slau_gauss(vec_d &back_inductances, vec_d inductances)//должны приходить вектора забитые нулями с определенной длинной
    {
        int length = static_cast<int>(std::pow(inductances.size(),0.5));
        vec_d identity_matrix(length*length,0);
        for (int i = 0; i < length; ++i)
            identity_matrix[length*i+i] = 1;
        //прямой ход метода Гаусса++++++++++++++++++++++++++++++++++++++++++++++++++++++
        double buf = 0;
        vec_d tmp_vec(length,0);
        for(int r = 0, i = 0;i < length; ++i)
        {
            r = i;
            if(inductances[length * i + i] == 0)
            {
                while(inductances[length * r + i] == 0)
                    ++r;
                for(int f = i;f < length;++f)
                {
                    tmp_vec[f] = inductances[length * i + f];
                    inductances[length * i + f] = inductances[length * r + f];
                    inductances[length * r + f] = tmp_vec[f];
                }
            }
            for(int j = i + 1;j < length;++j)
            {
                buf = -inductances[length * j + i] / inductances[length * i + i];
                for(int k = 0;k < length;++k)
                    identity_matrix[k * length + j] += identity_matrix[k * length + i] * buf;
                for(int m = i; m < length;++m)
                    inductances[length * j + m] += inductances[length * i + m] * buf;
            }
        }
        //обратный ход метода Гаусса+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        int t=2;
        for(int i = length - 1;i >= 0;--i)
        {
            for(int j = length - t;j >= 0;--j)
            {
                buf = -inductances[length * j + i] / inductances[length * i + i];
                for(int k = 0;k < length;++k)
                    identity_matrix[ k * length + j ] += identity_matrix[k * length + i] * buf;
                for(int m = length - 1; m >= i; --m)
                    inductances[length * j + m] += inductances[length * i + m] * buf;
            }
            ++t;
        }
        for (int column_number = 0; column_number < length; ++column_number){
            for (int i = 0; i < length; ++i)
                back_inductances[column_number + length * i] = identity_matrix[column_number * length + i] / inductances[i * length + i];
        }
        return 0;
    }

    int Model::runge_kutta_4(FunctionalModel &functionalModel){
        double_t time_step = 0.0,
                 plasma_current = 0.0;
        vec_d k1(system_size,0),
              k2(system_size,0),
              k3(system_size,0),
              k4(system_size,0);
        vec_d Y_t_1(system_size,0),
              Y_t_2(system_size,0),
              Y_t_3(system_size,0);
        breakdown_key = IsBreakdown::no;//пробоя НЕТ
        for (uint64_t j = 1; j < number_of_steps_in_method; ++j){	//почему  длинна цикла не больше +1
            time_step += integration_step;
            voltage_calculator(time_step);
            model_combined_equations(time_step, currents[j-1], k1,plasma_current);
            for (uint64_t i = 0; i < system_size; i++){
                derivative_of_current[j-1][i] = k1[i] / integration_step;
                Y_t_1[i] = 0.5 * k1[i] + currents[j-1][i];
            }
            voltage_calculator(time_step + integration_step * 0.5);
            model_combined_equations(time_step + integration_step * 0.5, Y_t_1, k2,plasma_current);
            for (uint64_t i = 0; i < system_size; i++)
                Y_t_2[i] = 0.5 * k2[i] + currents[j-1][i];
            model_combined_equations(time_step + integration_step * 0.5, Y_t_2, k3,plasma_current);//,plasma_current какое из четырех верное??????
            for (uint64_t i = 0; i < system_size; i++)
                Y_t_3[i] = k3[i] + currents[j-1][i];
            voltage_calculator(time_step + integration_step);
            model_combined_equations(time_step + integration_step, Y_t_3, k4,plasma_current);//,plasma_current видимо это???!!!
            for(uint64_t i=0; i < system_size; i++)
                currents[j][i] = currents[j-1][i] + (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]) / 6;

            if(breakdown_key != IsBreakdown::yes){
                breakdown_key =  functionalModel.run(j - 1,plasma_current,currents,derivative_of_current,alfa_psi,alfa_r,alfa_z,voltages_in_some_momente );//проверка на выполнение условий для пробой
                breakdown_time = j - 1;
                breakdown_time *= integration_step;//вычисляем время пробоя
                if(breakdown_key == IsBreakdown::yes)
                    cout << "breakdown_time = "<<breakdown_time<< endl;
            }else {
                breakdown_key =  functionalModel.run(j - 1,plasma_current,currents,derivative_of_current,alfa_psi,alfa_r,alfa_z,voltages_in_some_momente );//проверка на выполнение условий для пробой
                breakdown_time = j - 1;
                breakdown_time *= integration_step;//вычисляем время пробоя

            }
        }
        return 0;
    }

    int Model::parser_alfa_xxx(const ReadData &read_data){
        for(uint64_t i = 0;i < control_points_count;++i){
            alfa_r.push_back(vec_d(system_size,0));
            alfa_z.push_back(vec_d(system_size,0));
            alfa_psi.push_back(vec_d(system_size,0));
        }
        vec_d alfa_r_old(system_size * control_points_count, 0);
        vec_d alfa_z_old(system_size * control_points_count, 0);
        vec_d alfa_ps_old(system_size * control_points_count,0);

        read_data.get_alfa_r(alfa_r_old);
        read_data.get_alfa_z(alfa_z_old);
        read_data.get_alfa_psi(alfa_ps_old);
        for(uint64_t i=0;i<system_size;i++){
            for(uint64_t j=0;j<control_points_count;j++){
                alfa_r[j][i] = alfa_r_old[i + j * system_size];
                alfa_z[j][i] = alfa_z_old[i + j * system_size];
                alfa_psi[j][i] = alfa_ps_old[i + j * system_size];
            }
        }

        plasma_inductance_matrix.resize(alfa_ps_old.size());
        for(uint64_t i = 0;i < control_points_count;++i){//транспонирование матрицы - проверка показала все ОК
            for(uint64_t j = 0;j < system_size;++j)
                plasma_inductance_matrix[i + j * control_points_count] = alfa_ps_old[i * system_size + j];
        }

        return 0;
    }
    void Model::data_preparation(const ReadData &read_data)
    {
        system_size = read_data.get_system_size();

        auto r1 = std::async(std::launch::async,[&]
            {//создаем дополнительный поток
                read_data.get_inductance(inductance_matrix);
                inverse_inductance_matrix.resize(system_size*system_size);
                slau_gauss(inverse_inductance_matrix,inductance_matrix);

                number_of_steps_in_method = read_data.get_work_time() / read_data.get_integration_step() + 1;//количество итераций в рунге-кутта 4
                currents.resize(static_cast<uint64_t>(number_of_steps_in_method));
                for(uint64_t i = 0; i < currents.size(); ++i )
                    currents[i].resize(system_size, 0);
                derivative_of_current.resize(currents.size() - 1);
                for (uint64_t i = 0; i < derivative_of_current.size(); ++i)
                    derivative_of_current[i].resize(system_size, 0);
                read_data.get_initial_currents(currents[0]);
            });

        r_field_max = read_data.get_r_field_max();
        z_field_max = read_data.get_z_field_max();
        required_loop_voltage = read_data.get_required_loop_voltage();
        coils_count = read_data.get_coils_count();
        control_points_count = read_data.get_control_points();
        work_time = read_data.get_work_time();
        work_mode = static_cast<WorkMode>(read_data.get_work_mode());
        short_step = read_data.get_short_step();
        epsilon = read_data.get_epsilon();

        read_data.get_voltage_in_coils(voltage_in_coils);
        read_data.get_currents_max(currents_max);
        read_data.get_voltages_max(voltages_max);
        read_data.get_resistance(resistance_matrix);
        read_data.get_mag_fields_max(mag_fields_max);
        read_data.get_required_current_plasma(required_current_plasma);

        voltages_in_some_momente = vec_d(system_size, 0);

        parser_alfa_xxx(read_data);//подготовить вектора alfa_xxx для 5ти контрольных точек и матрицу взаимных индуктивностей плазмы с др.контурами

        inverse_L_on_R_matrix.resize(system_size * system_size, 0);//матрица от умножения обратной матрицы индуктивности на матрицу сопротивлений
        inverse_L_on_Mp_matrix.resize(control_points_count * system_size, 0);
        integration_step = read_data.get_integration_step();

        r1.get();
        matrixMultiplier(inverse_inductance_matrix,resistance_matrix,inverse_L_on_R_matrix,system_size,system_size);
        matrixMultiplier(inverse_inductance_matrix, plasma_inductance_matrix, inverse_L_on_Mp_matrix,system_size,control_points_count);
    }

    int Model::main_function(const ReadData& read_data, FunctionalModel &functionalModel){


        data_preparation(read_data);

        runge_kutta_4(functionalModel);

        return 0;
    }    
    void Model::voltage_calculator(double time_step)
    {
        for(uint64_t i = 0; i < coils_count; ++i)
            voltages_in_some_momente[i] = 0;
        uint64_t number = 0;
        bool inclusion = false;
        for(uint64_t i = 1;i <= coils_count;++i){
            number = time_comparison(inclusion, time_step, voltage_in_coils[i-1]);
            if (!inclusion)
                voltages_in_some_momente[i - 1] = dependence_U_on_T(voltage_in_coils[i - 1][number], voltage_in_coils[i-1][number + 2],
                                                voltage_in_coils[i - 1][number + 1], voltage_in_coils[i - 1][number + 3], time_step);
            else
                voltages_in_some_momente[i - 1] = voltage_in_coils[i - 1][number + 1];
        }
    }

    double Model::dependence_U_on_T(double t1, double t2, double u1, double u2, double t){
        double u, a, b;
        a = (u2 - u1) / (t2 - t1);
        b = u1 - a * t1;
        u = a * t + b;
        return u;
    }
    uint64_t Model::time_comparison(bool &inclusion, double time_step,const vec_d & sub_pf){
        uint64_t number = 0;
        for (uint64_t i = 0; i < sub_pf.size(); i = i + 2)
        {
            if (sub_pf[i] < time_step){
                number = i;
                inclusion = false;
            }
            if (::fabs(time_step - sub_pf[i]) <= integration_step){
                number = i;
                inclusion = true;
            }
            if (sub_pf[i] > time_step)
                i = sub_pf.size();
        }
        return number;
    }
    void Model::model_combined_equations(double time_step, vec_d &current_in_some_momente,vec_d &KN, double_t& plasma_current){
        double_t tmp1 = 0.0, tmp2 = 0.0, k = 0.0;
        if(breakdown_key == IsBreakdown::yes){
            auto val = law_of_plasma_current(time_step);
            k = std::get<0>(val);
            plasma_current = std::get<1>(val);
        }else
            plasma_current = 0.0;

        for (uint64_t i = 0; i < system_size; ++i){
            tmp1 = 0; tmp2 = 0;
            for (uint64_t j = 0; j < system_size; ++j)
                tmp1 = tmp1 + inverse_L_on_R_matrix[i * system_size + j] * current_in_some_momente[j];

            for (uint64_t j = 0; j < system_size; ++j)
                tmp2 = tmp2 + inverse_inductance_matrix[i * system_size + j] * voltages_in_some_momente[j];

            tmp1 = tmp2 - tmp1;
            if(breakdown_key == IsBreakdown::yes){
                tmp2 = 0;
                for (uint64_t j = 0; j < control_points_count; ++j)
                    tmp2 = tmp2 + inverse_L_on_Mp_matrix[i * control_points_count + j];
                tmp2 *= k;
                tmp1 = tmp1 - tmp2;
            }
            KN[i] = integration_step * tmp1;
        }
    }
    std::tuple<double_t,double_t> Model::law_of_plasma_current(double current_time)
    {
       /*double k=0;
        if(current_time <= 1.1805)
            k = (71971.224 - 0)/(1.1805 - 1.1268);
        else if(current_time <= 1.2341)
            k = (133149.7 - 71971.224)/(1.2341 - 1.1805);
        else if(current_time <= 1.2878)
            k = (185931.58 - 133149.7)/(1.2878 - 1.2341);
        else if(current_time <= 1.3415)
            k = (231930.25 - 185931.58)/(1.3415 - 1.2878);
        else if(current_time <= 1.3951)
            k = (273716.56 - 231930.25)/(1.3951 - 1.3415);
        else if(current_time <= 1.4488)
            k = (313873.73- 273716.56)/(1.4488- 1.3951);
        else if(current_time <= 1.5024)
            k = (353530.87 - 313873.73)/(1.5024 - 1.4488);
        else if(current_time <= 1.5561)
            k = (392298.85 - 353530.87)/(1.5561 - 1.5024);
        else if(current_time <= 1.6098)
            k = (429749.42 - 392298.85)/(1.6098 - 1.5561);
        else if(current_time <= 1.6634)
            k = (466027.29 - 429749.42)/(1.6634 - 1.6098);

        k=k/5;
        return k;*/
        double t = 0.0,
               k = 0.0,
               plasma_current = 0.0,
               b = 0.0;
        if(required_current_plasma.empty())
            cout << "ERROR: current plasma file is empty!\n";
        t = required_current_plasma[0];
        for(uint64_t i = 3; i <= required_current_plasma.size(); i += 2 )
        {
            t = required_current_plasma[i - 1];
            if(current_time <= t){
                k = (required_current_plasma[i] - required_current_plasma[i - 2]);
                k /= (required_current_plasma[i - 1] - required_current_plasma[i - 3]);

                b = required_current_plasma[i] - required_current_plasma[i -1] * k;
                plasma_current = k * current_time + b;
                break;
            }
        }
        k = k / control_points_count; //параллельное соединение контуров
        plasma_current = plasma_current / control_points_count;
        return std::make_tuple (k, plasma_current);// возвращаем К, т.к. нам нужна производная тока плазмы и само значение тока плазмы
    }
}
