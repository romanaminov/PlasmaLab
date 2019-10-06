#include<src/common.h>
namespace PlasmaLab{
    void matrixMultiplier(const vec_d& m1, const vec_d& m2,vec_d& m3Res, uint64_t m1Width, uint64_t m2High){
        // для примера, пусть матрица m1 имеет размерность 3на3, а m2 размерность 2на3.тогда m1_width=3, а  m2_high=2
        for(uint32_t n = 0;n < m1Width; ++n){
            for(uint32_t i = 0;i < m2High;++i){
                for(uint32_t j = 0;j < m1Width;++j)
                    m3Res[n * m2High + i] += m1[n * m1Width + j] * m2[m2High * j + i];
            }
        }
    }
    void matrixMultiplier(double& val, const vec_d& v1,const vec_d& v2){
        val=0;
        for(uint32_t j = 0;j < v1.size();++j)
            val += v1[j] * v2[j];
    }
}
