kjv: kjv.sh kjv.awk kjv.tsv
	cat kjv.sh > $@

	echo 'exit 0' >> $@

	echo '##SCRIPT##' >> $@
	gzip kjv.awk -c | base64 >> $@
	echo '##/SCRIPT##' >> $@

	echo '##BIBLE##' >> $@
	tail -n +2 kjv.tsv | gzip -c | base64 >> $@
	echo '##/BIBLE##' >> $@

	chmod +x $@

test: kjv.sh
	shellcheck -s sh kjv.sh

.PHONY: test
