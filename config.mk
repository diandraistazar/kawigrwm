VERSION = 0.1
NAME = kawigrwm

CC = g++

SRC = main.cpp
OUT = ${NAME}
LIBS = -lX11

CPPFLAGS = -s -Wall -std=c++17 -D VERSION=\"${VERSION}\" -D NAME=\"${NAME}\" -Os -Wno-deprecated-declarations

