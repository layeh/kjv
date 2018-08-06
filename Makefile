kjv: kjv_logic.sh kjv.awk kjv.tsv
	cat kjv_logic.sh > $@

	echo 'exit 0' >> $@

	echo '##SCRIPT##' >> $@
	gzip kjv.awk -c | base64 >> $@
	echo '##/SCRIPT##' >> $@

	echo '##BIBLE##' >> $@
	tail -n +2 kjv.tsv | gzip -c | base64 >> $@
	echo '##/BIBLE##' >> $@

	chmod +x $@

test: kjv kjv_logic.sh
	shellcheck -s sh kjv_logic.sh

.PHONY: test
