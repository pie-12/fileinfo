APP      = fileinspector
SRC      = src/main.c src/file_utils.c
CFLAGS   = -Wall -Wextra -O2 `pkg-config --cflags gtk+-3.0`
LDFLAGS  = `pkg-config --libs gtk+-3.0`

all: $(APP)

$(APP): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)
	@echo "Build complete: ./$(APP)"

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP)

.PHONY: all run clean

