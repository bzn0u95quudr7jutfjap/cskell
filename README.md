# cskell　「チスケール」

注意：日本語できません、本当にごめんなさい。  

## Descrizione　「記述」

cskell trasforma codice (attualmente solo c) in haskell-style.
cskellはハスケルスタイルに入力コード（Cだけ）を変える。


Prima　「以前」

```c
while(x == y) {
    func1();
    func2();
}
```

Dopo　「後」

```c
while(x == y)
    { func1()
    ; func2()
    ;
    }
```

## Compilazione　「処理」

### CMake

```sh
sh compile.sh
```

## Come usare　「使い方」

```sh
cskell <inputfile｜入力ファイル>  <outputfile｜出力ファイル>
```

l'output viene stampato su stdout.

STDOUTに出力を書く。
