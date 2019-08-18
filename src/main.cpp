#include <QCoreApplication>
#include <ctime>
#include <chrono>
#include <QTextCodec>
#include <iostream>
#include "read_data/read_data.h"
#include "model/model.h"
#include "write_data/write_data.h"
#include <QtCore\QtCore>
using namespace PlasmaLab;

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);
    qApp->addLibraryPath("plugins/");

    QCoreApplication a(argc, argv);
    string path;
#ifdef Q_OS_WIN32
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
    //path = "C:\\QtProjects\\PlasmaLab\\PlasmaLab_001\\const_init_data\\";
    path = "D:\\YandexDisk\\PORTFOLIO\\27.07.1019\\PlasmaLab\\const_init_data\\";
    path = "C:\\Projects\\1_no_change\\";
#endif

#ifdef Q_OS_LINUX
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    path = "/home/roman/QtProjects/PlasmaLab/PlasmaLab_001/const_init_data/";
#endif

    if(argc == 2)
        path = argv[0];

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    ReadData read_data; //храним начальные данные, загруженные из файлов
    Model model; // модель динамики полоидальных токов в токамаке
    WriteData write_data; //записываем полученные данные в файлы


    if(!read_data.load_data(path)){ //если данные для работы загружены, то работаем.
        FunctionalModel functionalModel(read_data); // храним значение итоговой штрафной функции и контролируем, чтобы
        model.main_function(read_data, functionalModel/* +приращения к управл.параметрам */ /* класс с функционалами */);
        write_data.main_write(path,model);
    }
    end = std::chrono::system_clock::now();

    int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds> (end-start).count();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << QString::fromUtf8("Вычисления закончены в ").toLocal8Bit().data() << std::ctime(&end_time)
              << QString::fromUtf8("Время выполнения: ").toLocal8Bit().data() << elapsed_seconds
              << QString::fromUtf8(" мс.\n").toLocal8Bit().data();

    return a.exec();
}
