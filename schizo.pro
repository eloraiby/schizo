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

INCLUDEPATH	+= $$PWD/include

SOURCES += \
    src/lexer.cpp \
    main.cpp \
    src/parser.cpp \
    src/schizo.cpp \
    src/expression.cpp

lexer.target = lexer.cpp
lexer.commands = ragel -C -o $$PWD/src/lexer.cpp $$PWD/src/lexer.rl
lexer.depends =

parser.target = parser.cpp
parser.commands = lemon -T$$PWD/src/lempar.c_template $$PWD/src/parser.y && mv $$PWD/src/parser.c $$PWD/src/parser.cpp
parser.depends =

QMAKE_EXTRA_TARGETS	+= lexer parser

PRE_TARGETDEPS	+= lexer.cpp parser.cpp

OTHER_FILES += \
    src/parser.y \
    src/lempar.c_template \
    src/lexer.rl

HEADERS += \
    src/parser.h \
    include/schizo/schizo.hpp \
    include/schizo/platform.hpp \
    include/schizo/intrusive_ptr.hpp \
    include/schizo/expression.hpp
