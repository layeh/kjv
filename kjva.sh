#!/bin/sh
# kjva: Read the Word of God+the Apocrypha from your terminal!
# License: Public domain

SELF="$0"

get_data() {
	sed '1,/^#EOF$/d' < "$SELF" | tar xz -O "$1"
}

if [ -z "$PAGER" ]; then
	if command -v less >/dev/null; then
		PAGER="less -R"
	else
		PAGER="cat"
	fi
fi

show_help() {
	exec >&2
	echo    "usage: $(basename "$0") [flags] [reference...]"
	echo   
	echo -e "  [33m-l[0m      list books"
	echo -e "  [32m-w[0m      no line wrap"
	echo -e "  [32m-W[0m      no line wrap (same as [32m-w[0m)"
	echo -e "  [36m-h[0m      show help"
	echo   
	echo    "  Reference types:"
	echo -e "      [31m<Book>[0m"
	echo    "          Individual book"
	echo -e "      [31m<Book>[0m:[32m<Chapter>[0m"
	echo    "          Individual chapter of a book"
	echo -e "      [31m<Book>[0m:[32m<Chapter>[0m:[33m<Verse>[0m[,[33m<Verse>[0m]..."
	echo    "          Individual verse(s) of a specific chapter of a book"
	echo -e "      [31m<Book>[0m:[32m<Chapter>[0m-[32m<Chapter>[0m"
	echo    "          Range of chapters in a book"
	echo -e "      [31m<Book>[0m:[32m<Chapter>[0m:[33m<Verse>[0m-[33m<Verse>[0m"
	echo    "          Range of verses in a book chapter"
	echo -e "      [31m<Book>[0m:[32m<Chapter>[0m:[33m<Verse>[0m-[32m<Chapter>[0m:[33m<Verse>[0m"
	echo    "          Range of chapters and verses in a book"
	echo
	echo    "      /<Search>"
	echo    "          All verses that match a pattern"
	echo -e "      [31m<Book>[0m/<Search>"
	echo    "          All verses in a book that match a pattern"
	echo -e "      [31m<Book>[0m:[32m<Chapter>[0m/<Search>"
	echo    "          All verses in a chapter of a book that match a pattern"
	exit 2
}

while [ $# -gt 0 ]; do
	isFlag=0
	firstChar="${1%"${1#?}"}"
	if [ "$firstChar" = "-" ]; then
		isFlag=1
	fi

	if [ "$1" = "--" ]; then
		shift
		break
	elif [ "$1" = "-l" ]; then
		# List all book names with their abbreviations
		get_data kjva.tsv | awk -v cmd=list "$(get_data kjva.awk)"
		exit
	elif [ "$1" = "-w" ]; then
		export KJVA_NOLINEWRAP=1
		shift
	elif [ "$1" = "-W" ]; then
		export KJVA_NOLINEWRAP=1
		shift
	elif [ "$1" = "-h" ] || [ "$isFlag" -eq 1 ]; then
		show_help
	else
		break
	fi
done

cols=$(tput cols 2>/dev/null)
if [ $? -eq 0 ]; then
	export KJVA_MAX_WIDTH="$cols"
fi

if [ $# -eq 0 ]; then
	if [ ! -t 0 ]; then
		show_help
	fi

	# Interactive mode
	while true; do
		printf "[33mWhat would you like to read?[0m "
		if ! read -r ref; then
			break
		fi
		get_data kjva.tsv | awk -v cmd=ref -v ref="$ref" "$(get_data kjva.awk)" | ${PAGER}
	done
	exit 0
fi

get_data kjva.tsv | awk -v cmd=ref -v ref="$*" "$(get_data kjva.awk)" | ${PAGER}
