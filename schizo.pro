#-------------------------------------------------
#
# Project created by QtCreator 2014-10-25T21:34:09
#
#-------------------------------------------------

QT       -= core gui

TARGET = schizo
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXX	= g++
QMAKE_CXXFLAGS	+= -std=c++11 -fno-rtti -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -fvisibility-inlines-hidden -pedantic -ffunction-sections -fdata-sections
QMAKE_LIBS	+= -ldl -lm
QMAKE_LFLAGS	+= -Wl,--gc-sections
QMAKE_LINK	= gcc

SOURCES += \
    lexer.cpp \
    main.cpp \
    parser.cpp \
    schizo.cpp

lexer.target = lexer.cpp
lexer.commands = ragel -C -o $$PWD/lexer.cpp $$PWD/lexer.rl
lexer.depends =

parser.target = parser.cpp
parser.commands = lemon -T$$PWD/lempar.c_template $$PWD/parser.y && mv $$PWD/parser.c $$PWD/parser.cpp
parser.depends =

QMAKE_EXTRA_TARGETS	+= lexer parser

PRE_TARGETDEPS	+= lexer.cpp parser.cpp

OTHER_FILES += \
    parser.y \
    lempar.c_template \
    lexer.rl

HEADERS += \
    parser.h \
    schizo.h \
    vector.h
