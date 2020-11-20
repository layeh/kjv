OBJS = src/kjv.o \
       src/intset.o \
       data/data.o
CFLAGS += -Wall -Iinclude/
LDLIBS += -lreadline

kjv: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)


data/data.c: data/kjv.tsv data/generate.awk include/data.h
	awk -f data/generate.awk $< > $@

.PHONY: clean
clean:
	rm -rf $(OBJS) kjv
