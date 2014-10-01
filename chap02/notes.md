### It's Not a Bug, It's a Language Feature

* `NUL` vs `NULL`: the former indicates the ASCII character with the bit pattern
  of zero that terminates a string; the latter is the name of the pointer that points
  to nothing. They mean different things and **cannot** be used interchangeably.

* C automatically concatenates consecutive strings. However, this can be dangerous
  when, for example, declaring an array of strings:

~~~c
char *strings[] = {
  "first",
  "second" /* no comma! */
  "third",
  "fourth"
}
~~~

  The above code will issue no error or warning since automatic string concatenation
  kicks in and the missing comma is never reported. However, this is most likely not
  what the author imagined. The resulting array will have only three - instead of four -
  elements and can generate bugs that are hard to be traced back.

* Functions are globally visible by default. Thus, every program that links against a
  certain library can see and call all functions. To make a function accessible only
  within the file it is declared (within its _translation unit_), a function should be
  declared `static`. If a library needs to export a function symbol to another, it must
  make it gloabally visible - there is no way to achieve a finer grained visibility.

* C has too much overloading of meanings, even at the keyword level. For instance,
the `static` keyword can mean a variable keeps its value in successive function calls,
_within a function context_. However, the same keyword is used to indicate that a function
is only visible within the file it is present. The same thing happens with others such
as `extern` and `void`.

* What is the value of `apple` in the following code?

~~~c
apple = sizeof (int) * p;
~~~

  Writing a simple program to check it, we could have:

~~~c
#include <stdio.h>

int main() {
  int apple, p = 1000;

  /* is it the size of an int multiplied by the value of the p variable?
   * or is it the size of the value of something pointed to by p cast to an int? */
  apple = sizeof (int) * p;
  printf("apple=%d\n", apple);

  return 0;
}
~~~

  And we learn that `apple` is the `sizeof` an int multiplied `p` times. That is
because when `sizeof`'s operand is a type, it must be enclosed in parentheses, which
is not required for variables.

* The previous example teaches us that **being an expert programmer is learning to write
little programs to probe questions like that**. Do more of this kind of experiment!

* Operator precedence can be a bit confusing and generate results that are not expected.
In general, the tip is to avoid mixing up arithmetic operators with logical and bitwise
operators. Use parentheses to keep the order easy to see and avoid hitting such problems.

* When operators share the same precedence, the order of execution is resolved through
the operators _associativity_. An operator can have _right associativity_, meaning that
the rightmost operation in the expression is evaluated first; a similar definition is
given to _left associativity_. For this reason, operators with the same precedence
also share its associativity.

* If you need to determine the associativity of an operator in order to understand the
order in which things are evaluated in an expression, it's a good sign that you should
have added parentheses to make the intent clear.

* Problems are not restricted to the specification of the languge itself, but also present
in its standard functions. As an example, the `gets(3)` function receives a buffer to write to
and, as it cannot perform bound checking, may overwrite the contents of the stack. A malicious
user can enter specially crafted input to get access to the host machine. Even though uses of
`gets(3)` are strongly discouraged in the documentation, it is still part of the C standard and
available on most implementations.

* Argument parsing is inadequate both on UNIX and ANSI C. There is no distinction between
program switches and arguments so that parsing is left to the program developer every time,
instead of relying on a built in tool provided by the system. This approach is error prone
and complicated and has led to famous bugs in many tools. Any change in the `argc`, `argv`
structure would be too big of a change at this point, so this is something programmers need
to learn to live with.

* Misplaced white space characters can lead to wrong programs. Examples include the use of the
backslash to escape newline characters (what if an invisible white space is added after the
backslash?), and also more subtle bugs, such as:

~~~c
r = *a/*b;
~~~

The code that was supposed to divide the content of two `int` pointers actually started a comment
using the `*/`. White space was necessary in this context.

* Always lint your code. What this means in contemporary context is to use the highest level of warnings
produced by the compiler whenever possible. In `gcc` this usually means using the `-Wall` and
`-Wextra` switches.
