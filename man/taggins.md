# NAME

taggins - Descriptively tag all of the things.

# SYNOPSIS

**taggins** \[-a|-r|-d\] \[-f *filepath*\] \[tag ...\]

# DESCRIPTION

**taggins** takes a stream of paths piped through stdin and performs
CRUD operations for a list of descriptive tags. Tags are a list of
single words separated by spaces. These tags are kept in the metadata
for the files themselves allowing portability. Optionally, you can
target a single file at the command line by passing **-f** *filepath*.
If no options are passed **taggins** operates in read mode. In this
default mode the complete file path is sent to stdout followed by all
tags for that file.

## Options

  - **-a**  
    Add tags. All tags passed are added to the list of tags on a given
    file.

  - **-r**  
    Remove tags. All tags passed are removed from the list of tags on a
    given file.

  - **-d**  
    Delete all tags. All tags on a given file are removed along with the
    extended attribute that originally stored them.

  - **-f**  
    Specify a file. Operate on the file given at the command line rather
    than processing files piped through stdin.

# SEE ALSO

xattr(7), setxattr(2), getxattr(2), listxattr(2), removexattr(2)

# BUGS

This program operates by maintaining a string of space delimited tags /
words in an extended attribute named user.tags. Extended attributes are
available and enabled by default on most (but not all) modern file
systems. **Taggins** will error out and inform the user if extended
attributes are not available for a file.
