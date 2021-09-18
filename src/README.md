# Source files for `efm-data-xtract`

`main.cpp` is the entry point for the code. The FFMpeg related static function defined in `main.cpp` should be used as blackboxes.
`Raga.cpp` contains definitions of class methods and functions defined in `Raga.h`. This file contains a bulk of the parsing code, and is a true successor of `main.cpp` in the flow of code.

The other files, viz. `fileops.cpp`, `notationops.cpp` and `stringops.cpp` are utility modules.
- `fileops.cpp` is concerned with filesystem related operations.
- `stringops.cpp` defines some general string operations
- `notationops.cpp` applies some specific operations to the _svara_ in the notations. These operations cannot be generalised for all strings, hence the separate file.

The file `filter.cpp` contains the boilerplate code for the ShowCQT filter to work efficiently along with this parser. It is strongly advised not to modify it unless extremely necessary
