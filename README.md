# wk - A Tag-based Command Line Personal Note Manager in C++

## Why this exists

I accreted thousands of links, small notes, and other snippets first
in the now defunct del.icio.us, then in org-mode and various versions
of Tiddlywiki. Those have dependencies-- a web browser, emacs, a
running process, javascript, etc.-- which is less than ideal for how I
want to work. I want to quickly add, edit, and search my clippings at
the command line using a single binary, a single compact data file,
and to transfer them between systems.

`wk` is an experiment. It is something less than either a Wiki or a
Personal Information Manager (PIM). It lacks WikiWord-style linking
and imposes no organization other than the user-supplied tags and the
entry titles. This works for me.

## Building

Prequisites:

- C++17 compiler
- cmake 3.13+
- sqlite3

```
$ git clone https://git.sr.ht/~rlonstein/wk.git
$ cd wk
$ ./update-submodules.sh
$ mkdir build && cd build
$ cmake ..
$ make
```
Then copy the binary `wk` somewhere in your path.

## Usage

`$ wk new`

```
$ wk add --title Zork --tags interactive-fiction infocom adventure game \
    --text "http://infocom-if.org/downloads/downloads.html"
```

`$ wk search game`

## Importing Entries

`$ wk import <path/to/file>`

A collection of JSON entries should be in the following form, single
entries are also accepted:

```
[
  {
    "title": "<title>",
    "created": "<YYYY-MM-DDThh:mm::ss-UTF_OFFSET>",
    "modified": "<YYYY-MM-DDThh:mm::ss-UTF_OFFSET>",
    "text": "<contents>",
    "tags": [ "<tag1>", "<tag2>", ... ],
  },
  ...
]
```

Entries must have `title`, `text`, and `tags` fields. The `created` or
`modified` fields are optional, and will filled using the current
system time.

## TODO

- &#x2610; Tests
- &#x2610; Editing tags
- &#x2610; Import/Export YAML
- &#x2610; Import HTML bookmarks
- &#x2610; Export Markdown (indirectly HTML output)
- &#x2610; DB maintenance
- ? Categories (tags mapped to tags)
- ? Option to build/use project sqlite3 (amalgam) instead of system version
