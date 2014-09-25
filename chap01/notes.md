### C Through the Mists of Time

**Historical bits**
  * C was designed with the compiler developer in mind (the main language "customer" at
  the time). This led the language to include (or not include) certain features that would
  make the compiler developer's life easier. Examples include array indexing starting from 0
  and direct mapping between data types and types supported by the processor.

* White spaces makes a difference when declaring macros. Depending on how you declare it,
  you might get unexpected results. Example:

~~~c
#define Macro(x) LongFunc(x)
#define BadMacro (x) LongFunc(x)

int main() {
  int x;
  Macro(x);
  BadMacro(x);
}
~~~

  If we expand the above code (`gcc -E`), the generated code is

~~~c
int main() {
  int x;
  LongFunc(x);
  (x) LongFunc(x)(x);
}
~~~

  Beware.

* The ANSI C standard had the purpose of creating a language that could be developed in any system,
  after variants started appearing. It was derived from K&R C manual, but expanded to include
  more features, keywords and behaviors.

* The `const` keyword does **not** make a constant variable (or symbol, for that matter). Instead,
  it only prohibits assigment _through that variable_. A more proper name for it would be `readonly`.

* Double care with comparisons, especially between signed and unsigned types. The automatic promotion
  rules can be confusing and cause subtle bugs. When in doubt, perform explicit type casts to avoid
  unexpected results. Also, avoid using an `unsigned` type just because its value "cannot be negative".
  It is safer to just use an `int` variable and avoid all sorts os problems. Keep unsigned types to
  bitmasks.
