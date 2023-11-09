# Haskellinator

## Descrizione

Haskellinator prende in input un file di codice (attualmente solo c) e lo trasforma in haskell-style.


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


## Compilazione

### Linux

    mkdir -p build/ && cd build/ && cmake .. && make

## Come usare

    ./haskellinator <FILE>

l'output verra stampato sul terminale in stdout
