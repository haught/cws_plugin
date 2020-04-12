#

VERSION				:= "V1.1"
FILE_NAME			:=
PACKAGE_NAME		:= 
OSFLAG 				:=
ifeq ($(OS),Windows_NT)
	OSFLAG += -D WIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		OSFLAG += -D AMD64
		FILE_NAME = "cws_plugin.exe"
		PACKAGE_NAME = "cws_plugin_32bit_$(VERSION).CHplugin"
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		OSFLAG += -D IA32
		FILE_NAME = "cws_plugin.exe"
		PACKAGE_NAME = "cws_plugin_64bit_$(VERSION).CHplugin"
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG += -D LINUX
		FILE_NAME = "cws_plugin_linux"
		PACKAGE_NAME = "cws_plugin_linux_$(VERSION).CHplugin"
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG += -D OSX -std=c++11
		FILE_NAME = "cws_plugin_Mac"
		PACKAGE_NAME = "cws_plugin_mac_$(VERSION).CHplugin"
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		OSFLAG += -D AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		OSFLAG += -D IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		OSFLAG += -D ARM
	endif
endif

all: build package

build:
	@echo $(OSFLAG)
	@echo $(PACKAGE_NAME)
	c++ -o $(FILE_NAME) ${OSFLAG} cws_plugin.cpp

package:
	zip $(PACKAGE_NAME) plugin.json $(FILE_NAME)

clean:
	rm -f $(FILE_NAME) $(PACKAGE_NAME)