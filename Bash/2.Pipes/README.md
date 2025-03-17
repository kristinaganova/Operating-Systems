
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

## Task 03-a-0210
### Изведете само 1-ва и 5-та колона на файла /etc/passwd спрямо разделител ":".

#### Commands:
```bash
cat /etc/passwd | cut -f 1,5 -d ':'
```
