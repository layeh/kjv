OBJS = src/kjv.o \
       src/intset.o \
       data/data.o
CFLAGS += -Wall -Iinclude/
LDFLAGS += -lreadline

kjv: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

data/data.c: data/kjv.tsv data/generate.awk include/data.h
	awk -f data/generate.awk $< > $@

.PHONY: clean
clean:
	rm -rf $(OBJS) kjv
