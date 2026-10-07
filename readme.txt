Sehnsucht-System
================
Programming as an art workshop for retarded children~~ ^-^

Hopefully this is the last one and the one I will work on 
long in the future, but you never know, so I won't make
any promises; not that anybody would care anyway.

Depedencies
-----------
- GNU GCC cross-compiler + Binutils (+ GDB for debugging)
- Netwide Assembler
- mtools, dosfstools, parted
- (git)
- qemu for x86_64 to run it
- Lua (for running the additional scripts in ./scripts/)

Usage
-----
$ make build_limine
$ make build
$ make run # for normal usage
$ make debug # for debugging (you need to connect with gdb)

Copying
-------
GPLv3 (see COPYING.txt)
