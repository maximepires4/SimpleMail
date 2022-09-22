% SIMPLEMAIL(1) 0.1.0-alpha
% Maxime Pires
% September 2022

# NAME
simplemail - Sending mail via terminal

# SYNOPSYS
**simplemail** [*OPTION*]... *TO* *SUBJECT* *CONTENT*

# DESCRIPTION
*simplemail* send an email to the address specified in *TO*, with the subject specified in *SUBJECT* and the content specified in *CONTENT*. Running *simplemail* for the first time causes the program to ask for the mail informations. These informations are saved at *~/.simplemailrc* and won't be re-asked unless parameter *-r* is specified.

# OPTIONS
**-r**
: Erases the config file and ask for the mail informations.

**-h**
: Shows help.

**-v**
: Shows what the program is doing.

**-c** *carbon-copy*
: Add the specified address to the carbon copy.

**-b** *blind-carbon-copy*
: Add the specified address to the blind carbon copy.

**-a** *attachment-file*
: Add the specified file to the attachment.

# EXAMPLES
**simplemail maximepires4@gmail.com "Hello World!" "Lorem ispum..."**
: Send a mail to maximepires4@gmail.com, with the subject "Hello World!" and the content "Lorem ipsum...".

**simplemail -h**
: Shows help.

**simplemail -r**
: Reconfigure config file.

# EXIT VALUES
**0**
: Success

# BUGS
Please report any issues to the following issue tracker: https://github.com/Marouk4/SimpleMail/issues

# COPYRIGHT
Copyright © 2022 Maxime Pires. License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.

# SEE ALSO
GitHub repository: https://github.com/Marouk4/SimpleMail