# SysOpy
Repository for operation systems

## Regulamin zajęć laboratoryjnych

Zaliczenie laboratorium uzyskuje się po zrealizowaniu następujących dwóch warunków:

- uczęszczanie na laboratoria i realizacja w wymaganym terminie zadanych na nich zadań (bierze się pod uwagę obecność, przygotowanie do zajęć)
- zaliczenie kolokwiów

Uzyskana ocena jest średnią ważoną dwóch ocen cząstkowych (w skali od 0 do 5 punktów) uzyskanych za realizację wyżej wymienionych warunków.

### Zestawy zadań (waga: 70%)

Na każde zajęcia należy zrealizować zadania, których tematy są podane w modułach związanych z poszczególnymi zajęciami. Wszystkie zadania, które mają być zrealizowane na dany dzień, powinny zostać umieszczone w platformie Moodle do końca dnia, poprzedzającego dzień, w którym odbywają się zajęcia.

Ocenie będą podlegać następujące kryteria:

- bezbłędna kompilacja (brak ostrzeżeń przy opcji -Wall),
- zgodność ze specyfikacją,
- poprawność działania dla poprawnych danych,
- odporność na błędy,
- stosowanie ogólnie przyjętych konwencji zachowania się programów,
- czytelność kodu źródłowego.

Prowadzący będzie na zajęciach sprawdzał rozwiązanie zadań zaprezentowanych przez studenta oraz stopień opanowania obowiązującego materiału. W przypadku przysłania zadań z opóźnieniem ich sprawdzenie będzie mogło nastąpić po otrzymaniu zadań, na kolejnych odbywających się zajęciach.

Jeśli zadanie nie będzie odpowiednio przygotowane do oddania, uzyskana liczba punktów zostanie za pierwszym razem obniżona o 1 punkt. Jeśli sytuacja oddania niepoprawnie przygotowanego zadania powtórzy się, student otrzyma za nie 0 punktów.

Za pierwszy, rozpoczęty tydzień opóźnienia oddania zestawu zadań, od końcowej łącznej oceny z zestawów, odjęte zostanie 0,1 punktu. Jednocześnie, opóźnienie oddania zestawu nie może przekroczyć jednego tygodnia, gdyż wówczas skutkuje to oceną niedostateczną z laboratorium.

**Kartkówki**

Podczas zajęć może odbyć się krótka kartkówka z materiału związanego z zestawem, który jest oddawany w danym dniu.  Ze wszystkich kartkówek wyliczana jest średnia ocena, która jest traktowana jak kolejna ocena z zestawu i jest jednym ze składników do wyliczenia oceny za zestawy zadań.

**Pozytywne zaliczenie wszystkich zestawów zadań jest jednym z warunków niezbędnych do zaliczenia laboratorium.**

**Oceny za pracę na laboratorium i za aktywność.** W ramach oceny składowej za realizację zestawów zadań prowadzący mogą oceniać przygotowanie do zajęć lub w wykazywaną aktywność. Może to mieć na przykład postać oceniania zadań cząstkowych (związanych z aktualnym tematem ćwiczenia). Taka pojedyncza ocena może poprawiać lub obniżać średnią ocenę za zestawy  do około 0.1 podczas każdego laboratorium.

**Zaliczenie zestawu.**  Poszczególne zadania i podzadania w ramach zestawu mają przypisane procenty, procenty przypisane zadaniom (i ewentualnie podzadaniom) sumują się do 100%. Żeby zaliczyć zestaw konieczna jest realizacja przynajmniej 50% zadań.  Ilość procentową rozwiązanych zadań w zestawie należy podać w komentarzach podczas ładowaniu pliku z rozwiązaniem zestawu.  Procent zrealizowanego zestawu jest weryfikowany przez prowadzącego i zestaw jest zaliczony wtedy, jeśli zadeklarowane przynajmniej 50% zestawu utrzyma się po tej weryfikacji.

