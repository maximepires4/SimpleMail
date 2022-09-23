% SIMPLEMAIL(1) 1.0.0
% Maxime Pires
% September 2022

# NAME
simplemail - Sending mail via terminal

# SYNOPSYS
**simplemail** [*OPTION*]... *TO* *SUBJECT* *CONTENT*

# DESCRIPTION
*simplemail* send an email to the address specified in *TO*, with the subject specified in *SUBJECT* and the content specified in *CONTENT*. Running *simplemail* for the first time causes the program to ask for the mail informations. These informations are saved at *~/.simplemailrc* and won't be re-asked unless parameter *-r* is specified.

*TO* can be specified as a simple parameter (e.g mail@mail.com) or as a string with angle brackets (e.g "<mail@mail.com>"). As a result, it is possible to send the mail to several addresses (e.g "<mail@mail.com><mail@mail.org>").

# OPTIONS
**-r**, **--reload**
: Erases the config file and ask for the mail informations.

**-h**, **--help**
: Shows help.

**-v**, **--verbose**
: Shows what the program is doing.

**-c**, **--cc** *carbon-copy*
: Add the specified address to the carbon copy. Can be transformed into a multiple addresses string using angle brackets.

**-b**, **--bcc** *blind-carbon-copy*
: Add the specified address to the blind carbon copy. Can be transformed into a multiple addresses string using angle brackets.

**-a**, **--attachment**
: Add the specified file(s) to the attachment. The program will understand as a filename ALL the arguments from after *CONTENT* to the end.

# EXAMPLES
**simplemail -a maximepires4@gmail.com "Hello World!" "Lorem ispum..." todo.txt program.c**
: Send a mail to maximepires4@gmail.com, with the subject "Hello World!" the content "Lorem ipsum...", and the attachments "todo.txt" and "program.c".

**simplemail maximepires4@gmail.com "Hello World!" "Lorem ispum..."**
: Send a mail to maximepires4@gmail.com, with the subject "Hello World!" and the content "Lorem ipsum...".

**simplemail -h | simplemal --help**
: Shows help.

**simplemail -r | simplemail --reload**
: Reconfigure config file.

# EXIT VALUES
**0**
: Success

# BUGS
Please report any issues to the following issue tracker: https://github.com/maximepires4/SimpleMail/issues

# COPYRIGHT
Copyright © 2022 Maxime Pires. License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.

# SEE ALSO
GitHub repository: https://github.com/maximepires4/SimpleMail
