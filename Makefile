OBJS = src/kjv.o \
       src/intset.o \
       data/data.o
CFLAGS += -Wall -Isrc/
LDLIBS += -lreadline

kjv: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

data/data.c: data/kjv.tsv data/generate.awk src/data.h
	awk -f data/generate.awk $< > $@
	awk -f data/subheadings.awk data/subheadings.tsv >> $@

.PHONY: clean
clean:
	rm -rf $(OBJS) kjv
