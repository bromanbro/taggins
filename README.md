[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.me/DorianYeager/5)

# Bilbo Taggins
Suckless tool for descriptively tagging files

## About The Project
Taggins is a command line tool that allows you to; create, read, update, and delete descripive tags for your files.  It adheres to the Unix phiosophy of tools that do one thing really well.  Taggins is meant to tie in with other command line tools providing you a way of quickly locating items of interest.

You should probably use taggins if:
* You have vast amounts of media and need advanced filtering.
* You are a researcher that needs to catagorize files.
* You are a parent that wants to launch a slideshow in < 20 seconds of all pictures with tags: Billy LittleLeaque 2019.
* You are a person who generally likes to categorize things in an arbitrary way.

## Getting Started
To use taggins you must have a file system that supports extended attributes.
Taggins maintains a string of one word tags separated by spaces in an extended file attribute.  Most (but not all) modern file systems support extended file attributes and have them enabled by default.

## Installation
1. Clone the repo
```sh
git clone https://github.com/bromanbro/taggins.git
```
2. Move into the project root and build for your system
```sh
cd taggins
./configure
make
sudo make install
```

## Contact
Dorian Yeager [LinkedIN](https://www.linkedin.com/in/dorian-yeager-346246163)

### NAME

taggins - Descriptively tag all of the things.

### SYNOPSIS

**taggins** \[-a|-r|-d\] \[-f *filepath*\] \[tag ...\]

### DESCRIPTION

**taggins** takes a stream of paths piped through stdin and performs
CRUD operations for a list of descriptive tags. Tags are a list of
single words separated by spaces. These tags are kept in the metadata
for the files themselves allowing portability. Optionally, you can
target a single file at the command line by passing **-f** *filepath*.
If no options are passed **taggins** operates in read mode. In this
default mode the complete file path is sent to stdout followed by all
tags for that file.

#### Options

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

### SEE ALSO

xattr(7), setxattr(2), getxattr(2), listxattr(2), removexattr(2)

### BUGS

This program operates by maintaining a string of space delimited tags /
words in an extended attribute named user.tags. Extended attributes are
available and enabled by default on most (but not all) modern file
systems. **Taggins** will error out and inform the user if extended
attributes are not available for a file.
