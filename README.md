# Haskellinator

## Descrizione

Haskellinator trasforma codice (attualmente solo c) in haskell-style.


Prima

    while(x == y) {
        func1();
        func2();
    }

Dopo

    while(x == y)
        { func1()
        ; func2()
        ;
        }

--pesato-a-destra

    while(x == y) { func1()
                  ; func2()
                  ;
                  }

## Compilazione

### CMake

    cmake .
    make

## Come usare

    ./haskellinator <FILE> [--pesato-a-destra]

l'output viene stampato su stdout.
