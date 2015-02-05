CXXFLAGS = -std=c++11 -Wall -MMD

-include *.d

all: tests

tests: test_gf256 test_base64

test_gf256: test_gf256.o gf256.o
	$(CXX) -o $@ $^ $(LDFLAGS)

test_base64: test_base64.o base64.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) test_gf256 test_base64 test_gf256.o gf256.o test_base64.o base64.o

.PHONY: all tests clean

