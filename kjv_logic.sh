#!/bin/sh
# kjv: Read the Word of God from your terminal
# License: Public domain

SELF="$0"

get_data() {
	sed -n "/^##$1##$/,/^##\\/$1##$/p" < "$SELF" | tail -n +2 | head -n -1 | base64 -d | gunzip -c
}

if [ -z "$PAGER" ]; then
	if command -v less >/dev/null; then
		PAGER="less"
	else
		PAGER="cat"
	fi
fi

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
		get_data BIBLE | awk -v cmd=list -e "$(get_data SCRIPT)"
		exit
	elif [ "$1" = "-h" ] || [ "$isFlag" -eq 1 ]; then
		exec >&2
		echo "usage: $0 [flags] <references...>"
		echo
		echo "  -l      list books"
		echo "  -h      show help"
		echo
		echo "  References types:"
		echo "      {Book}"
		echo "          Individual book"
		echo "      {Book}:{Chapter}"
		echo "          Individual chapter of a book"
		echo "      {Book}:{Chapter}:{Verse}"
		echo "          Individual verse of a specific chapter of a book"
		echo "      {Book}:{Chapter}-{Chapter}"
		echo "          Range of chapters in a book"
		echo "      {Book}:{Chapter}:{Verse}-{Verse}"
		echo "          Range of verses in a book chapter"
		echo "      {Book}:{Chapter}:{Verse}-{Chapter}:{Verse}"
		echo "          Range of chapters and verses in a book"
		echo
		echo "      /{Search}"
		echo "          All verses that match a pattern"
		echo "      {Book}/{Search}"
		echo "          All verses in a book that match a pattern"
		echo "      {Book}:{Chapter}/{Search}"
		echo "          All verses in a chapter of a book that match a pattern"
		exit 2
	fi
	break
done

startIdx=$#
(while [ $# -gt 0 ]; do
	if [ "${startIdx}" -ne $# ]; then
		echo
	fi
	ref="$1"
	shift
	get_data BIBLE | awk -v cmd=ref -v ref="$ref" -e "$(get_data SCRIPT)"
done ) | ${PAGER}
