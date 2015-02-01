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

QMAKE_CXX	= clang
QMAKE_CXXFLAGS	+= -std=c++11 -fno-rtti -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -fvisibility-inlines-hidden -pedantic -ffunction-sections -fdata-sections
QMAKE_LIBS	+= -ldl -lm -lpthread
QMAKE_LFLAGS	+= -Wl,--gc-sections
QMAKE_LINK	= gcc

INCLUDEPATH	+= $$PWD/include

SOURCES += \
    src/lexer.cpp \
    main.cpp \
    src/parser.cpp \
    src/expression.cpp \
    src/special_forms.cpp \
    src/built_ins.cpp \
    src/eval.cpp \
    src/platform.cpp

lexer.target = lexer.cpp
lexer.commands = ragel -C -o $$PWD/src/lexer.cpp $$PWD/src/lexer.rl
lexer.depends =

parser.target = parser.cpp
parser.commands = lemon -T$$PWD/src/lempar.c_template $$PWD/src/parser.y && mv $$PWD/src/parser.c $$PWD/src/parser.cpp
#$$PWD/lemon-cpp -X -T$$PWD/src/lempar.c_template $$PWD/src/parser.y #&& mv $$PWD/src/parser.c $$PWD/src/parser.cpp
#parser.depends = lemon-cpp

#lemon-cpp.target = lemon-cpp
#lemon-cpp.commands = gcc -o$$PWD/lemon-cpp $$PWD/src/lemon-cpp.c
#lemon-cpp.depends =

QMAKE_EXTRA_TARGETS	+= lexer parser #lemon-cpp

PRE_TARGETDEPS	+= lexer.cpp parser.cpp #lemon-cpp

OTHER_FILES += \
    src/parser.y \
    src/lempar.c_template \
    src/lexer.rl \
    src/lemon-cpp.c

HEADERS += \
    src/parser.h \
    include/schizo/schizo.hpp \
    include/schizo/expression.hpp \
    include/ftl/function.hpp \
    include/ftl/intrusive_ptr.hpp \
    include/ftl/array.hpp \
    include/ftl/platform.hpp \
    include/ftl/noncopyable.hpp \
    include/ftl/string.hpp \
    include/ftl/thunk.hpp \
    include/schizo/environment.hpp \
    include/ftl/list.hpp
