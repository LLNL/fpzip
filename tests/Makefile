include ../Config

BINDIR = ../bin
LIBDIR = ../lib
TARGET = $(BINDIR)/testfpzip

all: $(TARGET)

$(TARGET): testfpzip.c ../lib/$(LIBFPZIP)
	mkdir -p ../bin
	$(CC) $(CFLAGS) testfpzip.c -L$(LIBDIR) -lfpzip -lstdc++ -o $(TARGET)

test: $(BINDIR)/testfpzip
	$(BINDIR)/testfpzip

clean:
	rm -f $(TARGET)
