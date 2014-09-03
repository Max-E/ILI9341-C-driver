LDLIBS := -lm
CFLAGS := -std=c99 -g -Ofast -Wall -Wextra

ALL: testprogram
.PHONY: all

clean:
	rm *.o testprogram

testprogram: ILI9341.o testprogram.o
