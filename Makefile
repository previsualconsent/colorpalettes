ROOT_FLAGS:=$(shell root-config --cflags --libs)

all: colorpalettes

colorpalettes: colorpalettes.cpp colorpalettes.h
	g++ colorpalettes.cpp -o colorpalettes  -std=c++0x ${ROOT_FLAGS}
