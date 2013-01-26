
KERNELDIR = /usr/src/linux-source-3.0.0

MAKEFLAGS = -k

CFLAGS    = -O2 -Wall -Wno-parentheses -I$(KERNELDIR)/include \
	    -fno-strict-aliasing \
	    -DSO_RXQ_OVFL=40 \
	    -DPF_CAN=29 \
	    -DAF_CAN=PF_CAN

PROGRAMS = cangen_pectel

all: $(PROGRAMS)

clean:
	rm -f $(PROGRAMS) *.o

cangen_pectel.o:	

cangen_pectel:	cangen_pectel.o

