CC = gcc
SUBMIT_DIR := .submit
SUBMIT_FILES:= *.c *.h Makefile readme EthicsOath.pdf
SUBMIT := $(STUDENT_ID)_assign5.tar.gz

TARGET = ish
OBJECT = shell.o dynarray.o

all: $(TARGET)

$(TARGET) : $(OBJECT)
	$(CC) -o $@ $^

submit:
	mkdir -p $(SUBMIT_DIR)
	cp $(SUBMIT_FILES) $(SUBMIT_DIR)
	cd $(SUBMIT_DIR) && tar -czf ../$(SUBMIT) *
	rm -rf $(SUBMIT_DIR)

clean:
	rm -rf $(TARGET) *.o

.PHONY: all clean submit