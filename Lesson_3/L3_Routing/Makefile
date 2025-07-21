CONTIKI_PROJECT = routing
all: $(CONTIKI_PROJECT)

PROJECT_SOURCEFILES += buffer.c
PROJECT_SOURCEFILES += helpers.c

CONTIKI = $(HOME)/contiki-ng
MAKE_MAC = MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

include $(CONTIKI)/Makefile.include
