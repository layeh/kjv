kjv: kjv.sh kjv.awk kjv.tsv
	cat kjv.sh > $@
	echo 'exit 0' >> $@
	echo '#EOF' >> $@
	tar cz kjv.awk kjv.tsv >> $@
	chmod +x $@

darby: darby.sh kjv.awk darby.tsv
	cat darby.sh > $@
	echo 'exit 0' >> $@
	echo '#EOF' >> $@
	tar cz kjv.awk darby.tsv >> $@
	chmod +x $@

chiuns: chiuns.sh kjv.awk chiuns.tsv
	cat chiuns.sh > $@
	echo 'exit 0' >> $@
	echo '#EOF' >> $@
	tar cz kjv.awk chiuns.tsv >> $@
	chmod +x $@

test: kjv.sh
	shellcheck -s sh kjv.sh

.PHONY: test
