# cskell

[ITA|日本語](./README.md)

## Description

cskell formats code (only C at the moment) to haskell-style.

Before

```c
while(x == y) {
    func1();
    func2();
}
```

After

```c
while(x == y)
    { func1()
    ; func2()
    ;
    }
```

## Compiling

### CMake

```sh
sh compile.sh
```

## How to use

```sh
cskell <inputfile>  <outputfile>
```

The output is printed to stdout when outputfile isn't specified.
