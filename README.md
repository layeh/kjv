# kjva

Read the Word of God + the Apocrypha from your terminal!

Forked from [https://github.com/LukeSmithxyz/kjv.git](https://github.com/LukeSmithxyz/kjv.git), which is forked from [https://github.com/bontibon/kjv.git](https://github.com/bontibon/kjv.git) but with the Apocrypha added.

Smith added the apocrypha and made a better Makefile, however, I have rebranded it as "kjva," meaning kjv+apocyrpha, to avoid confusion with the original by bontibon, and have added a "-w" option, over the "-W."

## Usage

    usage: ./kjva [flags] [reference...]

      -l      list books
      -w      no line wrap
      -W      no line wrap (same as -w)
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

I recommend creating a folder in your ~/Downloads, called ~/Downloads/.src for all source code; if you plan on using other people's forks of a particular software, like bontibon's "kjv," then make a subfolder appropriately named.

    mkdir ~/Downloads/.src
    mkdir ~/Downloads/.src/cmd-bibles

Now, clone kjva's repository and then run make:

    git clone https://github.com/Matthew-Tate-Scarbrough/kjva.git ~/Downloads/.src/cmd-bibles/kjva
    cd ~/Downloads/.src/cmd-bibles/kjva
    sudo make install

## License

Public domain
