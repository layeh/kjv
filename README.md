# kjv [![AUR](https://img.shields.io/badge/AUR-kjv--git-blue.svg)](https://aur.archlinux.org/packages/kjv-git/)

Read the Word of God from your terminal

# Usage

    usage: ./kjv [flags] <references...>

      -l      list books
      -h      show help

      References types:
          <Book>
              Individual book
          <Book>:<Chapter>
              Individual chapter of a book
          <Book>:<Chapter>:<Verse>
              Individual verse of a specific chapter of a book
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

## License

Public domain
