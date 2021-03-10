OBJS = src/kjv_main.o \
       src/kjv_ref.o \
       src/intset.o \
       data/kjv_data.o
CFLAGS += -Wall -Isrc/
LDLIBS += -lreadline

kjv: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

src/kjv_main.o: src/kjv_data.h src/kjv_ref.h src/intset.h

src/kjv_ref.o: src/kjv_ref.h src/intset.h src/kjv_data.h

src/insetset.o: src/intset.h

data/kjv_data.c: data/kjv.tsv data/generate.awk src/kjv_data.h
	awk -f data/generate.awk $< > $@

.PHONY: clean
clean:
	rm -rf $(OBJS) kjv
