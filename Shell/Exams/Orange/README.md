## Shell Tasks Archive

###  `2016-SE-02`
2016-SE-02 Напишете shell скрипт, който приема един позиционен параметър - число. Ако скриптът
се изпълнява като root, да извежда обобщена информация за общото количество активна памет (RSS
- resident set size, non-swapped physical memory that a task has used) на процесите на всеки потребител.
Ако за някой потребител обобщеното число надвишава подадения параметър, да изпраща подходящи
сигнали за прекратяване на процеса с най-много активна памет на потребителя.
Забележка: Приемаме, че изхода в колоната RSS е число в същата мерна единица, като числото, подадено като аргумент. Примерен формат:
5
USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
root 1 0.0 0.0 15816 1884 ? Ss May12 0:03 init [2]
root 2 0.0 0.0 0 0 ? S May12 0:00 [kthreadd]
root 3 0.0 0.0 0 0 ? S May12 0:02 [ksoftirqd/0]
Забележка: Алтернативно може да ползвате изхода от ps -e -o uid,pid,rss

```bash
#!/bin/bash

if [[ $(id -u) -ne 0 ]] ; then
    echo "This script can only be run by root"
    exit 1
fi

if [[ "${#}" -ne 1 ]] ; then
    echo "Exprected 1 argument - number"
    exit 2
fi

num="${1}"
if [[ ! "${num}" =~ ^[-]?[0-9]+$ ]] ; then
    echo "Parameter should be a number"
    exit 3
fi

result=$(ps -e -o uid=,pid=,rss= | sort -k 1n)

for user in $(echo "${result}" | awk '{print $1}' | sort | uniq) ; do
    sum=$(ps -e -o uid=,pid=,rss= | grep -E "^${user}" | awk '{sum+=$3} END {print sum}' )
    if [[ "${sum}" -gt "${1}" ]] ; then
        toKill=$(echo "${result}" | grep -E "^${user}" | sort -k 3 -t ' ' -r | head -n 1 | awk '{print $2}')
        kill -9 "${toKill}"
    fi
done
```

---

###  `2017-IN-02`
2017-IN-02 Напишете скрипт, който приема задължителен позиционен аргумент - име на потребител
FOO. Ако скриптът се изпълнява от root:
а) да извежда имената на потребителите, които имат повече на брой процеси от FOO, ако има такива;
б) да извежда средното време (в секунди), за което са работили процесите на всички потребители
на системата (TIME, във формат HH:MM:SS);
в) ако съществуват процеси на FOO, които са работили над два пъти повече от средното време,
скриптът да прекратява изпълнението им по подходящ начин.
За справка:

```bash
$ ps -e -o user,pid,%cpu,%mem,vsz,rss,tty,stat,time,command | head -5
USER PID %CPU %MEM VSZ RSS TT STAT TIME COMMAND
root 1 0.0 0.0 15820 1920 ? Ss 00:00:05 init [2]
root 2 0.0 0.0 0 0 ? S 00:00:00 [kthreadd]
root 3 0.0 0.0 0 0 ? S 00:00:01 [ksoftirqd/0]
root 5 0.0 0.0 0 0 ? S< 00:00:00 [kworker/0:0H]
```

```bash
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then 
    echo "One argument expected"
    exit 1
fi

if [[ $(id -u) -ne 0 ]] ; then 
    echo "Only root can execute"
    exit 2
fi

user="${1}"
grep -q "${user}" <(cat /etc/passwd | cut -d ':' -f 1)
if [[ $? -ne 0 ]] ; then
    echo "Invalid user"
    exit 2
fi

processes=$(ps -e -o user=,time=,pid=)
user_processes=$(echo "${processes}" | grep -E "${user}" | wc -l )

#a)

while read line ; do
    user_to_compare=$(echo "${line}" | awk '{print $2}')
    count=$(echo "${line}" | awk  '{print $1}')
    if [[ "${user_to_compare}" == "${user}" || "${count}" -lt "${user_processes}" ]] ; then 
        continue
    fi

    echo "${line}" | cut -d ' ' -f 2

done < <(echo "${processes}" | awk '{print $1}' | sort | uniq -c )

#b)

time=$(echo "${line}" | awk '{print $2}')
count=0
while read line ; do
    count=$((count + 1))
    hours=$(echo "${line}" | cut -d ':' -f 1)
    minutes=$(echo "${line}" | cut -d ':' -f 2)
    seconds=$(echo "${line}" | cut -d ':' -f 3)
    secs=$((hours*3600 + minutes*60 + seconds))
    result=$((result + secs))
done < <(echo "${time}")

echo "${result} / ${count} " | bc
$((result*=2))
while read line ; do
    seconds=$(echо "${line}" | awk '{print $2}' | sed 's/:/*3600/' | sed 's/:/*60/' | bc )

    if [[ "${seconds}" -gt $((result*=2)) ]] ; then
        kill -9 $(echo "${line}" | awk '{print $3}')
    fi
done < <(echo "${user_processes}" | awk '{print $1,$2}')
```
