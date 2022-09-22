# SimpleMail - Sending mail via terminal
Simple mail sending program written in C with libcurl.
Supports **attachment**, **carbon copy** and **blind carbon copy**..

## Installation
Requires [libcurl](https://curl.se/libcurl/).

```
sudo make
```

*Optionally* `make clean`

## Uninstall
```
sudo make uninstall
```

# Usage
`simplemail maximepires4@gmail.com "Hello World!" "Lorem ispum..."`
Send a mail to maximepires4@gmail.com, with the subject "Hello World!" and the content "Lorem ipsum...".

`simplemail -h`
: Shows help.

`simplemail -r`
: Reconfigure config file.

## Support
Please report any issues to the following issue tracker: https://github.com/Marouk4/SimpleMail/issues

## Roadmap
- Add Markdown or HTML support.
- Open mail editing screen if no parameters are specified (like git do when commiting without -m).
- Multiple TOs, CCs and BCCs

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Note: The manpage is built using [pandoc](https://pandoc.org/), with the following command:
```
pandoc simplemail.man1.md -s -t man -o simplemail.1
```

## License
[GNU v3.0](./LICENSE.md)

Copyright © 2022 Maxime Pires. License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.
