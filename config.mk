VERSION = 0.1
NAME = kawigrwm

CC = g++
P_SRC = src/
SRC = ${P_SRC}main.cpp ${P_SRC}Manager.cpp ${P_SRC}Events.cpp ${P_SRC}Functions.cpp ${P_SRC}ClientList.cpp ${P_SRC}Layout.cpp
OUT = ${NAME}
LIBS = -lX11

CPPFLAGS = -s -Wall -std=c++17 -D VERSION=\"${VERSION}\" -D NAME=\"${NAME}\" -Os -Wno-deprecated-declarations

