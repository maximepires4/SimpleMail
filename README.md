# SimpleMail - Sending mail via terminal
Simple mail sending program written in C using [curl](https://curl.se).

Supports **attachment**, **carbon copy** and **blind carbon copy**.

## Table of contents

* [Features](#features)
* [Installation](#installation)
* [Uninstall](#uninstall)
* [Parameters](#parameters)
  * [Passwords](#passwords)
* [Usage](#usage)
* [Examples](#examples)
* [Support](#support)
* [Roadmap](#roadmap)
  * [Changelog](#changelog)
* [Contributing](#contributing)
* [License](#license)

## Features

* *Subject* and *content* can be written as string for sentences.
* *To*, *carbon copy* and *blind carbon copy* can be multiplied by using string and brackets, separated by **comma** (e.g "\<mail@mail.com>,\<mail@mail.org>")
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

## Parameters
At first launch the program will ask for parameters and will store them at `~/.simplemailrc`.

```
NAME=Your Name				# e.g Maxime Pires
USERNAME=yourusername			# e.g (for google) What is before the @gmail.com, like "mail" for "mail@gmail.com"
PASSWORD=yourverysecretapppassword	# BEWARE: Do NOT use your real password here, prefer using app passwords
MAIL=yourmailaddress			# e.g mail@gmail.com
SMTP=smtpaddress:port			# e.g (for google) smtp.gmail.com:587
```

### Passwords

**BEWARE**, do **NOT** use your real password, prefer using app passwords (Most hosts will refuses the connection with the real password anyway).

For setting up app passwords, search the web for the right procedure for your host.

For example, follow this [link](https://support.google.com/accounts/answer/185833?hl=en) for setting up password applications for gmail addresses.

## Usage
See [manpage](docs/simplemail.man1.md).

## Support
Please report any issues to the following issue tracker: https://github.com/maximepires4/SimpleMail/issues

## Roadmap
- Add Markdown or HTML support.
- Open mail editing screen if no parameters are specified (like `git commit` does).
- Multiple TOs, CCs and BCCs without specifying angle brackets.

### Changelog
See [changelog](CHANGELOG.md).

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
