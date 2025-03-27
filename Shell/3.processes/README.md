## Task 04-a-5000
### Намерете командите на 10-те най-стари процеси в системата.

#### Commands:
```bash
ps -e --sort=etime -o cmd | tail -n 10
```

## Task 04-a-6000
### Намерете PID и командата на процеса, който заема най-много виртуална памет в системата.

#### Commands:
```bash
ps -e --sort=vsz -o pid=,cmd= | tail -n 1
```

## Task 04-a-6300
### Изведете командата на най-стария процес

#### Commands:
```bash
ps -e --sort=etime -o comm= | tail -n 1
```

## Task 04-a-6300
### Изведете командата на най-стария процес

#### Commands:
```bash
ps -e --sort=etime -o comm= | tail -n 1
```

## Task 04-b-5000.txt
### Намерете колко физическа памет заемат всички процеси на потребителската група root.

#### Commands:
```bash
ps -eo group,rss | grep '^root' | awk '{sum+=$2} END {print sum}'

#or

ps -g 'root' -o group,rss | grep '^root' | awk '{sum+=$2} END {print sum}'
``` 

## Task 04-b-6100.txt
### Изведете имената на потребителите, които имат поне 2 процеса, чиято команда е vim (независимо какви са аргументите й)

#### Commands:
```bash
ps -A -o user,cmd | grep "vim" | awk '{print $1}' | sort | uniq -c | awk '{if ($1>=2)print $2}'
``` 

## Task 04-b-6200.txt
### Изведете имената на потребителите, които не са логнати в момента, но имат живи процеси

#### Commands:
```bash
grep --color -v -f <( who | cut -d ' ' -f 1 ) <( ps -eo user | sort | uniq )
``` 

## Task 04-b-7000.txt
### Намерете колко физическа памет заемат осреднено всички процеси на потребителската група root. Внимавайте, когато групата няма нито един процес.

#### Commands:
```bash
ps -A -o drs,group | grep 'root' | cut -d ' ' -f1 | sed '/^[[:space:]]*$/d'  | awk '{sum += $1; counter += 1;} END {if (counter > 0) print (sum/counter)}'
``` 