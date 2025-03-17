
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
