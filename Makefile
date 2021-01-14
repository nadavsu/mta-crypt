TARGET_NAME = mtacrypt.out
CC = gcc

#added libraries.
ODIR ?= build

CFLAGS = -I$(IDIR)
LIBS = -lpthread -lmta_rand -lmta_crypt

OUTFILE = $(ODIR)/out.log

#finding all files with suffix .c, removing the './' returned by 'find' command using subst command.
SRCS := $(subst ./,,$(shell find . -name "*.c"))

#creating object file names by replacing *.c to *.o.
OBJS := $(addprefix $(ODIR)/,$(patsubst %.c,%.o,$(SRCS)))
#Rules:

all: $(ODIR)/$(TARGET_NAME)

#pattern matched rule - anything that ends with .o relies on the same file with .c
#magic variables: $@ == target, $^ == all prequisites, $< == first prequisite
$(ODIR)/%.o : %.c
	@mkdir -p $(ODIR)
	$(CC) -c $< -o $@ 

$(ODIR)/$(TARGET_NAME): $(OBJS) 
	$(CC) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm -rf $(ODIR)

.PHONY: test
test:
	./$(ODIR)/$(TARGET_NAME) -n 5 -l 8 -t 5
