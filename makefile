ROOT_DIR:= $(realpath ./)
EXAMPLE_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

DEFINES:=-DHOME_DIR=\""$(EXAMPLE_DIR)\"" -DASSETS_DIR=\""$(ROOT_DIR)/assets/\""
all : main

main : main.cpp
	c++ -I./opengl $(DEFINES) -g -o $@ $^ -lGL -lglfw -lGLEW -lopencv_core -lopencv_imgproc
	#c++ -I./opengl -I./thirdparty/libpnmio/src -L./thirdparty/libpnmio/lib $(DEFINES) -g -o $@ $< -lGL -lglfw -lGLEW 

clean:
	rm -r main
