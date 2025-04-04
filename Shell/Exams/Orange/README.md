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