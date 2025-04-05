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
---


###  `2017-SE-03`
Напишете скрипт, който ако се изпълнява от root потребителя:
а) извежда обобщена информация за броя и общото количество активна памет (RSS - resident set
size, non-swaped physical memory that a task has used) на текущите процеси на всеки потребител;
б) ако процесът с най-голяма активна памет на даден потребител използва два пъти повече памет
от средното за потребителя, то скриптът да прекратява изпълнението му по подходящ начин.
За справка:

```bash
$ ps aux | head -5
USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
root 1 0.0 0.0 15820 1052 ? Ss Apr21 0:06 init [2]
root 2 0.0 0.0 0 0 ? S Apr21 0:00 [kthreadd]
root 3 0.0 0.0 0 0 ? S Apr21 0:02 [ksoftirqd/0]
root 5 0.0 0.0 0 0 ? S< Apr21 0:00 [kworker/0:0H]
Алтернативно, може да ползвате изхода от ps -e -o uid,pid,rss
```

```bash

#!/bin/bash

processes=$(ps -e -o user=,rss=,pid=)
users=$(echo "${processes}" | awk '{print $1}' | sort | uniq)

temp=$(mktemp)
while read user ; do
    count=$(echo "${processes}" | grep "${user}" | wc -l)
    sum=$(echo "${processes}" | grep "${user}" | awk '{sum+=$2} END {print sum}' )
    avg=$(echo "${sum} / ${count}" | bc )
    echo "${user} ${avg}" >> ${temp}
    echo "${user} count: ${count} rss count: ${sum}"
done < <(echo "${users}")

while read user ; do
    avg=$(echo "$temp" | grep "${user}" | cut -d ' ' -f 2)
    user_processes=$(echo "${processes}" | grep "$user")
    while read process ; do
        rss=$(echo "${process}" | awk '{print $2}')
        if [[ "${rss}" -gt $((2 * "${avg}")) ]] ; then 
            kill -9 $(echo "${process}" | awk '{print $3}')      
        else 
            continue
        fi
    done <<< "${user_processes}"
    
done <<< "${users}"

rm "${temp}"
```

---


###  `2017-SE-06`
 Напишете скрипт, който ако се изпълнява от root потребителя, намира процесите на
потребителите, които не са root потребителя и е изпълнено поне едно от следните неща:
• имат зададена несъществуваща home директория;
• не са собственици на home директорията си;
• собственика на директорията не може да пише в нея.
Ако общото количество активна памет (RSS - resident set size, non-swaped physical memory that a task has
used) на процесите на даден такъв потребител е по-голямо от общото количество активна памет на
root потребителя, то скриптът да прекратява изпълнението на всички процеси на потребителя.
За справка:
```
$ ps aux | head -n 5
USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
root 1 0.0 0.0 15820 1052 ? Ss Apr21 0:06 init [2]
root 2 0.0 0.0 0 0 ? S Apr21 0:00 [kthreadd]
root 3 0.0 0.0 0 0 ? S Apr21 0:02 [ksoftirqd/0]
root 5 0.0 0.0 0 0 ? S< Apr21 0:00 [kworker/0:0H]
Алтернативно, може да ползвате изхода от ps -e -o uid,pid,rss
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
s61934:x:1177:504:Mariq Cholakova:/home/SI/s61934:/bin/bash
```

```bash
#!/bin/bash

processes=$(ps -e -o user=,pid=,rss=)
users=$(echo "${processes}" | awk '{print $1}' | grep -v "root" | sort | uniq )

temp=$(mktemp)
while read user ; do
    home_dir=$(cat /etc/passwd | grep "${user}" | cut -d ':' -f 6)
    sum=$(echo "${processes}" | grep "${user}" | awk '{sum+=$3} END {print sum}')
    if [[ ! -e "${home_dir}" ]] ; then
        echo "${user} ${sum}" >> "${temp}"
        continue
    fi

    if [[ $(stat -c '%U' "${home_dir}") != "${user}" ]]
        echo "${user} ${sum}" >> "${temp}"
        continue
    fi

    if [[ $(stat -c '%A' "${home_dir}" | cut -c 3) != 'w' ]] ; then
        echo "${user} ${sum}" >> "${temp}"
        continue
    fi
done <<< "${users}"

root_sum=$(echo "${processes}" | grep "root" | awk '{sum+=$3} END {print sum}')

while read line ; do
    if [[ $(echo "${line}" | cut -d ' ' -f 2) -gt "${root_sum}" ]] ; then
        kill -9 $(echo "${processes}" | grep "${user}" | awk '{print $2}')
    fi
done <<< "${temp}"

```

---


###  `2018-SE-01`
Нека съществува програма за моментна комуникация (Instant messaging), която записва
логове на разговорите в следния формат:
• има определена директория за логове (LOGDIR)
• в нея има директорийна структура от следния вид:
LOGDIR/протокол/акаунт/приятел/
като на всяко ниво може да има няколко екземпляра от съответния вид, т.е. няколко директории
протокол, във всяка от тях може да има няколко директории акаунт, и във всяка от тях – няколко
директории приятел
• във всяка от директориите приятел може да има файлове с имена от вида yyyy-mm-dd-hh-mmss.txt – година-месец-ден и т.н., спрямо това кога е започнал даден разговор
• всеки такъв файл представлява лог на даден разговор със съответния приятел, като всяка разменена реплика между вас е на отделен ред
• даден идентификатор приятел може да се среща няколко пъти в структурата (напр. през различни ваши акаунти сте водили разговори със същия приятел)
Напишете скрипт, който приема задължителен позиционен аргумент - име на лог директория (LOGDIR).
Скриптът трябва да извежда десетимата приятели, с които имате най-много редове комуникация глобално (без значение протокол и акаунт), и колко реда имате с всеки от тях. Опишете в коментар как
работи алгоритъмът ви.

```bash
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
    echo "Invalid number of arguments"
    exit 1
fi

dir=${1}
if [[ ! -d "${dir}" ]] ; then
    echo "Not a valid directory"
    exit 2
fi

friends=$(find "${dir}" -mindepth 4 -maxdepth 4 -type f | cut -d '/' -f 4 | sort | uniq)
files=$(find "${dir}" -type f)

temp=$(mktemp)
while read friend ; do
    conversations=$(echo "${files}" | grep -E "^([^\/]*\/){3}${friend}\/[0-9]{4}-([0-9]{2}-){4}[0-9]{2}\.txt$" )
    lines=$(cat ${conversations} | wc -l)
    echo "${friend} ${lines}" >> "${temp}"
done < <(echo "${friends}")
cat ${temp} | sort -n -r -k 2 -t ' ' | head -n 10

```


