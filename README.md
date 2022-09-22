# SimpleMail - Sending mail via terminal
Simple mail sending program written in C using [curl](https://curl.se).

Supports **attachment**, **carbon copy** and **blind carbon copy**.

## Table of contents

* [Features](#features)
* [Installation](#installation)
* [Uninstall](#uninstall)
* [Usage](#usage)
  * [Parameters](#parameters)
* [Examples](#examples)
* [Support](#support)
* [Roadmap](#roadmap)
* [Contributing](#contributing)
* [License](#license)

## Features

* *Subject* and *content* can be written as string for sentences.
* *To*, *carbon copy* and *blind carbon copy* can be multiplied by using string and brackets (e.g "\<mail@mail.com>\<mail@mail.org>")
* Multiple *attachments* specifying the files's name.

## Installation

Requires [libcurl](https://curl.se/libcurl/).

```bash
sudo make
make clean # Optional
```

### Uninstall

```bash
sudo make uninstall
```

## Usage

**simplemail** [*OPTION*]... *TO* *SUBJECT* *CONTENT*

*simplemail* send an email to the address specified in *TO*, with the subject specified in *SUBJECT* and the content specified in *CONTENT*. Running *simplemail* for the first time causes the program to ask for the mail informations. These informations are saved at *~/.simplemailrc* and won't be re-asked unless parameter *-r* is specified.

*TO* can be specified as a simple parameter (e.g mail@mail.com) or as a string with angle brackets (e.g "\<mail@mail.com>"). As a result, it is possible to send the mail to several addresses (e.g "\<mail@mail.com>\<mail@mail.org>").

### Parameters

​	**-r**

Erases the config file and ask for the mail informations.

​	**-h**

Shows help.

​	**-v**

Shows what the program is doing.

​	**-c** *carbon-copy*

Add the specified address to the carbon copy. Can be transformed into a multiple addresses string using angle brackets.

​	**-b** *blind-carbon-copy*

Add the specified address to the blind carbon copy. Can be transformed into a multiple addresses string using angle brackets.

​	**-a**

Add the specified file(s) to the attachment. The program will understand as a filename ALL the arguments from after *CONTENT* to the end.

## Examples
```bash
simplemail maximepires4@gmail.com "Hello World!" "Lorem ispum..."
```
Send a mail to maximepires4@gmail.com, with the subject "Hello World!" and the content "Lorem ipsum...".

```bash
simplemail -a maximepires4@gmail.com "Hello World!" "Lorem ispum..." todo.txt program.c
```
Send a mail to maximepires4@gmail.com, with the subject "Hello World!" the content "Lorem ipsum...", and the attachments "todo.txt" and "program.c".

```bash
simplemail -h
```
Shows help.

```bash
simplemail -r
```
Reconfigure config file.

## Support
Please report any issues to the following issue tracker: https://github.com/Marouk4/SimpleMail/issues

## Roadmap
- Add Markdown or HTML support.
- Open mail editing screen if no parameters are specified (like `git commit` does).
- Multiple TOs, CCs and BCCs without specifying angle brackets.

### Already added
- Multiple attachments (Done in 0.2.0)

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Note: Manpage is built using [pandoc](https://pandoc.org/), with the following command:
```bash
pandoc docs/simplemail.man1.md -s -t man -o docs/simplemail.1
```

## License
[GNU v3.0](./LICENSE.md)

Copyright © 2022 Maxime Pires. License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.
