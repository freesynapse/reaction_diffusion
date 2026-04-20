BINARY=module.so
CODEDIRS=. ./src
INCDIRS=$(CODEDIRS) $(HOME)/source/lib/raylib/include $(HOME)/source/include/hotloader_raylib/src/common/
LIBDIRS=. /home/iomanip/source/lib/raylib/lib
BUILDDIR=.

CC=g++
OPT=-O0

# generate files that encode make rules for the .h dependencies
DEPFLAGS=-MP -MD

# automatically add the -I onto each include directory
CFLAGS=-Wall -Wextra -Wno-missing-field-initializers -ggdb $(foreach D,$(INCDIRS),-I$(D)) $(OPT) $(DEPFLAGS) -fPIC 
LDFLAGS=-shared $(foreach D,$(LIBDIRS),-L$(D)) $(foreach D,$(LIBDIRS),-Wl,-rpath,"$(D)") -lraylib -lm -ldl

# for-style iteration (foreach) and regular expression completions (wildcard)
CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))

# regular expression replacement
OBJECTS=$(patsubst %.cpp,%.o,$(CFILES))
DEPFILES=$(patsubst %.cpp,%.d,$(CFILES))

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $(BUILDDIR)/$@ $^ $(LDFLAGS)

# only want the .cpp file dependency here, thus $< instead of $^.
#
%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR)/$(BINARY) $(OBJECTS) $(DEPFILES)


# include the dependencies
-include $(DEPFILES)

# add .PHONY so that the non-targetfile - rules work even if a file with the same name exists.
.PHONY: all clean