#include "src/read_data/read_data.h"


namespace PlasmaLab{

    ReadData::ReadData(void)
    {
        system_size = 0;
        coils_count = 0;
        short_step = 0;
        control_points_count = 0;
        work_mode = 0;
        required_loop_voltage = 0;
        r_field_max = 0;
        z_field_max = 0;
        epsilon = 0;
        integration_step = 0;
        work_time = 0;
    }

    ReadData::~ReadData(void)
    {
    }
    int ReadData::change_initial_current_for_coil(int idx, double data){
        initial_currents[idx] += data;
        return 0;
    }

    int ReadData::change_resistance_for_coil(int coil_number, double data){//нумерация катушек с 0!!!!!
        resistance[system_size * coil_number + coil_number] += data;
        return 0;
    }

    int ReadData::change_voltage_for_coil(int coil_number, int idx, double data){
        voltage_in_coils[coil_number][idx] += data;
        return 0;
    }

    int ReadData::text_parser(const string& str_in, TypeVariable key, string& str_out, double& val_out){
        bool myBool = false;
        str_out = "";
        val_out = 0;
        for(unsigned int i=0;i<str_in.size();i++){
            if(str_in[i] == '\t')
                myBool = true;
            if(myBool && (str_in[i]!= '\n') && (str_in[i]!= '\t') )//&&(str_in[i]!= ' ') )
                str_out += str_in[i];
        }
        std::istringstream stm;
        if(key == TypeVariable::type_string)
            return 0;
        else if(key == TypeVariable::type_double){
            stm.str(str_out);
            stm >> val_out;
            stm.clear();
        }
        return 0;
    }

    //function for read

    int ReadData::load_data(const string& read_path)
    {
        data_clear();
        ifstream reader;
        string str_tmp="",str_empty;
        double val_empty;
        //read_path = "C:\\PlasmaLabData\\";
        string full_read_path=read_path+"config.ini";
        reader.open(full_read_path.c_str());
        if(!reader)
        {
            cout<<"error open config.ini"<<endl;
            cout<<full_read_path;
            return -10;
        }
        getline(reader, str_tmp);//propusk stroki
        getline(reader,str_tmp);//vremia raboty
        text_parser(str_tmp,TypeVariable::type_double,str_empty,work_time);
        getline(reader,str_tmp);//shag integrirovanya
        text_parser(str_tmp,TypeVariable::type_double,str_empty,integration_step);
        getline(reader,str_tmp);//razmernost' sistemy
        text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
        system_size = (int)val_empty;
        getline(reader,str_tmp);//kol-vo control coils
        text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
        coils_count = (int)val_empty;
        getline(reader,str_tmp);//pogreshnost'
        text_parser(str_tmp,TypeVariable::type_double,str_empty,epsilon);
        getline(reader,str_tmp);//control points
        text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
        control_points_count = (int)val_empty;
        getline(reader,str_tmp);//obrezal'shik
        text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
        short_step = (int)val_empty;
        getline(reader,str_tmp);//loop voltage
        text_parser(str_tmp,TypeVariable::type_double,str_empty,required_loop_voltage);
        getline(reader,str_tmp);//radial field max
        text_parser(str_tmp,TypeVariable::type_double,str_empty,r_field_max);
        getline(reader,str_tmp);//vertical field max
        text_parser(str_tmp,TypeVariable::type_double,str_empty,z_field_max);

        getline(reader,str_tmp);//propusk stroki
        for(int i=0;i<coils_count;i++){
            getline(reader,str_tmp);
            text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
            currents_max.push_back(val_empty);
        }
        getline(reader,str_tmp);
        for(int i=0;i<coils_count;i++){
            getline(reader,str_tmp);
            text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
            voltages_max.push_back(val_empty);
        }

        getline(reader,str_tmp);
        for(int i=0;i<coils_count;i++){
            getline(reader,str_tmp);
            text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
            mag_fields_max.push_back(val_empty);
        }
        getline(reader,str_tmp);

        getline(reader,str_tmp);
        text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
        work_mode = val_empty;

        for(int i=0;i<9/*кол-во функционалов*/;i++){
            getline(reader,str_tmp);
            text_parser(str_tmp,TypeVariable::type_double,str_empty,val_empty);
            weight_coefficient.push_back(val_empty);
        }
//------------------------------------
        string tmp = read_path;
        string tmp_read_path = read_path;
        tmp_read_path += path_for_input_data;

        auto r1 = std::async(std::launch::async,&ReadData::load_inductance,this,tmp_read_path, file_name_for_inductance);
        auto r2 = std::async(std::launch::async,&ReadData::load_resistance,this,tmp_read_path, file_name_for_resistance);

        int res = 0;
        res |= load_voltage_in_coils(tmp_read_path);
        res |= load_alfa_xxx(tmp_read_path, file_name_for_alfa_psi_alfa_r_alfa_z);
        res |= load_initial_currents(tmp_read_path, file_name_for_initial_currents);
        res |= load_required_current_plasma(tmp_read_path, file_name_for_required_current_plasma);
        res |= load_test_currents_in_coils(tmp_read_path, file_name_for_test_currents);

        tmp_read_path = tmp;

        res |= r1.get();
        res |= r2.get();

        return res;
    }

