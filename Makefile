CC=gcc
TARGET=shell
all:
	$(CC) $(TARGET).c -o $(TARGET) 
	./$(TARGET)
clean:
	rm $(TARGET)
