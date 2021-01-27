# Buildroot

## $ make

### blah blah "No targets specified and no makefile found"

Example:

```
make[1]: Entering directory '/home/josep/Desktop/buildroot-2020.11.1/output/build/gdb-8.3.1/gdb/gdbserver'
make[1]: *** No targets specified and no makefile found.  Stop.
make[1]: Leaving directory '/home/josep/Desktop/buildroot-2020.11.1/output/build/gdb-8.3.1/gdb/gdbserver'
```

- cd to the directory where the problem occured
- run ./ configure
- ... (still not conpletely solved)