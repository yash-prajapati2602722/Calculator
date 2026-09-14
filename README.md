# C / C++ Calculator

A small command-line scientific calculator written in C and C++. It supports
the common expression, trigonometry, logarithm, and square-root operations
found on a Casio FX-991CW.

The project focuses on C and C++. “C+” is not a separate standard language;
when people say C+, they usually mean C or C++.

> The FX-991CW is a non-programmable calculator, so C code cannot be uploaded
> to run on the physical device. This program is a compatible terminal
> practice calculator for a computer.

## Build and run

```sh
make c
./calculator
```

Build and run the C++ version:

```sh
make cpp
./calculator-cpp
```

Build both versions with `make all`.

Examples:

```text
[DEG] > sin(30) + sqrt(16)
= 4.5
[DEG] > 2^8 / 4
= 64
[DEG] > mode rad
[RAD] > pi * 2
= 6.28318530718
```

Use `help` inside the calculator for the complete command and function list.

## UI library

Open `index.html` in a browser to use the Calc Atlas interface. It includes a
working scientific keypad, DEG/RAD switching, calculation history, search, and
a timeline of notable calculators from the 1970s through the current decade.

For a local preview with the browser console available:

```sh
python3 -m http.server 4173
```

Then open `http://localhost:4173`.
