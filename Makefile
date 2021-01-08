TARGET_NAME = mtacrypt.out
CC = gcc

#added libraries.
IDIR = ../include
LDIR = ../lib
ODIR ?= build

CFLAGS = -I$(IDIR)
LIBS = -lpthread -lmta_rand -lmta_crypt -lcrypto

#finding all files with suffix .c, removing the './' returned by 'find' command using subst command.
SRCS := $(subst ./,,$(shell find . -name "*.c"))

#creating object file names by replacing *.c to *.o.
OBJS := $(patsubst %.c, %.o, $(SRCS))

#Rules:

all: $(TARGET_NAME)

#pattern matched rule - anything that ends with .o relies on the same file with .c
#magic variables: $@ == target, $^ == all prequisites
%.o : %.c
	$(CC) -g -c $^

$(TARGET_NAME): $(OBJS)
	$(CC) -g -Wall $^ -o $@ $(LIBS)

.PHONY: clean
clean:
	rm -rf $(ODIR)