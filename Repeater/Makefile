#CROSS_COMPILE=arm-none-linux-gnueabi-
TARGET = repeater
CXX=$(CROSS_COMPILE)g++
CC=$(CROSS_COMPILE)gcc
#C_FLAGS += -lasound
C_FLAGS += -ldl -lm -lpthread -ljrtp  -lspeex -lasound -g -Wl,--as-needed

objs := $(patsubst %c, %o, $(shell ls *.cpp))

INC += -I /usr/share/alsa-lib/include -I /opt/linux/Examples/Repeater-test/  -I /opt/linux/speex-utils/speex-1.2rc2/include -I /usr/local/include/jrtplib3 

LIB += -L/usr/share/alsa-lib/lib/  -L/usr/local/lib/ 


all: $(TARGET)


repeater: main.o fifoqueue.o syninterface.o MyRepeater.o Protocol.o CMaster.o CSlave.o Myrtp.o Monitor_Interface.o MFile.o gpio.o GPIO_App.o AudioAlsa.o myspeex.o
	$(CXX) -o $@ $^ $(INC) $(LIB) $(C_FLAGS)

.cpp.o:
	$(CXX) -c -o $*.o $(INC) $(C_FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(INC) $(C_FLAGS) $*.c

.PHONY : clean
clean:
	rm -f *.o $(TARGET)







