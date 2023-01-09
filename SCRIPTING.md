# Scripting engine

The scripting engine is largely based on _Lox_, the language developed in the [Crafting Interpreters](https://craftinginterpreters.com/) book.

## Engine interop

There is none at the moment.

The end goal is that you'd write all your game logic in an easier scripting language instead of torturing yourself with
the mess that is C++, but right now this language is extremely bare-bones and cannot interact with the rest of the world
other than some `print "Hello, World!";` action.

## Types

- **Strings.** _Extremely basic_, no Unicode support, no fancy formatting.
- **Numbers.** 64-bit floating points.
- **Functions.** First class types!
- **Booleans.**
- **nil.** Also known as _null_, now renamed for some reason. Guess it's less spooky.

## Basic syntax

Lines must be terminated with a semicolon. Indentation doesn't matter. This ain't Python.

Comments start with `//` and span until the end of the line. There are no special multi-line block comments.

## Examples!

**Hello World**: see above

**Variables**:

```c++
var name = "some value";
print name;
// 'print' is not treated like a function call but rather as its own statement,
// similar to ancient versions of Python.
// ... didn't I just say this ain't Python? This makes no sense...
```

There are no constant values _yet_, every `var`iable is mutable.

**Functions**:

```c++
fun return_something() {
    return 42;
}

// String concatenation works exactly as you'd expect.
print "The answer is " + return_something();
```

**If statements**:

```c++
var result = true;
if (result) {
    print "Result is true";
} else {
    print "Result is false";
}
```

**For and while loops**:

```c++
var i = 0;
while (i < 10) {
    print i;
    i = i + 1;
}

// equivalent to
for (var i = 0; i < 10; i = i + 1) {
    print i;
}
```

**Classes (basic)**:

```c++
class Greeter {
    init(person) {
        print "Class initializer, called automatically";
        
        this.person = person;
    }
    
    // Class member functions do not use the 'fun' keyword (this may change)
    greet() {
        print "Hello, " + this.person + "!";
    }
}

var greeter_1 = Greeter("Cedric");
var greeter_2 = Greeter("Rue");
var greeter_3 = Greeter("Prototype");

greeter_1.greet();
greeter_2.greet();
greeter_3.greet();
```

**Classes (inheritance)**:

Important change from original Lox language: Inheriting classes uses the `:` token, not `<`.

```c++
class Base {
    inBase() {
        return "Hello from the base class!";
    }
    
    override() {
        return "Hello from the base class!";
    }
}

class Child : Base {
    // 'inBase()' is inherited (copied) from Base parent class
    
    // new method exclusive to 'Child'
    inChild() {
        return "Hello from the child class!";
    }
    
    // overwrites previously declared 'override()' method
    override() {
        return "Hello from the child class!";
    }
}

var child = Child();
print child.inBase(); // 'Hello from the base class!'
print child.inChild(); // 'Hello from the child class!'
print child.override(); // 'Hello from the child class!'
```

**Native functions**:

```c++
// Seconds since program start
var time = clock();

// Total bytes allocated by VM
var bytes_allocated = memory();

// Force immediate garbage collection
gc();
```

**Passing functions**:
```c++
fun do_work(callback) {
    for (var i = 0; i < 10; i = i + 1) {
        callback(i);
    }
}

fun callback(number) {
    print number + 1;
}

// Prints numbers from 1..=10
do_work(callback);
```

