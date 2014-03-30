const char* usage_line = "Usage: brawlls [options] filename [path within file]";
const char* usage_help_line = "Run with --help or /? for more information.";

const char* usage_desc = R"(
-d, --self  list only the specified nodes, not their children (akin to ls -d)
-R          list nodes recursively (akin to ls -R)
--mdl0      descend into MDL0 and other BRES entries (implies -R)
--deep      Print STPMEntry and STDT values + MSBin strings (implies -R)
                --mdl0 and --deep behaviors are default for the root node, so
                you can specify a node in the [path within file] section to
                print it out. --deep applies the same behavior to all children.
-c          find only the first path that matches (disables wildcards and +s)
--help, /?  print this message to stdout

--xhelp     info about the "x" command
--xallhelp  info about the "xall" command
--valshelp  info about the "vals" command

-t              show node types next to names
-m              show MD5 checksums next to names
--no-bone       don't print bone values (default is --bone)
--full-path     show full path instead of name
--format="..."  overrides -t, -m, --bone, --no-bone, and --full-path;
                run "brawlls --formathelp" for more information

Elements of the [path within file] can be node names, with or without
wildcards (*), or indicies prefixed by "+" (for example, +0 or +17).
The + character does not need to be preceded by a slash.)";

const char* format_help = R"(
Default format with -t and -m: %~+%i %n %t %b %m
%t appears when -t is specified, %m appears for -m, and %b
appears by default (but can be turned off with --no-bone.)
    
Available options for --format:
%~  indentation for recursive listing (two spaces for each depth)
%i  index of node in parent (you can use this with + prefix in pathname)
%n  name of node
%p  full path of node inside file
%t  type of node
%b  trans/rot/scale of MDL0 bones (only appears for MDL0BoneNodes)
%m  MD5 checksum of original node data
%s  size in bytes of original node data
%%  literal % sign)";

const char* x_help = R"(Usage: brawlls [OPTIONS] FILENAME PATH x OUTPUT

The x command extracts the first node matching PATH within FILENAME to the
file OUTPUT. If OUTPUT starts with a period, it will be treated as the
extension, and the name of the node will be used as the filename.

The -c option is the only brawlls option that can be used with vals, and the
PATH follows the same rules as when brawlls is being used normally.

OUTPUT can be - for standard output. If you use -, the UncompressedSource of
the node will be used; the data may differ slightly when you export to a file,
due to the way BrawlLib handles export operations.

Example:
brawlls common5.pac sc_selmap_en/*80] -d # show the node that gets selected
brawlls common5.pac sc_selmap_en/*80] x .brres # extract to MiscData[80].brres)";

const char* xall_help = R"(Usage: brawlls [OPTIONS] FILENAME PATH xall OUTPUT-DIR [extension]

The xall command extracts all direct children of a node within FILENAME,
given by PATH, to the directory OUTPUT-DIR.

The -c option is the only brawlls option that can be used with vals, and the
PATH follows the same rules as when brawlls is being used normally.

Example:
mkdir outdir
brawlls common5.pac sc_selmap_en/*80]/Tex* # lists all textures
brawlls common5.pac sc_selmap_en/*80]/Tex* xall outdir png # extract to folder)";

const char* vals_help = R"(Usage: brawlls [OPTIONS] FILENAME PATH vals OUTPUT

The vals command prints out one line for every four bytes in a given node.
The format is:
[Address in hex]: [unsigned int] / [float] ([as ASCII text]) [hexadecimal]
This is also the behavior used to show STDT nodes when they are specified or
when listing recursively, because BrawlLib doesn't read STDT data.

vals uses the UncompressedSource of a node. This may differ slightly from
what "brawlls ... x" (or BrawlBox) produces when you export to a file, due
to the way BrawlLib handles export operations.

The -c option is the only brawlls option that can be used with vals, and the
PATH follows the same rules as when brawlls is being used normally.
OUTPUT can be - for standard output.

Example:
brawlls STGCRAYON.PAC 2/STPM/* # show values in first entry of STPM node
brawlls STGCRAYON.PAC 2/STPM vals - # view raw data of STPM node)";
