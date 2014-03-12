const char* usage_line = "Usage: brawlls [options] filename [path within file]";
const char* usage_help_line = "Run with --help or /? for more information.";

const char* usage_desc = R"(
-d, --self  list only the specified nodes, not their children (akin to ls -d)
-R          list nodes recursively (akin to ls -R)
-c          find only the first path that matches (disables wildcards and +s)
--help, /?  print this message to stdout
--xallhelp  info about the "xall" command

--no-stpm  don't list STPM values (default is --stpm)
--no-bone  don't print bone values (default is --bone)
--mdl0     always list sub-nodes of MDL0 models
--no-mdl0  never list sub-nodes of MDL0 models
The default is to show MDL0 sub-nodes when the node is the working root
(specified on the command-line) or its name ends in "osition".

-t              show node types next to names
-m              show MD5 checksums next to names
--full-path     show full path instead of name
--format="..."  overrides -t, -m, --bone, --no-bone, and --full-path;
                run "brawlls --formathelp" for more information

Elements of the inside-file path can be node names, with or without
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

const char* xall_help = R"(Usage: brawlls [OPTIONS] FILENAME PATH xall OUTPUT-DIR [extension]

The xall command extracts all direct children of a node within FILENAME,
given by PATH, to the directory OUTPUT-DIR.

The -c option is the only brawlls option that can be used with xall.

Elements of the inside-file path can be node names, with or without
wildcards (*), or indicies prefixed by "+" (for example, +0 or +17).
The + character does not need to be preceded by a slash.

Example:
mkdir outdir
brawlls common5.pac sc_selmap_en/*80]/Tex* # lists all textures
brawlls common5.pac sc_selmap_en/*80]/Tex* xall outdir png # extract to folder)";
