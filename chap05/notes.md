### Thinking of Linking

* A compiler is usually made up of several different programs responsible
for the different activities that must happen in order to transform source
code into executable object files (eg, preprocessing, code generation,
optimization, etc.)

* Specific flags can be passed to the compiler using the `-W` flag, followed
by the phase identifier, a comma, and the option itself (as in `Wl,-m` to pass
an the `-m` option to the linker).

* Dynamic linking trades off consumed disk space and compilation time in
exchange for a small runtime penalty. However, it brings benefits - small
virtual memory consumption; and sharing a copy of libraries across a system.

* Moreover, when libraries are updates, dynamically linked programs use the
new version without the need to relink them.

* Linkers add the library path in the executable itself; thus, these library
files cannot be moved to unexpected places, otherwise loading the program
will fail.

* The main purpose of dynamic linking is not having to rely on specific
versions of libraries, relying instead on their **ABI** (ie. dependency
on the interface provided by executable shared files). This means that
as long as applications do not need to be recompiled when moving to different
systems, as long as the libraries respect the same interface.

* Dynamic and static libraries are linked in a slightly different manner.
With dynamic libraries, _all_ defined symbols end up in the program address
space, whereas in static linking only the undefined symbols in a program
are searched in the archives and added to the program address space.
Moreover, the order in which arguments are processed matters. An undefined
symbol is searched on the libraries given on the command line from left to
right, and the first one found is the one that is used.

* Special care must be taken to avoid accidentally creating _interpositioning_.
This happens when a function already defined in the C standard library or in
any other included library is defined with the same name in the user program.
This causes not only local calls to the function to use the new one, but also
all other references, since the symbol namespace is global. In these situations,
either change the name to a unique one or make the function `static` so that
only the local calls will be affected.
