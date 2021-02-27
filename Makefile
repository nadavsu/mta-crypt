TARGET_NAME = launcher.out
CC = gcc

#added libraries.


CFLAGS = -I$(IDIR)
LIBS = -lrt -lmta_rand -lmta_crypt


#finding all files with suffix .c, removing the './' returned by 'find' command using subst command.
SRCS := $(subst ./,,$(shell find . -name "*.c"))

#creating object file names by replacing *.c to *.o.
OBJS := $(patsubst %.c,%.out,$(SRCS))
#Rules:

all: $(OBJS)

#pattern matched rule - anything that ends with .o relies on the same file with .c
#magic variables: $@ == target, $^ == all prequisites, $< == first prequisite
%.o : %.c 
	$(CC) -c $< -o $@ 

%.out: %.o include.h
	$(CC) $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm *.out


