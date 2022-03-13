# Instrukcja do testów

## Zadanie 1

- `make all` / `make` - kompilacja + testy
- `make build` - kompilacja
- `make test-all` - wykonanie wszystkich testow
- `make clean` - usuwanie nadmiarowych plikow

## Zadanie 2
Testy 2 i 3 do celu testowania uzywa funkcji `rec()`, ktora celowo wywoluje przeladowanie stosu. **Test 3 powoduje Segmentation Fault i jest to celowe, w celu porownania z testem 2 w ktorym customowy handler daje rade sie wykonac.**
- `make build` / `make` - kompilacja
- `make test1` - test flagi SIGINFO i RESETHAND
- `make test2` - test flagi ONSTACK
- `make test3` - test braku flagi ONSTACK
- `make clean` - usuwanie nadmiarowych plikow

## Zadanie 3a i 3b
Testy:
1. KILL
2. SIGQUEUE
3. SIGRT

Dla kazdego z 3 testow procedura testow jest taka sama. Po kompilacji programu, w terminalu uruchomic catchera poleceniem `make catchX`, gdzie zamiast `X` wstawic numer testu. Nastepnie w osobnej sesji terminala uruchomic sendera poprzez `make sendX`. PID catchera zostanie pobrany automatycznie z pliku pomocniczego tworzonego przez catcher. Sendery ustawiane sa na 1000 sygnalow.
- `make build` / `make` - kompilacja
- `make clean` - usuwanie nadmiarowych plikow
- `make catch1`/`catch2`/`catch3` - uruchamianie catchera
- `make send1`/`send2`/`send3` - uruchamianie sendera
