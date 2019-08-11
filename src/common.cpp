#include<src/common.h>
namespace PlasmaLab{
    void matrix_multiplier(const vec_d &m1, const vec_d &m2,vec_d &m3_res, uint32_t m1_width, uint32_t m2_high){
        // для примера, пусть матрица m1 имеет размерность 3на3, а m2 размерность 2на3.тогда m1_width=3, а  m2_high=2
        for(uint32_t n = 0;n < m1_width; ++n){
            for(uint32_t i = 0;i < m2_high;++i){
                for(uint32_t j = 0;j < m1_width;++j)
                    m3_res[n * m2_high + i] += m1[n * m1_width + j] * m2[m2_high * j + i];
            }
        }
    }
    void matrix_multiplier(double &val, const vec_d &v1,const vec_d &v2){
        val=0;
        for(uint32_t j = 0;j < v1.size();++j)
            val += v1[j] * v2[j];
    }
}
