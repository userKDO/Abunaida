CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lavformat -lavcodec -lavutil -lm -lpthread -ldl

TARGET = test_player
OBJ = player.o miniaudio.o main.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)
	
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f *.o $(TARGET)
