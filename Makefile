PREFIX = /usr/local

kjva: kjva.sh kjva.awk kjva.tsv
	cat kjva.sh > $@
	echo 'exit 0' >> $@
	echo '#EOF' >> $@
	tar cz kjva.awk kjva.tsv >> $@
	chmod +x $@

test: kjva.sh
	shellcheck -s sh kjva.sh

clean:
	rm -f kjva

install: kjva
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f kjva $(DESTDIR)$(PREFIX)/bin/
	cp -f kjv2latex $(DESTDIR)$(PREFIX)/bin/
	chmod 755 $(DESTDIR)$(PREFIX)/bin/kjva
	chmod 755 $(DESTDIR)$(PREFIX)/bin/kjv2latex

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/kjva
	rm -f $(DESTDIR)$(PREFIX)/bin/kjv2latex

.PHONY: test clean install uninstall
