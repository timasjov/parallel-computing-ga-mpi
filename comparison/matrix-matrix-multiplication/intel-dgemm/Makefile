# first, load => intel_cluster_studio/2013.1.046
# values for Gnu Fortran
FC=gfortran
FCFLAGS=-O3 -g -funroll-loops
FCFLAGS_OMP=-fopenmp
LIBS=-lgomp
CXXFLAGS=$(WARN) -DMKL_ILP64
LDFLAGS=-L$(MKLROOT)/lib/intel64 -lmkl_intel_ilp64 -lmkl_core -lmkl_sequential -lpthread -lm

PROGRAMS=matrixmul
all: $(PROGRAMS)
	
mmm: mmm.F90 timings.o
	$(FC) $(FCFLAGS) $(CXXFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS)

timings.o: timings.F90
	$(FC) -c $(CPPFLAGS) $(FCFLAGS_OMP) $(FCFLAGS) $^

clean:
	rm -f *.o *.mod
	rm -f $(PROGRAMS)
