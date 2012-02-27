CXXFLAGS = -Wall -std=c++0x
objects = paint.o TargaImageManipulator.o AreaResize.o CircleBrush.o LineBrush.o GaussianKernel.o TargaImage.o libtarga.o

all: CXXFLAGS += -D NDEBUG
all: CXXFLAGS += -O3
all: paint
debug: CXXFLAGS += -g
debug: paint

paint: $(objects)
	$(CXX) $(CXXFLAGS) $(objects) -o paint

libtarga.o:
	$(CXX) -c -o libtarga.o libtarga.c

clean:
	rm -f *.o *.out *.a paint

.PHONY: clean
