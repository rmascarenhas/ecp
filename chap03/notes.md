### Unscrambling Declarations in C

* C syntax for declarations can get very complicated for the programmer
(but easier for the compiler to parse). Levels of nesting, parentheses and
modifiers can make a declaration look cryptic. Example:

~~~c
char * const *(* next)();
~~~

* A declaration in C is composed by a _type specifier_ and a _declarator_.
The declarator is composed by an identifier and any pointers, function
brackets or array indications. An optional initial value is also part
of the declarator.

* Important facts: functions are not "first-class citizens", so a function
cannot return a function. Also, a function cannot return an array and an
array cannot hold a function. So constructs like `func()()`, `func()[]`
and `func[]()` are invalid. However, a function can return a _pointer_
to a function, a _pointer_ to an array and an array can hold a _pointer_
to a function. Thus, constructs like `int (* func())()`, `int (* func())[]`
and `int (* func[])()` **are** valid.

* C structs can have unnamed, bit and word-aligned fields. The syntax used
for declaring the field length is to append a colon and the number of bits that
by which the field is composed, as in:

~~~c
struct struc {
  unsigned int flag :1; /* boolean flag, only one bit is necessary */
  unsigned int :0; /* unnamed field useful to pad to next word boundary */
}
~~~

* It is implementation defined whether a bit field declared as an `int` can
be negative or not. Better to declare them with `unsigned int`.

* If a struct contains an array, then passing a struct to a function causes
the whole array to be copied over by value to the function. Also, assigning
two structs with arrays causes the whole array to be automatically copied over.

* Unions are defined as having every data within the struct to share the same
offset of zero. In other words, only one element can be stored at a time.

* No type enforcement is performed by C when reading from a union. The programmer
should know what is in there.

* A useful case of unions is to access the same data with different representations,
as in:

~~~c
union int32b {
  int whole;
  struct  { char b0, b1, b2, b3 } byte;
}
~~~

* C `enums` have a potential advantage over `#define` directives since they are
maintained past compilation. This can make debugging easier.

* `typedef`s may be useful in a few situations to give a name to a complex data
type. However, it may still cause problems. Consider just giving the variable a
meaningful name instead of giving the data type itself an alias.

* `typedef` is not quite the same as using `#define` for text substituiton. The
former works as if it created a new known type (though it is just an alias after
all), while the latter is just text replacement performed by the C preprocessor.

~~~c
#define string char *
typedef char * string2;

string s1, s2; /* expands to char * s1,  s2; probably not expected */
string2 s3, s4; /* creates two `char *` variables, as exptected */
~~~
