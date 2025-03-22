## To do :
```
03-a-5004

```

## Task 03-a-0200
### Сортирайте /etc/passwd лексикографски по поле UserID.

#### Commands:
```bash
cat /etc/passwd | sort -k 3 -t ':'
```

## Task 03-a-0201
### Сортирайте /etc/passwd числово по поле UserID.
(Открийте разликите с лексикографската сортировка)

#### Commands:
```bash
cat /etc/passwd | sort -n -k 3 -t ':'
```

## Task 03-a-0201
### Сортирайте /etc/passwd числово по поле UserID.
(Открийте разликите с лексикографската сортировка)

#### Commands:
```bash
cat /etc/passwd | cut -f 1,5 -d ':'
```

## Task 03-a-0211
### Изведете само 1-ва и 5-та колона на файла /etc/passwd спрямо разделител ":".

#### Commands:
```bash
cat /etc/passwd | cut -f 1,5 -d ':'
```

## Task 03-a-0211
### Изведете съдържанието на файла /etc/passwd от 2-ри до 6-ти символ.

#### Commands:
```bash
cat /etc/passwd | cut -c 2-6
```

## Task 03-a-0212
### Отпечатайте потребителските имена и техните home директории от /etc/passwd.

#### Commands:
```bash
cat /etc/passwd | cut -f 1,6 -d ':' | tr ':' ' '
```

## Task 03-a-0213
### Отпечатайте втората колона на /etc/passwd, разделена спрямо символ '/'.

#### Commands:
```bash
cat /etc/passwd | cut -f 2 -d '\'
```

## Task 03-a-1500
### Изведете броя на байтовете в /etc/passwd.
Изведете броя на символите в /etc/passwd.
Изведете броя на редовете  в /etc/passwd.

#### Commands:
```bash
wc -c /etc/passwd
32605 /etc/passwd
wc -m /etc/passwd
27871 /etc/passwd
wc -l /etc/passwd
389 /etc/passwd
```

## Task 03-a-3000
### С отделни команди, извадете от файл /etc/passwd:
- първите 12 реда
- първите 26 символа
- всички редове, освен последните 4
- последните 17 реда
- 151-я ред (или друг произволен, ако нямате достатъчно редове)
- последните 4 символа от 13-ти ред (символът за нов ред не е част от реда)

#### Commands:
```bash
head -n 12 /etc/passwd
head -c 26 /etc/passwd
head -n -4 /etc/passwd
tail -n 17  /etc/passwd
head -n 151 /etc/passwd | tail -n 1
head -n 13 /etc/passwd | tail -n 1 | tail -c 5
```

## Task 03-a-3100
### Запазете само потребителските имена от /etc/passwd във файл users във вашата home директория.

#### Commands:
```bash
 cat /etc/passwd | cut -f 5 -d ':' | cut -f 1 -d ',' >  users
```

## Task 03-a-3500
### Изпишете всички usernames от /etc/passwd с главни букви.

#### Commands:
```bash
cat /etc/passwd | cut -f 1 -d ':' | tr [a-z] [A-Z]
```
## Task 03-a-5000
### 
Изведете реда от /etc/passwd, на който има информация за вашия потребител.

Изведедете този ред и двата реда преди него.

Изведете този ред, двата преди него, и трите след него.

Изведете *само* реда, който се намира 2 реда преди реда, съдържащ информация за вашия потребител.

#### Commands:
```bash
cat /etc/passwd | grep $(id -u)
cat /etc/passwd | grep $(id -u) -B 2
cat /etc/passwd | grep $(id -u) -B 2 -A 3
cat /etc/passwd | grep $(id -u) -B 2 | head -n 1
```

## Task 03-a-5100
### Изведете колко потребители не изпозват /bin/bash за login shell според /etc/passwd

(hint: 'man 5 passwd' за информация какъв е форматът на /etc/passwd)

#### Commands:
```bash
cat /etc/passwd | cut -f 7 -d ':' | grep -v "/bin/bash" | wc -l
```

## Task 03-a-5002
### Изведете само имената на хората с второ име по-дълго от 6 (>6) символа според /etc/passwd

#### Commands:
```bash
cat /etc/passwd | cut -f 5 -d ':' | cut -f 2 -d ' '| cut -f 1 -d ',' |grep -E ".{7,}"

```

## Task 03-a-5003
### Изведете имената на хората с второ име по-късо от 8 (<=7) символа според /etc/passwd // !(>7) = ?

#### Commands:
```bash
cat /etc/passwd | cut -f 5 -d ':' | cut -f 2 -d ' '| cut -f 1 -d ',' |grep -v -E ".{8,}$"

//to remove the white spaces from the ouputs

cat /etc/passwd | cut -f 5 -d ':' | cut -f 2 -d ' '| cut -f 1 -d ',' |grep -v -E ".{8,}$" | grep -E "[^a-zA-Z]"
=
```

