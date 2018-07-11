/*!
  \file
* В данном файле происходит запись в файлы (в несколько потоков) результа работы программы.
*/
#ifndef WRITE_DATA_H
#define WRITE_DATA_H

#include <src/model/model.h>

namespace  PlasmaLab {

    class WriteData
    {
        int write(const string &, const vvec_d &, double, double, int, int) const;
    public:
        WriteData() {}
        ~WriteData(){}
        /*!
         * \brief Функция для записи результатов.
         * \param[in] const string &, const Model & путь, куда будет произведена запись и данные для записи.
         * \return 0, если все хорошо.
         */
        int main_write(const string &path, const Model &) const;
    };
}
#endif // WRITE_DATA_H
