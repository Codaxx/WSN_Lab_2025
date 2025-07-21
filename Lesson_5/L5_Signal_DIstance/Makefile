CONTIKI_PROJECT = signal_distance
all: $(CONTIKI_PROJECT)
	
#UIP_CONF_IPV6=1

MODULES += os/services/shell

CONTIKI = $(HOME)/contiki-ng
MAKE_MAC = MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET

include $(CONTIKI)/Makefile.include
