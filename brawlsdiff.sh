#!/bin/bash
# brawlsdiff - shell script to find differences in 2 brawl files.
# brawlls.exe gets its files from stdin, so it's OK to use cygwin paths if you run from a cygwin shell.
# Any arguments after the first two will be sent to both instances of brawlls.

width_arg=""
if [ "$1" = "-w" ];then
	width_arg="-w $2"
	if [ "$2" = "full" ];then
		width_arg="-w $(stty size|awk '{print $2}')"
	fi
	shift
	shift
fi
f1="$1"
f2="$2"
shift
shift
brawlls_args="$*"
sdiff $width_arg -s <(< "$f1" ~/brawlls.exe -Rm $brawlls_args -) <(< "$f2" ~/brawlls.exe -Rm $brawlls_args -)
