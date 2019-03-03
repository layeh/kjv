PREFIX = /usr/local

kjv: kjv.sh kjv.awk kjv.tsv
	cat kjv.sh > $@
	echo 'exit 0' >> $@
	echo '#EOF' >> $@
	tar cz kjv.awk kjv.tsv >> $@
	chmod +x $@

test: kjv.sh
	shellcheck -s sh kjv.sh

clean:
	rm -f kjv

install: kjv
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f kjv $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/kjv

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/kjv

.PHONY: test clean install uninstall
