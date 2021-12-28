# Stagyra

## Example code

`now "to demonstrate the look and feel of Stagyra" [out] print("Hello, world!");`

## What is Stagyra ?

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

## Unrestricted function call in Stagyra

The functions are called in a similar way like in C-like languages,
but mutable parameters (the "matter") are clearly separated from read-only ones
(the "species", as more descriptive term "cognitive form" coined by Swiezawski is not well known in English).

This is, the read-only parameters are passed in parentheses, and write-out parameters - in square braces.
It looks like a function has two parameter lists.

For instance:

`set[0](1997)`

is an unrestricted function call meaning "put value of 1997 in cell number 0")

A similar call:

`get[0](1997)`

means: "put insides of cell number 1997 into cell number 0" , "copy cell 1997 to cell 0"

The mutable parameters are called "accidental matter",
and the immutables are called "accidental species".

## Restricted function calls in Stagyra

This is good, prior to calling each function, so that we know what can be read and what can be written by this function.
This is why all commands have the "restriction part". So:

`[out] print("Hello world!");`

tells the Stagyra interpreter that this function will write to output,

`[1](0) get[1](0)`

that this call changes cell 1, and reads cell 0, etc.

`[1] set[1](50)`

that this call does not read any cells, but changes cell 1 etc.

These restrictions are called "matter" and "species" accordingly (the immediate function parameters were called "accidental matter" and "accidental species").

If a command tries to write or even read something that is not specified in its restriction, an error will be raised.

## Perfect function calls in Stagyra

For a call to be interpreted by Stagyra, we need two more things: "actualization mode" (the "who" - immediate call, stored to be executed later...) and the "goal" (final cause) - which is an explanation (rather for programmer than the interpreter) why we execute this at all.

For immediate statements, we use `now` as actualization mode.
For stored functions, `func` is used.

After the actualization mode follows the goal - being just a text description (but this is recorded in AST and can be backtraced!).

So, 

`now "to demonstrate the look and feel of Stagyra" [out] print("Hello, world!");`

means: I want to execute this now. If someone wraps his or her head around this code why, I am doing this for demonstration of Stagyra.
This call will change output (`[out]`) and does not read any data from the data memory.
(no parentheses with "species" before the function name). The function will take one accidental parameter that is read - an immediate string.
This will not take any accidental parameters that are written (no square parentheses after function name).

Please note that not taking any ACCIDENTAL write-out parameters by `print` does not mean there is no "matter" for this function - actually
`[out]` is a SUBSTANTIAL parameter for this function that would be hidden if Stagyra's syntax did not force us to state it clear. 

## Named perfect function calls in Stagyra

We would not go any far without naming things - our descriptions would be too long and exhaustive.
And, it's hard to imagine how we could call functions without names.
However, because we usually ask "what is this?" or coin a name after seeing the thing, my decision was to include name as the last part of a statement.

So:


`func "to demonstrate how stored functions are made in Stagyra" [out] print("Hello world!")`

`              : "helloWorld";`


 means:

 I want to define a stored function. The purpose is a demonstration how to made stored function calls for anyone who reads it.
 The matter of this function is output (it writes to output).
 As all the matter and all the read-only parameters (the "species") are specified, this function does not take any accidental matter and species
 (single empty parentheses may be necessary to call this due to Stagyra syntax...).
 This function will be called by the name `helloWorld`.

 To check how it works we can do simply:

 `now "to test the first stored function" [out] helloWorld();`