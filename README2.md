Special cases
-------------

brawlls has special behaviors for MSBin, STPMEntry, and STDT nodes (none of
which ever have children):

* MSBin - brawlls will print each line.
* STPMEntry - brawlls will print each STPM field (as defined in BrawlLib.)
* STDT - BrawlLib doesn't support STDT data, but brawlls will detect it and
  print each set of four bytes as an integer, float, ASCII string, as well as
  in raw hexadecimal format.
  * The `vals` argument can force this behavior for any node - see
    `brawlls.exe --valshelp` - and the `values32b` program wraps this
	function into a standalone program that can deal with generic data.

Additionally, when printing a model bone (MDL0BoneNode), the translation,
rotation, and scale of the bone will be appended to the end of the line,
unless you add the command --no-bone.

    ./brawlls.exe STGFINAL.PAC 2/ModelData[100]/3D*/stagePosition/Bones/stagePosition

    +0 CamCntrN T(0,0,0) R(0,0,0) S(1,1,1)
	+1 CamLimit0N T(-180,130,0) R(0,0,0) S(1,1,1)
	+2 CamLimit1N T(180,-60,0) R(0,0,0) S(1,1,1)
	+3 Dead0N T(-240,180,0) R(0,0,0) S(1,1,1)
	...
	
STDT nodes / values32b
----------------------

The default behavior for showing STDT nodes is to print every four bytes in
multiple interpretations. The columns (in order) are:
* Address
* Bytes as integer (in decimal)
* Bytes as floating point
* Bytes as ASCII data
* Bytes in raw hexadecimal format

	brawlls.exe STGMETALGEAR_00.PAC 2/MiscData[20]

	0x00:  1398031444 /  9.11679e+011 (STDT) 53544454
	0x04:           1 /   1.4013e-045 (    ) 00000001
	0x08:           0 /             0 (    ) 00000000
	0x0c:           0 /             0 (    ) 00000000
	0x10:          20 /   2.8026e-044 (    ) 00000014
	0x14:  1045220557 /           0.2 (>L  ) 3E4CCCCD
	0x18:  1053609165 /           0.4 (>   ) 3ECCCCCD
	0x1c:  1050253722 /           0.3 (>   ) 3E99999A
	0x20:  1036831949 /           0.1 (=   ) 3DCCCCCD
	...

You can also force this behavior for any data node with the "vals" command
(although it isn't usually that useful):

	brawlls.exe STGMETALGEAR_00.PAC 2/STPM vals -

	0x000:  1398034509 /   9.1188e+011 (STPM) 5354504D
	0x004:           1 /   1.4013e-045 (    ) 00000001
	0x008:           0 /             0 (    ) 00000000
	0x00c:           0 /             0 (    ) 00000000
	0x010:          20 /   2.8026e-044 (    ) 00000014
	0x014:     3939584 /  5.52053e-039 ( <  ) 003C1D00
	0x018:  1065353216 /             1 (?   ) 3F800000
	0x01c:  1109393408 /            40 (B   ) 42200000
	0x020:  1103101952 /            24 (A   ) 41C00000
	0x024:  1095761920 /            13 (AP  ) 41500000
	0x028:  1684275397 /  1.68237e+022 (dd  ) 646400C5
	...

Recursion
---------

To recursively list the contents of a data file, use the -R option:

    brawlls.exe -R STGFINAL.PAC

This command doesn't actually list *every* node - when it finds a type that
extends BRESEntryNode (such as MDL0 models or CHR0 or VIS0 animations), it
will print that node, but it won't print any of its children. To print the
children of these nodes too (and the way BrawlLib handles data, there can be a
lot of them!), use the --mdl0 flag. Additionally, you can use --deep to add
the contents of MSBin, STPMEntry, and STDT (as described above) to the
printout. Both options automatically enable -R.

    brawlls.exe --mdl0 --deep STGFINAL.PAC

Comparing files
---------------

I've included a bash script - brawlsdiff.sh - to compare data files. It runs
brawlls on two different files, with the options -m (which finds an md5sum for
each node) and --deep. Because the script uses brawlls, which is Windows-only,
as well as the Unix utility sdiff, it's designed to be run under Cygwin.

Here are the differences between Jungle Japes in Project M 3.02 (on the left)
and the original Brawl version (on the right):

	STGDXGARDEN.pac                                                 |  /cygdrive/c/Brawl/stage/melee/STGDXGARDEN.PAC
	+1 2  MD5:4EFB141E8D1F99114E8BF5C20FB664EE                      |  +1 2  MD5:E97B9A89BB14558BF351F7A15E34D644
	  +13 MiscData[2]  MD5:CB6A9F412FC487F7B8AD67271E055CB3         |    +13 MiscData[2]  MD5:F6A49BD21ACEAEC19C5CEBCC7498127A
	  +14 STPM  MD5:F47DAA30B92ED66490A7E411C4144085                |    +14 STPM  MD5:8E13A5642BA2F9C99D2CFDFFB08CCBAB
		+0 STPMEntry65  MD5:36DF923011AB8472D162C0989739CF19        |      +0 STPMEntry65  MD5:5BDA70729ABAB57CD1E15C980DC26CF9
		  MinimumTilt -3.6                                          |        MinimumTilt -2.5
		  Value15 0.2                                               |        Value15 0.15
		  Value16 0.15                                              |        Value16 0.2
		  Value17 1.125                                             |        Value17 1
		  Value19 2                                                 |        Value19 1
		  PauseCamZoomIn 40                                         |        PauseCamZoomIn 70
		  PauseCamZoomDefault 100                                   |        PauseCamZoomDefault 200
		  PauseCamRotYMin 18                                        |        PauseCamRotYMin -5
		  PauseCamRotXMin 72                                        |        PauseCamRotXMin 65
		  PauseCamRotXMax 72                                        |        PauseCamRotXMax 65
	  +15 MiscData[20]  MD5:91E934433AB99ECB9C84168617EB4486        |    +15 MiscData[20]  MD5:565CD6E8C77C7A66AEFFC596E1A63ABB
		0x14:  -998637568 /         -1000 ( z  ) C47A0000           |      0x14: -1038090240 /           -40 (    ) C2200000

(MiscData[2] is collision data, and MiscData[20] is STDT data.)

By default, brawlsdiff.sh runs *without* the --mdl0 option, but you can add it
as an argument to the script itself (if you want to check changes in
stagePosition, for example.) You can also add the -t argument to print the
type of each node.
