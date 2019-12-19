include ../Config

LIBDIR = ../lib
TARGETS = $(LIBDIR)/libfpzip.a $(LIBDIR)/libfpzip.so
OBJECTS = error.o rcdecoder.o rcencoder.o rcqsmodel.o read.o version.o write.o

static: $(LIBDIR)/libfpzip.a

shared: $(LIBDIR)/libfpzip.so

clean:
	rm -f $(TARGETS) $(OBJECTS)

$(LIBDIR)/libfpzip.a: $(OBJECTS)
	mkdir -p $(LIBDIR)
	rm -f $@
	ar rc $@ $^

$(LIBDIR)/libfpzip.so: $(OBJECTS)
	mkdir -p $(LIBDIR)
	$(CXX) $(CXXFLAGS) -shared $^ -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
