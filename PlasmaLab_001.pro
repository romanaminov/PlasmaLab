QT += core
QT -= gui

CONFIG += c++11

TARGET = PlasmaLab
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp \
    src/read_data/read_data.cpp \
    src/model/model.cpp \
    src/write_data/write_data.cpp \
    src/model/functionals.cpp

DEFINES += QT_DEPRECATED_WARNINGS


HEADERS += src/read_data/read_data.h \
    src/common.h \
    src/model/model.h \
    src/write_data/write_data.h \
    src/model/functionals.h
