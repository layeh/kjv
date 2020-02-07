![KJVA - The Psalm Update!](https://raw.githubusercontent.com/Matthew-Tate-Scarbrough/kjva/master/psalm-update.png)

kjv
===

Read the Word of God from your terminal!

Forked from [https://github.com/LukeSmithxyz/kjv.git](https://github.com/LukeSmithxyz/kjv.git), which is forked from [https://github.com/bontibon/kjv.git](https://github.com/bontibon/kjv.git) but with the Apocrypha added.

Smith added the apocrypha and made a better Makefile, however, I have removed it in this branch, and have added a "-w" option, over the "-W."
Recently, I have also added the Psalm headers and Alephbet to the Psalms.


Branches
--------

I am adding branches as I make them, to download these, use:

    git clone https://github.comf/Matthew-Tate-Scarbrough/kjva.git --branch <branch name>

Current list of branches:

    lut - Martin Luther's 1545 Bible
    kjv - Intended to be an improved version of Bontibon's KJV pure

Recommendation
--------------

I recommend when using multiple language texts in your terminal that you use a monospace font that support multiple languages; the FSF's FreeMono has almost 4,200 unique glyphs and their FreeSerif--almost 10,600 (though it's not mono-space).

You may also want to look fonts in a specific desired language.

Usage
-----

    usage: ./kjv [flags] [reference...]

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

Build
-----

I recommend creating a folder in your ~/Downloads, called ~/Downloads/.src for all source code; if you plan on using other people's forks of a particular software, like bontibon's "kjv," then make a subfolder appropriately named.

    mkdir ~/Downloads/.src
    mkdir ~/Downloads/.src/cmd-bibles

Now, clone kjva's repository and then run make:

    git clone https://github.com/Matthew-Tate-Scarbrough/kjva.git ~/Downloads/.src/cmd-bibles/kjva
    cd ~/Downloads/.src/cmd-bibles/kjva
    sudo make install

For the Luther 1545 branch, do:

    git clone https://github.com/Matthew-Tate-Scarbrough/kjva.git --branch lut ~/Downloads/.src/cmd-bibles/lut
    cd ~/Downloads/.src/cmd-bibles/lut
    sudo make install

For the KJV-pure branch, do:

    git clone https://github.com/Matthew-Tate-Scarbrough/kjva.git --branch kjv ~/Downloads/.src/cmd-bibles/kjv-mts
    cd ~/Downloads/.src/cmd-bibles/lut
    sudo make install

## License

Public domain
