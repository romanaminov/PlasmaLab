#include <src/write_data/write_data.h>

#include <QDir>

namespace  PlasmaLab {

    int WriteData::main_write(const string &path,const Model &model) const
    {
        string output_path = path + path_for_output_data;
        if( !QDir(output_path.c_str()).exists() )
            QDir().mkdir(output_path.c_str());

        double start_time = model.get_integration_step(),
               integration_step = model.get_integration_step();
        int system_size = model.get_system_size(),
            short_step = model.get_short_step();
        vvec_d currents = model.get_currents(),
               derivative_of_currents = model.get_derivative_of_currents();

        int res = 0;

        string res_path = output_path + file_name_for_currents_result;
        auto r1 = std::async(std::launch::async,&WriteData::write,this, res_path, currents, 0 , integration_step, system_size, short_step);
        string res_path_2 = output_path + file_name_for_derivative_currents_result;
        res |= write(res_path_2, derivative_of_currents, start_time , integration_step, system_size, short_step);

        res |= r1.get();

        return res;
    }

    int  WriteData::write(const string &file_name, const vvec_d &data,
                          double start_time, double integration_step, int system_size, int short_step) const
    {
        ofstream recording;
        recording.open(file_name);
        if(!recording)
            return -10;

        recording << "data" << endl;
        for(unsigned int i = 0;i < data.size();i += short_step)
        {
            recording << start_time << endl;
            for (int j = 0; j < system_size; ++j){
                recording << data[i][j] << endl;
            }
            start_time += (integration_step * short_step);
        }
        recording.close();

        return 0;
    }

}

