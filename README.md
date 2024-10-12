# cskell　「チスケール」

注意：日本語できません、本当にごめんなさい。  

[EN](./README-en.md)

## Descrizione　「記述」

cskell trasforma codice (attualmente solo c) in haskell-style.<br>
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

L'output viene stampato su stdout se outputfile non viene specificato.

出力ファイルが選びません場合は、STDOUTに出力を書く。
