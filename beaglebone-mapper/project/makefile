CROSS_COMPILE = arm-linux-gnueabi-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -pthread -Wall -g  -std=c99 -D_POSIX_C_SOURCE=200809L -Werror
OUTDIR = $(HOME)/cmpt433/public/myApps
OUTFILE = project

SRC = project.c pwm_servo.c ad_reader.c lib.c routine.c UDPListener.c webcam.c
OBJ = $(addsuffix .o, $(basename $(SRC)))

all: beatbox

beatbox: $(OBJ)
	$(CC_C) $(CFLAGS) $(LFLAGS) -o $(OUTDIR)/$(OUTFILE) $(OBJ) -lm -lpthread -pg
%.o : %.c
	$(CC_C) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
	rm -f $(OUTDIR)/$(OUTFILE)