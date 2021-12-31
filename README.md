# Stagira

## Example code

`now "to demonstrate the look and feel of Stagira" [out] print("Hello, world!");`

## What is Stagira ?

This is a simple programming language inspired by Aristotle's philosophy.

According to Aristotle, each being has four factors called "causes":
* the final cause ("what's the purpose of this?"),
* the efficient cause ("who made it?"),
* the material cause or MATTER ("what is it made of?" OR "what undergoes change?")
* the formal cause or FORM ("how is it made?").

For instance, the formal cause of a vase is it's shape and being burnt,
the material cause - the clay,
the efficient cause - the potter,
and the final cause - carrying water.

It's worth noting that **matter should be understood rather as the "base of change"**
and the "physical" way of understanding is misleading in many cases.

For example the matter of an algorithm is it's mutable data (variables).

What's most striking here is that, unlike later philosophy, both Aristotle and Thomas Aquinas
insist on the sensible aspect of each being - it's final aim and purpose.
In other words - nothing in world is pointless, everything has a purpose and direction.

This is this is necessary to specify the goal of each call.

## Unrestricted function call in Stagira

The functions are called in a similar way like in C-like languages,
but mutable parameters (the "matter") are clearly separated from read-only ones
(the "species", as more descriptive term "cognitive form" coined by Swiezawski is not well known in English).

This is, the read-only parameters are passed in parentheses, and write-out parameters - in square brackets.
It looks like a function has two parameter lists.

For instance:

`set[0](1997)`

is an unrestricted function call meaning "put value of 1997 in cell number 0")

A similar call:

`get[0](1997)`

means: "put insides of cell number 1997 into cell number 0" , "copy cell 1997 to cell 0"