## Task 03-a-5004
### Изведете целите редове от /etc/passwd за хората от 03-a-5003

#### Commands:
```bash
cat /etc/passwd | cut -f 5 -d ':' | cut -f 2 -d ' '| cut -f 1 -d ',' |grep -v -E ".{8,}$" | grep -E "[^a-zA-Z]" | xargs -I {} grep -F {} /etc/passwd
```

## Task 03-b-0300
### Намерете само Group ID-то си от файлa /etc/passwd.

#### Commands:
```bash
grep $(id -u) /etc/passwd | cut -f 4 -d ':'
```

## Task 03-b-3400
### Колко коментара има във файла /etc/services ? Коментарите се маркират със символа #, след който всеки символ на реда се счита за коментар.

#### Commands:
```bash
cat /etc/services | grep '^#'| wc -l
37
```

## Task 03-b-3500
### Колко файлове в /bin са 'shell script'-oве? (Колко файлове в дадена директория са ASCII text?)

#### Commands:
```bash
find -L /bin -type f -exec file {} \; | grep 'shell script' | wc -l
77

find /usr/bin -type f -exec file {} \; | grep 'ASCII text' | wc -l
76
```

## Task 03-b-3600
### Направете списък с директориите на вашата файлова система, до които нямате достъп. Понеже файловата система може да е много голяма, търсете до 3 нива на дълбочина.

#### Commands:
```bash
find / -maxdepth 3 -type d >/dev/null 2>errors.txt
cat errors.txt | cut -f 2 -d ':' | tr -d ' ' | cut -c 4- | rev | cut -c 4- 
```

## Task 03-b-4001
### Във file2 (inplace) подменете всички малки букви с главни.

#### Commands:
```bash
cat dir5/file2 | tr 'a-z' 'A-Z' > dir5/tmp && cat dir5/tmp > dir5/file2 && rm dir5/tmp

#or 

sed -i 's/[a-z]/\U&/g' dir5/file2
```

## Task 03-b-4002
### Във file3 (inplace) изтрийте всички "1"-ци.

#### Commands:
```bash
sed -i -e '/1/d' -e '/^[[:space:]]*$/d' dir5/file3
```

## Task 03-b-4003
### Изведете статистика за най-често срещаните символи в трите файла.

#### Commands:
```bash
cat dir5/file{1,2,3} | sort | uniq -c | sort -nr
```

## Task 03-b-4004
### Прочетете текстов файл file1 и направете всички главни букви малки като запишете резултата във file2.

#### Commands:
```bash
cat dir5/file1 | tr 'a-z' 'A-Z' > dir5/file2
```
#note: tr не записва резултата във файла => си записваме резултата в друг файл и си го презаписваме ако искаме да заменим директно във файла

## Task 03-b-5200
### Намерете броя на символите, различни от буквата 'а' във файла /etc/passwd

#### Commands:
```bash
grep -o '.' /etc/passwd | grep -v 'a' | grep -v 'а' | sort | uniq -c | sort -nr
```

## Task 03-b-5300
### Намерете броя на уникалните символи, използвани в имената на потребителите от /etc/passwd.

#### Commands:
```bash
cat /etc/passwd | cut -f 5  -d ':' | cut -f 1 -d ',' | grep -o '.' | sed -e '/[[:space:]]/d' -e '/[[:punct:]]/d' | sort | uniq -c | sort -nr | wc -l
```

## Task 03-b-5400
### Отпечатайте всички редове на файла /etc/passwd, които не съдържат символния низ 'ов'.

#### Commands:
```bash
 grep -v "ов" /etc/passwd
```

## Task 03-b-6100
### Отпечатайте последната цифра на UID на всички редове между 28-ми и 46-ред в /etc/passwd.

#### Commands:
```bash
 cat /etc/passwd | head -n 46 | tail -n 18 | cut -f 3 -d ':' |cut -c 4
```

## Task 03-b-6700
### Отпечатайте правата (permissions) и имената на всички файлове, до които имате read достъп, намиращи се в директорията /tmp. (hint: 'man find', вижте -readable)

#### Commands:
```bash
 find /tmp -readable 2>/dev/null -printf "%M %f\n"
 ```

## Task 03-b-6900
### Намерете имената на 10-те файла във вашата home директория, чието съдържание е редактирано най-скоро. На първо място трябва да бъде най-скоро редактираният файл. Намерете 10-те най-скоро достъпени файлове. (hint: Unix time)

#### Commands:
```bash
 find . -type f -printf "%T@ %f\n" | sort -nr -k 1 -t ' ' | cut -f 2 -d ' '|head -n 10
 ```

## Task 03-b-7000
### да приемем, че файловете, които съдържат C код, завършват на `.c` или `.h`. Колко на брой са те в директорията `/usr/include`? Колко реда C код има в тези файлове?

#### Commands:
```bash
find /usr/include -name '*.[ch]' |  wc -l
cat $(find /usr/include -name '*.[ch]') |  wc -l
 ```