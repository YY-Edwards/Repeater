#CROSS_COMPILE=arm-none-linux-gnueabi-
TARGET = $(notdir $(CURDIR))
CC=$(CROSS_COMPILE)g++
#C_FLAGS += -lasound
C_FLAGS += -ldl -lm -lpthread -ljrtp  -lspeex -lasound

objs := $(patsubst %c, %o, $(shell ls *.cpp))

INC += -I /usr/share/alsa-lib/include -I /opt/linux/Examples/Repeater-test/  -I /opt/linux/speex-utils/speex-1.2rc2/include -I /usr/local/include/jrtplib3 

LIB += -L/usr/share/alsa-lib/lib/  -L/usr/local/lib/ 


all: repeater


repeater:
	$(CC) *.cpp -o repeater $(INC) $(LIB) $(C_FLAGS)

#all: send
#send:
#	$(CC) mulcast_send.cpp -o send $(INC) $(LIB) $(C_FLAGS)


clean:
	rm -f repeater   *.o *~







