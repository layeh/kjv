OBJS = src/kjv_main.o \
       src/kjv_match.o \
       src/kjv_ref.o \
       src/kjv_render.o \
       src/intset.o \
       src/strutil.o \
       data/kjv_data.o
CFLAGS += -Wall -Isrc/
LDLIBS += -lreadline

kjv: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

src/kjv_main.o: src/kjv_main.c src/kjv_config.h src/kjv_data.h src/kjv_match.h src/kjv_ref.h src/kjv_render.h src/strutil.h

src/kjv_match.o: src/kjv_match.h src/kjv_match.c src/kjv_config.h src/kjv_data.h src/kjv_ref.h

src/kjv_ref.o: src/kjv_ref.h src/kjv_ref.c src/intset.h src/kjv_data.h

src/kjv_render.o: src/kjv_render.h src/kjv_render.c src/kjv_config.h src/kjv_data.h src/kjv_match.h src/kjv_ref.h

src/insetset.o: src/intset.h src/insetset.c

src/strutil.o: src/strutil.h src/strutil.c

data/kjv_data.o: src/kjv_data.h data/kjv_data.c

data/kjv_data.c: data/kjv.tsv data/generate.awk src/kjv_data.h
	awk -f data/generate.awk $< > $@

.PHONY: clean
clean:
	rm -rf $(OBJS) kjv
