values32b
=========

This program shares code with brawlls, but compiles under standard C++ and
operates on raw data (from a file or from standard input.)

Usage:
	values32b filename
    some-command | values32b

The second version reads from standard input.

values32b outputs one line for every four bytes, in the format:

* Address (variable number of characters)
* Bytes as integer (in decimal)
* Bytes as floating point
* Bytes as ASCII data
* Bytes in raw hexadecimal format

The code was built to read data files from Super Smash Bros. Brawl ttat store
32-bit integers and floating-point values.

Sample
------

	echo hello| values32b
	Addr: UnsignedInt / FloatingPoint (Text) HexBytes
	-------------------------------------------------
	0x00:  1751477356 /  4.33369e+024 (hell) 68656C6C
	0x04:                             (o   ) 6F0A

Sample
------

	values32b file.png # a 30x36 PNG file made with mspaint
	 Addr: UnsignedInt / FloatingPoint (Text) HexBytes
	--------------------------------------------------
	0x000: -1991225785 / -2.50739e-033 ( PNG) 89504E47
	0x004:   218765834 /  4.25559e-031 (    ) 0D0A1A0A
	0x008:          13 /  1.82169e-044 (    ) 0000000D
	0x00c:  1229472850 /        820293 (IHDR) 49484452
	0x010:          30 /   4.2039e-044 (    ) 0000001E
	0x014:          36 /  5.04467e-044 (   $) 00000024
	0x018:   134348800 /  3.91205e-034 (    ) 08020000
	0x01c:     1159965 /  1.62546e-039 (    ) 0011B31D
	0x020:  1040187392 /         0.125 (>   ) 3E000000
	0x024:    24334919 /  4.46911e-038 ( sRG) 01735247
	0x028:  1107341006 /       32.1707 (B   ) 4200AECE
	0x02c:   485031936 /  1.54186e-021 (    ) 1CE90000
...

	0x150:   668883456 /   6.1715e-015 (' Z ) 27DE5A00
	0x154:          73 /  1.02295e-043 (   I) 00000049
	0x158:  1162757294 /       3300.29 (END ) 454E44AE
	0x15c:                             (B`  ) 426082

When reading from a file, values32b will put enough zeroes in the address
column at the beginning to avoid having to add any later.
