CONTIKI_PROJECT = mac_configuration
all: $(CONTIKI_PROJECT)

CONTIKI = $(HOME)/contiki-ng
MAKE_MAC ?= MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
 
include $(CONTIKI)/Makefile.include
