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
  * [Changelog](#changelog)
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
See [manpage](docs/simplemail.man1.md).

## Support
Please report any issues to the following issue tracker: https://github.com/Marouk4/SimpleMail/issues

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
