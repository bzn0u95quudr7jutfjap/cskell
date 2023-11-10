# Haskellinator

## Descrizione

Haskellinator trasforma codice (attualmente solo c) in haskell-style.


Prima

    while(cond) {
        func1();
        func2();
    }

Dopo

    while(cond)
        { func1()
        ; func2()
        ;
        }

--pesato-a-destra

    while(cond) { func1()
                ; func2()
                ;
                }

## Compilazione

### CMake

    mkdir -p build/ && cd build/ && cmake .. && make

## Come usare

    ./haskellinator <FILE>

l'output verra stampato sul terminale in stdout
