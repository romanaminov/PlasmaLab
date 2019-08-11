QT += core
QT -= gui

CONFIG += c++17

TARGET = PlasmaLab
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp \
    src/common.cpp \
    src/read_data/read_data.cpp \
    src/model/model.cpp \
    src/write_data/write_data.cpp \
    src/model/functionals.cpp

DEFINES += QT_DEPRECATED_WARNINGS
QTC_CLANG_CMD_OPTIONS_BLACKLIST=-fno-keep-inline-dllexport
#CXXFLAGS=-fno-keep-inline-dllexport

HEADERS += src/read_data/read_data.h \
    src/common.h \
    src/model/model.h \
    src/write_data/write_data.h \
    src/model/functionals.h
