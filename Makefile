SRC_DIR := ./src
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SRCS:.c=.o)
TARGET:= esp_ledd

CFLAGS := -Wall -Werror -pedantic -g
LDFLAGS := -lserialport

all: clean $(TARGET)
.PHONY: all

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
.PHONY: clean