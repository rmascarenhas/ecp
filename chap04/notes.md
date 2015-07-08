### The Shocking Truth: C Arrays and Pointers Are NOT the Same

* Considering pointers and arrays to be the same thing is a common abstraction
that holds in many situations, but is also flawed in key aspects. One must
understand their difference in order to avoid mistakes.

* A variable cannot be defined to be a pointer and later declared as an array
(or vice-versa).

* `lvalue` and `rvalue` are important concepts here. The former refers to an
**address**, which is _known at compile time_; it defines where the value is
to be stored. The latter is the _contents_ of an address, and _is not known
until runtime_.

* C defines the concept of a _modifiable `lvalue`_ - that is, `lvalues` that
are permitted to be used at the left side of an assignment.

* Array addresses are static and known at compile time. In contrast, the current
value of a pointer can be changed and is retrieved at runtime. For that reason,
**arrays are not modifiable `lvalue`s**.

* Only the definition of an array must contain its size limit. Later declarations
can merely contain the type information, without the size (being an _incomplete type_).

* A pointer definition - by contrast with a array definition - does **not** allocate
memory for what it is pointing at. However, there is one exception: if you assign a
pointer to `char` to a string literal, then _memory for that string will be allocated_.
ANSI C defines that the memory in which the string is allocated must be read-only,
and compilers typically use the text-segment to store such strings.
