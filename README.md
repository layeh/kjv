# kjv [![AUR](https://img.shields.io/badge/AUR-kjv--git-blue.svg)](https://aur.archlinux.org/packages/kjv-git/)

Read the Word of God from your terminal

## Usage

    usage: ./kjv [flags] [reference...]

      -l      list books
      -W      no line wrap
      -h      show help

      Reference types:
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

    To make different versions: 
      make kjv
      make darby
      make chiuns

    - To make the naming easier, can rename these binaries, such as:  mv chiuns cus 
      These binaries can be put at a place on the path for usage (such as ~/.local/bin/ etc. )

## License

Public domain

## Develop

Here are the notes for further develop this commandline tool.
    Format of the kjv.tsv - it's tab seperated bible verses, one line per verse:
       - column 1: Book title: Genesis
       - column 2: Abbreviated book title: Ge
       - column 3: Book number: For genesis, it's 1
       - column 4: chapter number
       - column 5: verse number
   
   Convert other Bible versions to this tsv format:
   
    - One way is to use diatheke (included in sword project), use the first 4 columns, to query the verse in the other version, to build a tsv Bible file.
        : diatheke -b Darby -o M -k John 3:1
        : This command return 2 lines: line 1: 
            the verse together with the John 3:16 as index 
                This line of returned text, truncate the first several chars which is the index for the verse: we can use the same length + 1 to remove the verse index.
                
            the second line is the (Darby) version: this second line can be omitted when reformatting for the Darby (or other different version of the Bible).
            
