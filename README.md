# z80em
A z80 compiler and emulator. Still very much in development, but slowly making progress. References:

http://www.z80.info/decoding.htm

Z-80 Assembly Language Programming by Lance A. Leventhal

Currently supports:

* ld: Load into register
* inc: Increment
* dec: Decrement
* jp: Jump to address
* add: Add to register/add registers
* out: Output data (currently just outputs to stdout, since we're emulating)
* cp: Compares registers and sets flags
* halt: Halts execution
* nop: Does nothing!

Run "make" in directory to make files. Run ./run to execute assembly