The mutable parameters are called "accidental matter",
and the immutables are called "accidental species" (see: [Sidenotes on terminology](#sidenotes-on-terminology) ).

## Restricted function calls in Stagira

This is good, prior to calling each function, so that we know what can be read and what can be written by this function.
This is why all commands have the "restriction part". So:

`[out] print("Hello world!");`

tells the Stagira interpreter that this function will write to output,

`[1](0) get[1](0)`

that this call changes cell 1, and reads cell 0, etc.

`[1] set[1](50)`

that this call does not read any cells, but changes cell 1 etc.

These restrictions are called "matter" and "species" accordingly (the immediate function parameters were called "accidental matter" and "accidental species").

If a command tries to write or even read something that is not specified in its restriction, an error will be raised.

## Perfect function calls in Stagira

For a call to be interpreted by Stagira, we need two more things: "actualization mode" (the "who" - immediate call, stored to be executed later...) and the "goal" (final cause) - which is an explanation (rather for programmer than the interpreter) why we execute this at all.

For immediate statements, we use `now` as actualization mode.
For stored functions, `func` is used.

After the actualization mode follows the goal - being just a text description (but this is recorded in AST and can be backtraced!).

So, 

`now "to demonstrate the look and feel of Stagira" [out] print("Hello, world!");`

means: I want to execute this now. If someone wraps his or her head around this code why, I am doing this for demonstration of Stagira.
This call will change output (`[out]`) and does not read any data from the data memory.
(no parentheses with "species" before the function name). The function will take one accidental parameter that is read - an immediate string.
This will not take any accidental parameters that are written (no square parentheses after function name).

Please note that not taking any ACCIDENTAL write-out parameters by `print` does not mean there is no "matter" for this function - actually
`[out]` is a SUBSTANTIAL parameter for this function that would be hidden if Stagira's syntax did not force us to state it clear. 

## Named perfect function calls in Stagira

We would not go any far without naming things - our descriptions would be too long and exhaustive.
And, it's hard to imagine how we could call functions without names.
However, because we usually ask "what is this?" or coin a name after seeing the thing, my decision was to include name as the last part of a statement.

So:

`func "to demonstrate how stored functions are made in Stagira" [out] print("Hello world!") : "helloWorld";`


 means:

 I want to define a stored function. The purpose is a demonstration how to made stored function calls for anyone who reads it.
 The matter of this function is output (it writes to output).
 As all the matter and all the read-only parameters (the "species") are specified, this function does not take any accidental matter and species
 (single empty parentheses may be necessary to call this due to Stagira syntax...).
 This function will be called by the name `helloWorld`.

 To check how it works we can do simply:

 `now "to test the first stored function" [out] helloWorld();`

## Passing accidental parameters to functions

If all matter (write-out data) and species (read-in data) specified in the matter & species clause are well-defined, there is no need to pass them as accidental parameters when calling a function. Especially, ordinal numbers when they appear there are simply cell numbers, and special keywords: `out` , `in` , `bind` are also understood as bound.

The missing parameters (literals) have to be passed in the order they appear in matter clause, and in the species clause.
The binding is made for matter clause (square brackets) first , and then for species clause (parentheses) next.
In case one parameter exists in both clauses, one bind is sufficient, and when the occurence in second clause this will not be re-bound.

Example:
```
func "Print first n Fibonacci numbers"
    [0,1,2,3,out]
    (0,1,2,3,n)
        seq(
            set[0](0),
            set[1](1),
            set[3](n),
            loop(
                get[bind](3),
                seq(
                    get[out](0),
                    op[2](get[bind](0), "+", get[bind](1)),
                    get[0](1),
                    get[1](2),
                    op[3](get[bind](3), "-", 1)
                )
            )
        )
    : "fib";

now "for user explanation" [out] print("This should print first 15 Fibonacci numbers");
now "do what we promised"
    [0,1,2,3,out](0,1,2,3) fib(15);
```

We define a function to print some Fibonacci numbers, not yet defined how many in the time of definition.
This function's matter (what it can change) is: cells 0, 1, 2, 3 in the memory,
and species (what it can read) is: cells 0, 1, 2, 3, 4 in the memory, output (`out`) and mysterious `n`.

All the matter parameters are well defined all the runtime, similar most species parameters, except for `n`.
Because Stagira does not recognize `n` when the function is loaded, it will look for it in accidental species list
(the parentheses in function call). This is why we pass `15` to the function.

As this might make unpredictable results in long parameter list, this is not recommended to use the same name for binding in matter clause and a binding in species clause when defining a function, even if we use the same variable for read and write.
In such a case, the parameter would be bound when reading the accidental matter clause, and if the same occurred in the species clause, this would be misbound to the next unbound parameter causing errors.

## Ranges

As we can see, the matter (write-out) & species (read-in) declarations swell very fast. Even than the `fib` function from the previous example is not that complicated, it reads and writes quite a number of memory cells:

`now "show how long is the declaration of reads () and writes []..." [0,1,2,3,out](0,1,2,3) fib(15);`

This is actually an intended effect to *discourage using global variables* (even though we do not have any other possibility just for now...) and make their usage clear and explicit. However if we use some cells as a regular array, and it has more than four or five cells, declaring it this way is neither practical nor even possible in some cases.

This is what ranges are for. Actually, in the matter & species declaration context, for a continuous range of cells, this is possible to use double-dot-connected terminal values of that range. Eg `[0,1,2,3]` in matter clause can be shortened to `[0..3]` expression.

Please note range is not just a syntactic sugar, it has a different internal representation. However this:

`now "this looks much better with enumerations" [0..3, out](0..3) fib(15);`

works just the same as the longer version above.

## Binding ranges

Ranges, just as single cells, might be declared but not bound in matter and species parameters. There are two cases to be distinguished - either left or right binding is undefined (`0..a` , `a..0`) or both of them (`a..b`).

In the first case only the missing part is looked for in the accidental parameters. So the function caller only specifies the missing number defining the end:

```
now "preset values to detect potential errors"
[0..4]
seq(
    set[0](100),
    set[1](200),
    set[2](300),
    set[3](400),
    set[4](500)
);

func "print a range from zero"
[out,10]
(0..a,10)
seq(
    set[10](0),
    loop(
        op[bind](get[bind](10), "<=", set[bind](a)),
        seq(
            get[out](get[bind](10)),
            op[10](get[bind](10), "+", 1)
        )
    )
) : "printRangeLeft";

now "print first four numbers" [out](0..3) printRangeLeft(3);
now "horizontal line" [out] print("-------");
now "print first five numbers" [out](0..4) printRangeLeft(4);
```

Please note, that `printRangeLeft` function has one unbound parameter which is a range. However only one range final element is undefined, and this is actually the only unbound value to be passed in accidental parameters. This is why to make the `0..a` range a `0..3` range we only pass `3` as accidental parameter. If `printRangeLeft(0..3)` would be used instead of `printRangeLeft(3)`, this would be an error.

This is an example for a range that is bound on it's left and unbound on its right side.
For ranges that are only right-bound the mode of operation is the same - we only supply the missing end in accidental parameters.

However the situation changes when the range is undefined as whole - it is when both ends are undefined.
In such a case we do not pass the final elements separately - but a range as whole. In such a case trying to bind the ends by passing two separate accidental parameters would be an error:

```
func "print a range from zero"
[out,10]
(a..b,10)
seq(
    set[10](a),
    loop(
        op[bind](get[bind](10), "<=", set[bind](b)),
        seq(
            get[out](get[bind](10)),
            op[10](get[bind](10), "+", 1)
        )
    )
) : "printRange";

now "print numbers 2,3,4" [out](2..4) printRange(2..4);
now "horizontal line" [out] print("-------");
now "print all numbers" [out](0..4) printRange(0..4);
now "horizontal line" [out] print("-------");
now "print second-to-last two numbers" [out](3..4) printRange(3..4);
```

Even though there are two unbound parameters `a` and `b` at the most basic level, they form a range, and this is whole the range that is unbound. Therefore we specify it as one parameter in the accidental params: `printRange(3..4)`. If `printRange(3,4)` would be used instead, this would be incorrect.

*The motivation for this is rather making the code more logical, as the very act of passing the parameters shows that what we pass is a range, which could be hardly deduced if two comma-separated values were passed.*


## The special `out` binding

Sometimes we want to print a value immediately instead of storing it. This is why `out` special literal is used. It is only valid in matter clause (square brackets) due to the very nature of "output".

So, for instance,

`now "demonstrate how to put immediate number into cell 0" [0] set[0](1997);`

quietly sets cell 0 to 1997, while

`now "demonstrate how to output a number" [out] set[out](1997);`

will not set anything in the memory, putting the number on the screen.

## The special `in` binding

Similar to `out` , there is also an `in` binding enabling the user to input something from the keyboard.
For now only integer values are supported. Of course this parameter only has sense in the "species" part as this is read-only by nature.

This code just echoes the number input by the user:

`now "check simplest echo possible" [out](in) set[out](in);`

## The special `bind` binding

The basic model of Stagira is not stack-based, as the return values of functions are specified and passed in the matter clause (square brackets).
However, this undermines any nested function calls (where we intend to use the result of one call as the input of the other).

In order to make the function call result accessible as a parameter of other function, we need to use `bind` for the selected "matter" component.

So, for instance,
```
now "setup one cell" [0] set[0](20);
now "setup other cell" [1] set[1](17);
now "print the sum of contents of both cells" [out](0,1) op[out](get[bind](0), "+", get[bind](1));
```

will setup the cells 0, 1 to values of 20 and 17 immediately, and then perform and arithmetic sum of its contents, printing this immediately.
Here `bind` just means "the output destination is this parameter of the calling function, where it appeared".

Even though this is overhead in writing, this enables us to select which matter parameter is the one we want to return, if a function has more of them.

## Sidenotes on terminology

_Species - a generality received by the intellect from individuals,
e.g. "my dog", "my sister's dog" - individuals, "dog" - species.
Species is a formal (intellectual, spiritual) phenomenon free of matter - otherwise cognition could not accept it._

However, because in our terminology the function body is the *form*, I decided to use the term
*species* as it's kind of a form - forms don't change. We have to look from the function's perspective and not ours because the destination of the species in Stagira is not our knowledge (as in normal meaning of "species"), but the function's "knowledge".
