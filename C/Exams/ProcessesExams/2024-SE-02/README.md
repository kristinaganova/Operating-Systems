# Задача 2024-SE-02: 

## Описание
Напишете програма наблюдател, която няколкократно изпълнява други програми до тяхното успешно изпълнение.
Програмата получава между 1 и 10 аргумента – имена на наблюдавани програми:
```bash
./main <наблюдавана програма 1> <наблюдавана програма 2> ... <наблюдавана програма N>
```
Програмата наблюдател трябва да изпълни всяка от подадените ѝ наблюдавани програми без аргументи. Програмите трябва да се изпълняват в паралелно работещи процеси под следните условия:
- Когато някой процес завърши успешно (с нулев статус), съответстващата му програма се счита
за приключила и вече не се използва от вашата програма наблюдател.
- Когато някой процес завърши неуспешно (с ненулев статус), съответстващата му програма трябва да се изпълни наново, като за новия процес важат същите правила.
- Ако някой процес бъде убит преди да е завършил (напр. при краш), програмата наблюдател
трябва да:
1. Прекрати изпълнението на всички други текущо изпълняващи се процеси. Прекратяването
на процес става, като му изпратим сигнал SIGTERM и след това го изчакаме да приключи.
2. Завърши със статус, съответстващ на номера на програмата, чийто процес е бил убит. Например, ако процес, изпълняващ наблюдавана програма 2, бъде убит, вашата програма наблюдател трябва да завърши със статус 2.
С други думи, всяка наблюдавана програма трябва да бъде изпълнявана многократно до първи успех,
а ако което и да е изпълнение на която и да е програма бъде убито, програмата наблюдател спира
работа.
Решения, в които наблюдаваните програми не се изпълняват паралелно, ще получат най-много 6
точки. В тази ситуация, очакваме последователно изпълнение на наблюдаваните програми (първо
програма 1 до първи успех, после програма 2 и т.н.), а условието за прекратяване на всички процеси
при убит процес отпада.
---
