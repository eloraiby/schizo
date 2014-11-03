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

QMAKE_CC	= clang
QMAKE_CFLAGS	+= -std=c89

SOURCES += \
    main.c \
    lexer.c \
    parser.c \
    cell.c

lexer.target = lexer.c
lexer.commands = ragel -C -o $$PWD/lexer.c $$PWD/lexer.rl
lexer.depends =

parser.target = parser.c
parser.commands = lemon -T$$PWD/lempar.c_template $$PWD/parser.y
parser.depends =

QMAKE_EXTRA_TARGETS	+= lexer parser

PRE_TARGETDEPS	+= lexer.c parser.c

OTHER_FILES += \
    parser.y \
    lempar.c_template \
    lexer.rl

HEADERS += \
    parser.h \
    schizo.h
