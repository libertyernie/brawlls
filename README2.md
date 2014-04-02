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
rotation, and scale of the bone will be appended to the end of the line.

    ./brawlls.exe STGFINAL.PAC 2/ModelData[100]/3D*/stagePosition/Bones/stagePosition

    +0 CamCntrN T(0,0,0) R(0,0,0) S(1,1,1)
	+1 CamLimit0N T(-180,130,0) R(0,0,0) S(1,1,1)
	+2 CamLimit1N T(180,-60,0) R(0,0,0) S(1,1,1)
	+3 Dead0N T(-240,180,0) R(0,0,0) S(1,1,1)
	...
	
STDT nodes / values32b
----------------------

Not written yet.

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
