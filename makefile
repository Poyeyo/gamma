# this file has the app specific data

# this defines the name of the app, and the main file
TARGET  =	gamma

# this is the list of object files needed

LNOBJS	=	$(oDir)/$(TARGET).o

EXOBJS	=	$(oDir)/$(TARGET)_rc.o \
		$(LNOBJS)

TESTS   =	

USEWX   = 1
DEVWX	= 1
DEBUG	= 0

RESOBJ  =	gamma.ico

# global data
include ../include.mak


# this is the deps of the object files 
$(oDir)/$(TARGET).o:	$(TARGET).cpp
	$(CC) -c $(CFLAGS) -o $@ $<