    int ReadData::data_clear(){
        if(!mag_fields_max.empty())
            mag_fields_max.clear();
        if(!weight_coefficient.empty())
            weight_coefficient.clear();
        if(!currents_max.empty())
            currents_max.clear();
        if(!voltages_max.empty())
            voltages_max.clear();
        if(!voltage_in_coils.empty()){
            for(unsigned int i=0;i<voltage_in_coils.size();i++){
                if(!voltage_in_coils[i].empty())
                    voltage_in_coils[i].clear();
            }
            voltage_in_coils.clear();
        }
        if(!test_currents_in_coils.empty()){
            for(unsigned int i=0;i<test_currents_in_coils.size();i++){
                if(!test_currents_in_coils[i].empty())
                    test_currents_in_coils[i].clear();
            }
            test_currents_in_coils.clear();
        }
        if(!required_current_plasma.empty())
            required_current_plasma.clear();
        if(!inductance.empty())
            inductance.clear();
        if(!resistance.empty())
            resistance.clear();
        if(!initial_currents.empty())
            initial_currents.clear();
        if(!alfa_psi.empty())
            alfa_psi.clear();
        if(!alfa_r.empty())
            alfa_r.clear();
        if(!alfa_z.empty())
            alfa_z.clear();

        return 0;
    }

    int ReadData::load_resistance(const string& read_path, const string& file_name)
    {
        ifstream reader;
        string str_tmp="";
        std::istringstream stm;
        string full_read_path=read_path + file_name;//"matr_res_univ.dat";
        resistance.resize(system_size*system_size);
        reader.open(full_read_path.c_str());
        if(!reader)
        {
            return -10;
        }
        int i = 0;
        char ch;
        str_tmp.clear();
        reader.unsetf(ios::skipws);
        while(!reader.eof()) {
            reader>>ch;
            if(reader.eof())break;
            if((ch!=' ')&&(ch!='\n')&&(ch!='\t')) {
                str_tmp += ch;
            }
            else {
                stm.clear();
                if(str_tmp!=""){
                    stm.str(str_tmp);
                    stm>>resistance[i];
                    str_tmp.clear();
                    i++;
                }
            }
        }
        if((reader.eof())&&(str_tmp!="")){
            stm.clear();
            stm.str(str_tmp);
            stm>>resistance[system_size*system_size-1];
            str_tmp.clear();
        }
        reader.close();
        reader.clear();
        return 0;
    }

    int ReadData::load_inductance(const string& read_path, const string& file_name)
    {
        ifstream reader;
        string str_tmp="";
        std::istringstream stm;
        string full_read_path=read_path + file_name;
        //считывает индуктивность в контурах++++++++++++++++++++++++++++++++++++
        inductance.resize(system_size*system_size);
        reader.open(full_read_path.c_str());
        if(!reader)
            return -10;

        int i = 0;
        char ch;
        str_tmp.clear();
        reader.unsetf(ios::skipws);
        while(!reader.eof()) {
            reader>>ch;
            if(reader.eof())
                break;
            if((ch!=' ')&&(ch!='\n')&&(ch!='\t'))
                str_tmp += ch;
            else {
                stm.clear();
                if(str_tmp!=""){
                    stm.str(str_tmp);
                    stm>>inductance[i];
                    str_tmp.clear();
                    ++i;
                }
            }
        }
        if((reader.eof())&&(str_tmp!="")){
            stm.clear();
            stm.str(str_tmp);
            stm>>inductance[system_size*system_size-1];
            str_tmp.clear();
        }
        reader.close();
        reader.clear();
        return 0;
    }

    int ReadData::load_initial_currents(const string& read_path,const string& file_name)
    {
        //считывает начальные значения тока в катушках++++++++++++++++++++++++++++++++++++
        int i = 0;
        initial_currents.resize(system_size, 0);
        ifstream reader;
        string str_tmp="";
        std::istringstream stm;
        string full_read_path = read_path + file_name;//"I0.txt";
        reader.open(full_read_path.c_str());
        if(!reader){
            return -10;
        }
        while(!reader.eof()) {
            getline(reader,str_tmp);
            stm.clear();
            stm.str(str_tmp);
            stm>>initial_currents[i];
            i++;

        }
        reader.close();
        reader.clear();
        return 0;
    }

