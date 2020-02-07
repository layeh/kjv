BEGIN {
	#  $1 Book name
	#  $2 Book abbreviation
	#  $3 Book number
	#  $4 Chapter number
	#  $5 Verse number
	#  $6 Verse
	FS = "\t"

	MAX_WIDTH = 80
	if (ENVIRON["KJV_MAX_WIDTH"] ~ /^[0-9]+$/) {
		if (int(ENVIRON["KJV_MAX_WIDTH"]) < MAX_WIDTH) {
			MAX_WIDTH = int(ENVIRON["KJV_MAX_WIDTH"])
		}
	}

	if (cmd == "ref") {
		mode = parseref(ref, p)
		p["book"] = cleanbook(p["book"])
	}
}

cmd == "list" {
	if (!($2 in seen_books)) {
		printf("%s (%s)\n", $1, $2)
		seen_books[$2] = 1
	}
}

function parseref(ref, arr) {
	# 1. <book>
	# 2. <book>:?<chapter>
	# 3. <book>:?<chapter>:<verse>
	# 3a. <book>:?<chapter>:<verse>[,<verse>]...
	# 4. <book>:?<chapter>-<chapter>
	# 5. <book>:?<chapter>:<verse>-<verse>
	# 6. <book>:?<chapter>:<verse>-<chapter>:<verse>
	# 7. /<search>
	# 8. <book>/search
	# 9. <book>:?<chapter>/search

	if (match(ref, "^[1-9]?[a-zA-Z ]+")) {
		# 1, 2, 3, 3a, 4, 5, 6, 8, 9
		arr["book"] = substr(ref, 1, RLENGTH)
		ref = substr(ref, RLENGTH + 1)
	} else if (match(ref, "^/")) {
		# 7
		arr["search"] = substr(ref, 2)
		return "search"
	} else {
		return "unknown"
	}

	if (match(ref, "^:?[1-9]+[0-9]*")) {
		# 2, 3, 3a, 4, 5, 6, 9
		if (sub("^:", "", ref)) {
			arr["chapter"] = int(substr(ref, 1, RLENGTH - 1))
			ref = substr(ref, RLENGTH)
		} else {
			arr["chapter"] = int(substr(ref, 1, RLENGTH))
			ref = substr(ref, RLENGTH + 1)
		}
	} else if (match(ref, "^/")) {
		# 8
		arr["search"] = substr(ref, 2)
		return "search"
	} else if (ref == "") {
		# 1
		return "exact"
	} else {
		return "unknown"
	}

	if (match(ref, "^:[1-9]+[0-9]*")) {
		# 3, 3a, 5, 6
		arr["verse"] = int(substr(ref, 2, RLENGTH - 1))
		ref = substr(ref, RLENGTH + 1)
	} else if (match(ref, "^-[1-9]+[0-9]*$")) {
		# 4
		arr["chapter_end"] = int(substr(ref, 2))
		return "range"
	} else if (match(ref, "^/")) {
		# 9
		arr["search"] = substr(ref, 2)
		return "search"
	} else if (ref == "") {
		# 2
		return "exact"
	} else {
		return "unknown"
	}

	if (match(ref, "^-[1-9]+[0-9]*$")) {
		# 5
		arr["verse_end"] = int(substr(ref, 2))
		return "range"
	} else if (match(ref, "-[1-9]+[0-9]*")) {
		# 6
		arr["chapter_end"] = int(substr(ref, 2, RLENGTH - 1))
		ref = substr(ref, RLENGTH + 1)
	} else if (ref == "") {
		# 3
		return "exact"
	} else if (match(ref, "^,[1-9]+[0-9]*")) {
		# 3a
		arr["verse", arr["verse"]] = 1
		delete arr["verse"]
		do {
			arr["verse", substr(ref, 2, RLENGTH - 1)] = 1
			ref = substr(ref, RLENGTH + 1)
		} while (match(ref, "^,[1-9]+[0-9]*"))

		if (ref != "") {
			return "unknown"
		}

		return "exact_set"
	} else {
		return "unknown"
	}

	if (match(ref, "^:[1-9]+[0-9]*$")) {
		# 6
		arr["verse_end"] = int(substr(ref, 2))
		return "range_ext"
	} else {
		return "unknown"
	}
}

function cleanbook(book) {
	book = tolower(book)
	gsub(" +", "", book)
	return book
}

function bookmatches(book, bookabbr, query) {
	book = cleanbook(book)
	if (book == query) {
		return book
	}

	bookabbr = cleanbook(bookabbr)
	if (bookabbr == query) {
		return book
	}

	if (substr(book, 1, length(query)) == query) {
		return book
	}
}

function printverse(verse,    word_count, characters_printed) {
	if (ENVIRON["KJV_NOLINEWRAP"] != "" && ENVIRON["KJV_NOLINEWRAP"] != "0") {
		printf("%s\n", verse)
		return
	}

	word_count = split(verse, words, " ")
	for (i = 1; i <= word_count; i++) {
		if (characters_printed + length(words[i]) + (characters_printed > 0 ? 1 : 0) > MAX_WIDTH - 8) {
			printf("\n\t")
			characters_printed = 0
		}
		if (characters_printed > 0) {
			printf(" ")
			characters_printed++
		}
		printf("%s", words[i])
		characters_printed += length(words[i])
	}
	printf("\n")
}

function processline() {
	if (last_book_printed != $2) {
		print $1
		last_book_printed = $2
	}

	printf("%d:%d\t", $4, $5)
	printverse($6)
	outputted_records++
}

cmd == "ref" && mode == "exact" && bookmatches($1, $2, p["book"]) && (p["chapter"] == "" || $4 == p["chapter"]) && (p["verse"] == "" || $5 == p["verse"]) {
	processline()
}

cmd == "ref" && mode == "exact_set" && bookmatches($1, $2, p["book"]) && (p["chapter"] == "" || $4 == p["chapter"]) && p["verse", $5] {
	processline()
}

cmd == "ref" && mode == "range" && bookmatches($1, $2, p["book"]) && ((p["chapter_end"] == "" && $4 == p["chapter"]) || ($4 >= p["chapter"] && $4 <= p["chapter_end"])) && (p["verse"] == "" || $5 >= p["verse"]) && (p["verse_end"] == "" || $5 <= p["verse_end"]) {
	processline()
}

cmd == "ref" && mode == "range_ext" && bookmatches($1, $2, p["book"]) && (($4 == p["chapter"] && $5 >= p["verse"] && p["chapter"] != p["chapter_end"]) || ($4 > p["chapter"] && $4 < p["chapter_end"]) || ($4 == p["chapter_end"] && $5 <= p["verse_end"] && p["chapter"] != p["chapter_end"]) || (p["chapter"] == p["chapter_end"] && $4 == p["chapter"] && $5 >= p["verse"] && $5 <= p["verse_end"])) {
	processline()
}

cmd == "ref" && mode == "search" && (p["book"] == "" || bookmatches($1, $2, p["book"])) && (p["chapter"] == "" || $4 == p["chapter"]) && match(tolower($6), tolower(p["search"])) {
	processline()
}

END {
	if (cmd == "ref" && outputted_records == 0) {
		print "Unknown reference: " ref
	}
}
