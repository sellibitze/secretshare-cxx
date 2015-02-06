CXXFLAGS = -std=c++11 -Wall -O1 -MMD
LDFLAGS = 

all: tests sss

tests: test_base64 test_gf256

TEST_GF256_OBJ = gf256.o test_gf256.o
TEST_BASE64_OBJ = base64.o test_base64.o
SSS_OBJ = base64.o crc24.o gf256.o poly.o getrandom.o main.o

test_base64: $(TEST_BASE64_OBJ)
	$(CXX) -o $@ $(TEST_BASE64_OBJ) $(LDFLAGS)

test_gf256: $(TEST_GF256_OBJ)
	$(CXX) -o $@ $(TEST_GF256_OBJ) $(LDFLAGS)

sss: $(SSS_OBJ)
	$(CXX) -o $@ $(SSS_OBJ) $(LDFLAGS)

ALL_BINS = test_base64 test_gf256 sss
ALL_OBJS = $(sort $(TEST_BASE64_OBJ) $(TEST_GF256_OBJ) $(SSS_OBJ))
ALL_DEPS = $(patsubst %.o,%.d, $(ALL_OBJS))

clean:
	$(RM) $(ALL_BINS) $(ALL_OBJS) $(ALL_DEPS)

-include *.d

.PHONY: all tests clean
