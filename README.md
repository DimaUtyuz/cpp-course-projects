# Учебные проекты по курсу "Программирование C++"

## Памятка

Решения реализованы на языках программирования Assembler, C++.

Более подробное условие и описание находится внутри каждого проекта.

### Список условий:

#### [1. Wordcount](#домашнее-задание-1-wordcount)

#### [2. Asm](#домашнее-задание-2-asm)

#### [3. Substr](#домашнее-задание-3-substr)

#### [4. Vector](#домашнее-задание-4-vector)

#### [5. Vector со small-object и copy-on-write оптимизациями](#домашнее-задание-5-Vector-со-small-object-и-copy-on-write-оптимизациями)

#### [6. Bigint](#домашнее-задание-6-bigint)

#### [7. Set](#домашнее-задание-7-set)

### Домашнее задание 1. Wordcount

Необходимо написать на **Assembler** утилиту для подсчёта количества слов.

Программа должна читать ввод со стандартного потока ввода (stdin), пока он не закончится, а затем вывести ответ в стандартный поток вывода (stdout).
Её поведение очень похоже на команду `wc -w` в linux.

### Домашнее задание 2. Asm

Необходимо написать на **Assembler** программы, выполняющие вычитание и умножение беззнаковых длинных чисел.

### Домашнее задание 3. Substr

Необходимо написать программу на языке **C++** в файле `solution.cpp`, которая:
- Принимает два аргумента командной строки: имя входного файла и некоторую непустую строку. В случае, если количество входных параметров не равно двум (`argc != 3`) - необходимо вернуть ошибку и вывести некоторое информативное сообщение в `stderr`.
- Выводит `Yes` если удалось найти заданную строку как подстроку в заданном файле;
- Возвращает некоторый ненулевой (например, -1) код ошибки в случае возникновения ошибки, а также читаемое сообщение об ошибке на `stderr`;
- Выводит `No` в случае, если не удалось найти строку и ошибки не произошло.

### Домашнее задание 4. Vector

Необходимо реализовать класс аналогичный `std::vector`.
Ожидаемое поведение можно смотреть на cppreference:
https://en.cppreference.com/w/cpp/container/vector

Кроме того, метод `push_back` должен уметь вставлять элемент из этого же вектора без ошибок.

### Домашнее задание 5. Vector со small-object и copy-on-write оптимизациями

В этом задании необходимо реализовать класс аналогичный `std::vector`, но имеющий *small-object* и *copy-on-write* оптимизации.

*small-object* предполагает, что вектор умеет хранить небольшое число элементов без динамической аллокации памяти. 

*copy-on-write* предполагает, что копирование/присваивание больших векторов не копирует все элементы само, а откладывает копирование элементов до момента когда к объекту применят модифицирующую операцию.

### Домашнее задание 6. Bigint

Необходимо создать класс **длинного знакового числа** и реализовать:
- Конструктор по умолчанию, инициализирующий число нулём.
- Конструктор копирования.
- Конструкторы от числовых типов.
- Explicit конструктор от `std::string`.
- Оператор присваивания.
- Операторы сравнения.
- Арифметические операции: сложение, вычитание, умножение, деление, унарный минус и плюс.
- Инкреметы и декременты.
- Битовые операции: и, или, исключающее или, не (аналогично битовым операциям для `int`)
- Битовые сдвиги.
- Внешнюю функцию `std::string to_string(big_integer const&)`.

### Домашнее задание 7. Set

Необходимо реализовать класс аналогичный `std::set`.
Ожидаемое поведение можно смотреть на cppreference:
https://en.cppreference.com/w/cpp/container/set
