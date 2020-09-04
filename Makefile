kjv: kjv.sh kjv.awk kjv.tsv
	cat kjv.sh > $@

	echo 'exit 0' >> $@

	echo '#EOF' >> $@
	tar czf - kjv.awk kjv.tsv >> $@

	chmod +x $@

test: kjv.sh
	shellcheck -s sh kjv.sh

.PHONY: test
