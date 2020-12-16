<a href="https://www.youtube.com/watch?v=2H2oUl7ufK0" title="Why the King James Bible?"><img src="https://i.imgur.com/A9piMKc.png" width="250" align="right"></a>

# kjv [![AUR](https://img.shields.io/badge/AUR-kjv--git-blue.svg)](https://aur.archlinux.org/packages/kjv-git/)

Read the Word of God from your terminal

## Usage

    usage: kjv [flags] [reference...]

    Flags:
      -A num  number of verses of context after matching verses
      -B num  number of verses of context before matching verses
      -C      show matching verses in context of the chapter
      -l      list books
      -h      show help

    Reference:
        <Book>
            Individual book
        <Book>:<Chapter>
            Individual chapter of a book
        <Book>:<Chapter>:<Verse>[,<Verse>]...
            Individual verse(s) of a specific chapter of a book
        <Book>:<Chapter>-<Chapter>
            Range of chapters in a book
        <Book>:<Chapter>:<Verse>-<Verse>
            Range of verses in a book chapter
        <Book>:<Chapter>:<Verse>-<Chapter>:<Verse>
            Range of chapters and verses in a book

        /<Search>
            All verses that match a pattern
        <Book>/<Search>
            All verses in a book that match a pattern
        <Book>:<Chapter>/<Search>
            All verses in a chapter of a book that match a pattern

## Build

kjv can be built by cloning the repository and then running make:

    git clone https://github.com/bontibon/kjv.git
    cd kjv
    make

## License

Public domain
