BEGIN {
	#  $1 Book name
	#  $2 Book abbreviation
	#  $3 Book number
	#  $4 Chapter number
	#  $5 Verse number
	#  $6 Verse
	FS = "\t"

	if (cmd == "ref") {
		if (match(ref, "^[1-9]?[a-zA-Z ]+$")) {
			mode = "exact"
			r_book = tolower(ref)

		} else if (match(ref, "^[1-9]?[a-zA-Z ]+:[0-9]+$")) {
			mode = "exact"

			split(ref, arr, ":")
			r_book = tolower(arr[1])
			r_chapter = arr[2]

		} else if (match(ref, "^[1-9]?[a-zA-Z ]+:[0-9]+:[0-9]+$")) {
			mode = "exact"

			split(ref, arr, ":")
			r_book = tolower(arr[1])
			r_chapter = arr[2]
			r_verse = arr[3]

		} else if (match(ref, "^[1-9]?[a-zA-Z ]+:[0-9]+-[0-9]+$")) {
			mode = "range"

			split(ref, arr, ":")
			r_book = tolower(arr[1])

			split(arr[2], arr, "-")
			r_chapter_lower = arr[1]
			r_chapter_upper = arr[2]

		} else if (match(ref, "^[1-9]?[a-zA-Z ]+:[0-9]+:[0-9]+-[0-9]+$")) {
			mode = "range"

			split(ref, arr, ":")
			r_book = tolower(arr[1])
			r_chapter_lower = r_chapter_upper = arr[2]

			split(arr[3], arr, "-")
			r_verse_lower = arr[1]
			r_verse_upper = arr[2]

		} else if (match(ref, "^[1-9]?[a-zA-Z ]+:[0-9]+:[0-9]+-[0-9]+:[0-9]+$")) {
			mode = "range_ext"

			split(ref, arr, ":")
			r_book = tolower(arr[1])
			r_start_chapter = arr[2]
			r_end_verse = arr[4]

			split(arr[3], arr, "-")
			r_start_verse = arr[1]
			r_end_chapter = arr[2]

		} else if (match(ref, "^/")) {
			mode = "search"

			r_search = substr(ref, 2)

		} else if (match(ref, "^[1-9]?[a-zA-Z ]+/")) {
			mode = "search"

			i = index(ref, "/")
			r_book = tolower(substr(ref, 1, i - 1))
			r_search = substr(ref, i + 1)

		} else if (match(ref, "^[1-9]?[a-zA-Z ]+:[0-9]+/")) {
			mode = "search"

			i = index(ref, ":")
			r_book = tolower(substr(ref, 1, i - 1))

			ref2 = substr(ref, i + 1)
			i = index(ref2, "/")
			r_chapter = substr(ref2, 0, i - 1)
			r_search = substr(ref2, i + 1)
		}
	}
}

cmd == "list" {
	if (!($2 in seen_books)) {
		printf("%s (%s)\n", $1, $2)
		seen_books[$2] = 1
	}
}

function printverse(verse,    word_count, characters_printed) {
	word_count = split(verse, words, " ")
	for (i = 1; i <= word_count; i++) {
		if (characters_printed + length(words[i]) > 72) {
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

cmd == "ref" && mode == "exact" && (tolower($1) == r_book || tolower($2) == r_book) && (r_chapter == "" || $4 == r_chapter) && (r_verse == "" || $5 == r_verse) {
	processline()
}

cmd == "ref" && mode == "range" && (tolower($1) == r_book || tolower($2) == r_book) && $4 >= r_chapter_lower && $4 <= r_chapter_upper && (r_verse_lower == "" || $5 >= r_verse_lower) && (r_verse_upper == "" || $5 <= r_verse_upper) {
	processline()
}

cmd == "ref" && mode == "range_ext" && (tolower($1) == r_book || tolower($2) == r_book) && (($4 == r_start_chapter && $5 >= r_start_verse) || ($4 > r_start_chapter && $4 < r_end_chapter) || ($4 == r_end_chapter && $5 <= r_end_verse)) {
	processline()
}

cmd == "ref" && mode == "search" && (r_book == "" || tolower($1) == r_book || tolower($2) == r_book) && (r_chapter == "" || $4 == r_chapter) && match(tolower($6), tolower(r_search)) {
	processline()
}

END {
	if (cmd == "ref" && outputted_records == 0) {
		print "Unknown reference: " ref
	}
}
