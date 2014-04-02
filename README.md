brawlls
=======

Recursive listing of SSBB data file contents from the command line. (C++/CLI)

This repository also contains a C++ program named values32b that compiles with
Visual Studio as well as g++. The code in values32b.cpp is also included in
brawlls - primarily to describe STDT data, which is not supported directly by
BrawlLib but occasionally used to modify stage files.

Normal operation
----------------

A normal brawlls command line takes the format:

    brawlls.exe [options] filename [path within file]

The options are described by running `brawlls.exe /?`. `filename` can be `-`
(the minus character), in which case brawlls will read the standard input into
a temporary file, work from there, and delete the file when it's done.
`[path within file]` describes the particular node in the file you want to
list the children of. For example, the command:

    brawlls.exe FitKirby00.pcs TextureData[0]/Textures(NW4R)

lists the textures in Kirby's default color - the same thing you would see if
you opened FitKirby00.pcs with BrawlBox and navigated to TextureData[0] ->
Textures(NW4R). In this case, the output is:

	+0 PlyKirby5KEye.1
	+1 PlyKirby5KEye.2
	+2 PlyKirby5KEye.3
	+3 PlyKirby5KEye.4
	+4 PlyKirby5KEyeYellow.1
	+5 PlyKirby5KEyeYellow.2
	+6 PlyKirby5KEyeYellow.3
	+7 PlyKirby5KEyeYellow.4
	+8 PlyKirby5KFoot
	+9 PlyKirby5KMouth
	+10 PlyKirby5KSkin

By index
--------

brawlls also supports choosing a node based on its index within the file. The
+0 next to PlyKirby5KEye.1 in the line above means that PlyKirby5KEye.1 is the
first child of Textures(NW4R) (the second child is +1, and so on.) Since
TextureData[0] is the second node in Fitkirby00.pcs, and Textures(NW4R) is the
only folder inside it, we could have typed this command for the same result:

    brawlls.exe FitKirby00.pcs +1/+0

brawlls will even add a slash before a plus if there isn't one, so you can
also do this:

	brawlls.exe FitKirby00.pcs +1+0

Wildcards are supported in node names - however, brawlls.exe throws an error
if more than one node matches the wildcard. Also note that you can combine
names and indicies in the same path.

	brawlls.exe STGFINAL.PAC +1/Tex*

Usage example
-------------

    brawlls.exe STGDXCORNERIA.pac

This command gives:

	+0 1
	+1 2
	+2 ef_StgDxCorneria

We're interested in the node "2", and we might want also to know what type
each object is, so we'll add the -t argument.

    brawlls.exe STGDXCORNERIA.pac 2 -t

Among other things, we see:

    ...
    +15 MiscData[0] (MSBinNode)
    +16 MiscData[1] (MSBinNode)
    ...

MiscData[1], at index 16, is an MSBinNode, so we can read its contents. To do
this, just run:

    brawlls.exe STGDXCORNERIA.pac 2+16

In this command, "2" is a name, because it doesn't come after a plus (+), but
"16" is an index, because it does. This command prints:

	Wow! I almost feel like going
	for a swim!
	You swim in the ocean, too?
	Don't make fun of me,
	birdbrain!
	Ahh, to be young again...
	Fox! Be careful! I have a bad
	feeling about this.

...and so on. (See README2.md for more information about MSBin nodes and other
special cases.)

Extracting files
----------------

You can use the "x" command to extract the node specified to a file, rather
than listing its children.

    brawlls.exe STGFINAL.PAC 2/Texture*/*/planet01 x planet01.png

You can also use the "xall" command to extract all of a node's children. This
command also lets you specify an extension, which is useful for textures.

    brawlls.exe STGFINAL.PAC 2/Texture*/*/planet01 xall my_textures_folder png

More information
----------------

Documentation of more advanced and/or confusing features can be found in README2.md.
