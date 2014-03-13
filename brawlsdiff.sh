#!/bin/bash
# brawlsdiff - shell script to find differences in 2 brawl files.
# brawlls.exe gets its files from stdin, so it's OK to use cygwin paths if you run from a cygwin shell.
# Any arguments after the first two will be sent to both instances of brawlls.
f1="$1"
shift
f2="$1"
shift
sdiff -s <(< "$f1" ~/brawlls.exe -Rm $* -) <(< "$f2" ~/brawlls.exe -Rm $* -)
