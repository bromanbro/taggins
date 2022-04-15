[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.me/DorianYeager/5)

# Bilbo Taggins
A command line tool for descriptively tagging files

## About The Project
Taggins is a command line tool that allows you to; create, read, update, and delete descripive tags for your files.  It adheres to the Unix philosophy of tools that do one thing really well.  Taggins is meant to tie in with other command line tools providing you a way of quickly locating items of interest.

You should probably use taggins if:
* You are a linux user.
* You like stringing together powerful commands in the terminal.
* You have vast amounts of media and need a powerful tool to help you categorize.
* You are a researcher that needs to catagorize files.
* You are a parent that wants to launch a slideshow in < 20 seconds of all pictures with tags: Chester LittleLeague 2019.
* You are a person who likes to categorize things in arbitrary ways.

You may want to move along if:
* You work in Windows exclusively.
* You do not like the command line and the sight of a terminal fills you with chagrin.

## Getting Started
This application expects some kind of POSIX environment.  Linux is nice and I hear that it's free for the next 24 hours.

To use taggins you must have a file system that supports extended attributes.
Taggins maintains a string of one word tags separated by spaces in an extended file attribute.  Most (but not all) modern file systems support extended file attributes and have them enabled by default.  Don't worry, taggins will let you know.

## Installation in any Linux environment
1. Clone the repo
```sh
git clone https://github.com/bromanbro/taggins.git
```
2. Move into the project root and build for your system
```sh
./configure
make
sudo make install
```

## Usage
[Video Demonstration](https://www.youtube.com/watch?v=gsZpmVI-fp4)

## Some Examples
Add tags to a single file on the command line
```sh
taggins -af file1.jpg Florida SummerTime 1975
```

Add tags to all the files in set
```sh
find . -type f | taggins -a WestCoast 2019 MusicFestival
```

Display the tags for a set of files
```sh
find . -type f | taggins
```

Remove all tags for all files in a set
```sh
find . -type f | taggins -d
```

Build a slideshow using fzf and taggins
```sh
feh `find . -type f | taggins | fzf -m | sed 's/ tags:\{.*\}//'`
```

## Roadmap
- [x] Release usage videos demoing taggins with other command line tools.
- [ ] Create a Brew package for installation on Macintosh.

## Man Page
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

## Contact
Dorian Yeager [LinkedIn](https://www.linkedin.com/in/dorian-yeager-346246163)
