Pleasant Tone Generator
=======================

This is a C port of the Awk script referenced at

http://kmkeen.com/awk-music/2011-03-07-20-10-07-394.html

The tone generation is largely the same, but the note selection is
now hardcoded.  Additionally, the program outputs two channels of
16-bit unsigned int audio.


Usage
-----

Compile:

    make

Then run.  It sends data to stdout, so pipe it through something to play
back the data.

    ./notes-v2 | aplay -f cd -

or

    ./notes-v2 | paplay --raw


Differences from the original
-----------------------------

The original program produced tones that sounded slightly detuned.  This is
because it divided the scale into five equal parts.

This port modifies this algorithm by hardcoding the five basic notes,
and then making all subsequent notes multiples of these.  By doing this,
we can have finer control over the pitches.

A copy of the C-ified original can be found in "notes-v1.c".

The original awk script is available in "notes.sh"