    void ReadData::load_alfa_one(ifstream &reader, vec_d &vec)
    {
        int i = 0;
        string str_tmp="";
        std::istringstream stm;
        char ch;
        while(!reader.eof()&&(i < (control_points_count * system_size))) {
            reader>>ch;
            if(reader.eof() || (i >= (control_points_count * system_size)))
                break;
            if((ch!=' ')&&(ch!='\n')&&(ch!='\t'))
                str_tmp += ch;
            else {
                stm.clear();
                if(str_tmp!=""){
                    stm.str(str_tmp);
                    stm>>vec[i];
                    str_tmp.clear();
                    ++i;
                }
            }
        }
    }
    int ReadData::load_alfa_xxx(const string& read_path, const string& file_name)
    {
        ifstream reader;
        alfa_psi.resize(control_points_count * system_size);
        alfa_r.resize(control_points_count * system_size);
        alfa_z.resize(control_points_count * system_size);
        string full_read_path=read_path + file_name;
        reader.open(full_read_path.c_str());
        if(!reader)
            return -10;

        reader.unsetf(ios::skipws);

        load_alfa_one(reader,alfa_r);//заполняем радиальную компоненту
        load_alfa_one(reader,alfa_z);//заполняем вертикальную компоненту
        load_alfa_one(reader,alfa_psi);//считываем  поток от единичных токов

        reader.close();
        reader.clear();
        return 0;
    }


    int ReadData::load_voltage_in_coils(const string& read_path)
    {
        //считываем файлы cs3u cs2u cs1 cs2l cs3l pf1-6
        voltage_in_coils.resize(11,vector<double> (0));
        int res = 0;
        for(unsigned int i=0;i<file_name_for_coils.size();i++)
            res |= load_one_coil(read_path,file_name_for_coils[i],i);
        return res;
    }

    int ReadData::load_one_coil(const string& read_path,const string& file_name,int idx)
    {
        int i=0;
        fstream reader;
        std::istringstream stm1;
        std::string full_read_path = read_path + file_name;
        reader.open(full_read_path.c_str());
        if(!reader){
            return -10;
        }
        reader.unsetf(ios::skipws);

        char ch;
        string str_tmp="";
        while(!reader.eof()) {
            reader>>ch;
            if(reader.eof())break;
            if((ch!=' ')&&(ch!='\n')&&(ch!='\t')){// ||((str_tmp!="")&&(reader.eof()))){
                str_tmp += ch;
            }
            else {
                stm1.clear();
                if(str_tmp!=""){
                    double number_tmp=0;
                    stm1.str(str_tmp);
                    stm1>>number_tmp;
                    voltage_in_coils[idx].push_back(number_tmp);
                    str_tmp.clear();
                    ++i;
                    str_tmp.clear();
                }
            }
        }
        return 0;
    }

    int ReadData::load_required_current_plasma(const string& read_path, const string &file_name)
    {
        int i=0;
        fstream reader;
        std::istringstream stm1;
        std::string full_read_path = read_path + file_name;
        reader.open(full_read_path.c_str());
        if(!reader)
            return -10;

        reader.unsetf(ios::skipws);

        char ch;
        string str_tmp="";
        getline(reader, str_tmp);//propusk stroki
        str_tmp="";
        while(!reader.eof()) {
            reader>>ch;
            if(reader.eof())break;
            if((ch!=' ')&&(ch!='\n')&&(ch!='\t')){// ||((str_tmp!="")&&(reader.eof()))){
                str_tmp += ch;
            }
            else {
                stm1.clear();

                if(str_tmp!=""){
                    double double_buf = 0;
                    stm1.str(str_tmp);
                    stm1>>double_buf;
                    required_current_plasma.push_back(double_buf);
                    str_tmp.clear();
                    i++;
                    str_tmp.clear();
                }
            }
        }
        if(str_tmp != "" && str_tmp!= " " && str_tmp != "\n" && str_tmp != "\t"){
            stm1.clear();
            double double_buf=0;
            stm1.str(str_tmp);
            stm1>>double_buf;
            required_current_plasma.push_back(double_buf);
            str_tmp.clear();
            i++;
            str_tmp.clear();
        }
        return 0;
    }

    int ReadData::load_test_currents_in_coils(const string& read_path, const string& file_name){
        int i = 0, k = 0;
        ifstream reader;
        string str_tmp="", str="";
        std::istringstream stm;
        string big_path=read_path + file_name;
        reader.open(big_path.c_str());
        if(!reader)
            return -10;

        getline(reader,str_tmp);
        while(!reader.eof()) {
            if(i == 32) break;
            getline(reader,str_tmp);
            test_currents_in_coils.push_back(vec_d(12,0));
            k=0;
            for(unsigned int j=0;j<str_tmp.size();j++){
                if( (str_tmp[j]!='\t')&&(str_tmp[j]!=' ')&&(str_tmp[j]!='\n') )
                    str += str_tmp[j];
                else{
                    stm.clear();
                    stm.str(str);
                    if(k<7 && k!=0){
                        stm>>test_currents_in_coils[i][k+5];
                        test_currents_in_coils[i][k+5] *= 1000;
                    }else if(k>=7 && k!=0){
                        stm>>test_currents_in_coils[i][k-6];
                        test_currents_in_coils[i][k-6]*=1000;
                    }else if(k==0)
                        stm>>test_currents_in_coils[i][k];
                    str.clear();
                    k++;
                    if(k > coils_count) j = str_tmp.size();
                }
            }
            i++;
        }
        reader.close();
        reader.clear();
        return 0;
    }
}
