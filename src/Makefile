SRCS := $(wildcard *.c)
OBJECTS = $(SRCS:.c=.o)
TARGET:= esp_ubusd

CFLAGS := -Wall -pedantic -g
LDFLAGS := -lserialport -lubox -lblobmsg_json -ljson-c -lubus

all: clean $(TARGET)
.PHONY: all

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
.PHONY: clean