Waga zrobionych zestawów i jej wpływ na ocenę. Wyliczona średnia z odpowiedzi na temat zestawów jest przemnażana przez wartość liczbową opisującą procent zrealizowanych zestawów (np. 0,95 w przypadku realizacji 95% wszystkich 10 zestawów, 95% z 10 zestawów można uzyskać przy realizacji dziewięciu zestawów w 100% i jednego w 50%).

### Kolokwia (waga: 30%)

W ciągu całego semestru student ma obowiązek napisać dwa kolokwia. Terminy kolokwiów zostaną podane przez prowadzącego ze stosownym wyprzedzeniem. Uzyskanie pozytywnej średniej oceny ze wszystkich kolokwiów jest niezbędne do zaliczenia przedmiotu.
Przygotowanie do oddania

**Oddanie zadania będącego w całości lub części plagiatem skutkuje brakiem zaliczenia z przedmiotu.**

## Przygotowanie do oddania

### Zestawy zadań
Przekazywany plik, zawierający zrealizowany zestaw zadań, powinien zawierać wszystkie pliku źródłowe: (pliki C, skrypty, makefile, pliki tekstowe z danymi, pliki tekstowe z wynikami testów itd.). Nie wysyłamy żadnych plików binarnych (wykonywalnych, bibliotek, ...) czy innych zbędnych plików.
Nazwa pliku: <Nazwisko-i-Imie>-cw<dwucyfrowy-numer-cwiczenia>.tar.gz
np. BrzeczyszczykiewiczGrzegorz-cw01.tar.gz

#### Struktura archiwum

Plik powinien zawierać starowane i zgzipowane pliki z zadaniami z jednego ćwiczenia. Pliki z każdego zadania powinny być w oddzielnym katalogu o nazwach typu zad1, zad2, zad3a, zad3b (w zależności od numerów zadań w danym ćwiczeniu). Archiwum powinno dodatkowo obejmować dwa katalogi wyższego rzędu tzn. dwucyfrowy numer ćwiczenia (np. cw01 lub cw02 itd.) i na najwyższym stopniu: katalog o nazwie <NazwiskoImie>.
np. Jan Żądło archiwizując zadania z pierwszego ćwiczenia powinien stworzyć archiwum, w którym najwyższy katalog nazywa się ZadloJan, katalog ten mieści jeden podkatalog cw01, w którym znajdują się katalogi zad1, zad2, zad3a, zad3b, zad4 ...w których są odpowiednie pliki z każdego zadania.

Aby ułatwić sobie zadanie, należy na dysku utworzyć taką strukturę katalogową, a następnie stworzyć odpowiedni plik:
tar cvzf ZadloJan-cw01.tar.gz ZadloJan

Proszę nie używać polskich liter ani w nazwie pliku, ani w nazwach plików w archiwum, ani wewnątrz plików.

#### Realizacja zadania 

W każdym wykonywanym zadaniu tworzony program ma obowiązek sprawdzać poprawność przekazywanych do niego parametrów z linii poleceń.
W zadaniach, które wymagają tworzenia kodu w języku C należy dodatkowo dołączyć odpowiedni makefile, który pozwoli na budowanie zadania (`make` lub `make all`) oraz usuwanie zbędnych plików (`make clean`). Warto też dodać przykładowe wywołania programu (`make test`).
Przy kompilacji kodu źródłowego należy włączyć w kompilatorze opcję -Wall.

#### Wysłanie zadania

Wysyłając zadanie należy koniecznie w komentarzu napisać, które zadania z zestawu zostały zrealizowane (działają) i wysłane. Jeśli zestaw jest kompletny, również należy to napisać. Brak opisu albo niezgodność ze stanem faktycznym skutkuje niezaliczeniem zestawu.
Zestaw niekompletny uzupełniony w terminie późniejszym jest traktowany jako zestaw przysłany z opóźnieniem.
