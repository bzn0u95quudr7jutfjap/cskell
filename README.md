# Haskellinator　「ハスケリナター」

## Descrizione　「記述」

Haskellinator trasforma codice (attualmente solo c) in haskell-style.

注意：日本語できません、本当にごめんなさい。  
ハスケリナターはハスケルスタイルに入力コード（Cだけ）を変える。


Prima　「以前」

    while(x == y) {
        func1();
        func2();
    }

Dopo　「後」

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

## Compilazione　「処理」

### CMake

```sh
    mkdir build/ && cmake .. && make
```

## Come usare　「使い方」

```sh
    ./haskellinator <FILE「ファイル」> [--pesato-a-destra]
```

l'output viene stampato su stdout.

STDOUTに出力を書く。
