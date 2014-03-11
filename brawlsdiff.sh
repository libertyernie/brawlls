#!/bin/sh
# brawlsdiff - shell script to find differences in 2 brawl files.
# brawlls.exe gets its files from stdin, so it's OK to use cygwin paths if you run from a cygwin shell.
# Any arguments after the first two will be sent to both instances of brawlls.
f1="$1"
shift
f2="$1"
shift
< "$f1" ~/brawlls.exe -Rm $* - > /tmp/brawlcmp-a
< "$f2" ~/brawlls.exe -Rm $* - > /tmp/brawlcmp-b
sdiff -s /tmp/brawlcmp-a /tmp/brawlcmp-b
rm /tmp/brawlcmp-a /tmp/brawlcmp-b 2> /dev/null
