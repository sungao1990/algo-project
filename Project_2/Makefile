CXX = clang++
CXXFLAGS = -Wall -O0 -g `pkg-config --cflags gtk+-2.0`
PROGRAMS = convex-hull convex-hull_P3_Step convex-hull_P6_Step convex-hull_1_7_Step

all:
	for prg in $(PROGRAMS); \
	do \
	  $(MAKE) recurse PRG=$$prg; \
	done

recurse: $(PRG).o gfx.o Makefile
	$(CXX) $(CXXFLAGS) -o $(PRG) $(PRG).o gfx.o `pkg-config --libs gtk+-2.0`

clean:
	rm -rf *~ *.o *.dSYM $(PROGRAMS)

