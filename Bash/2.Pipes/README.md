
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
