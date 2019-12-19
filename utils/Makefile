include ../Config

TARGET = ../bin/fpzip

all: $(TARGET)

$(TARGET): fpzip.cpp ../lib/$(LIBFPZIP)
	mkdir -p ../bin
	$(CXX) $(CXXFLAGS) fpzip.cpp -L../lib -lfpzip -o $(TARGET)

clean:
	rm -f $(TARGET)
