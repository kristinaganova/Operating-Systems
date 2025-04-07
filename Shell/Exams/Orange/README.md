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

---


###  `2021-SE-02`
Един от често използваните DNS сървъри е BIND9, при който описанието на DNS зоните обикновенно стои в текстови файлове, наричани зонални файлове. За улеснение, в рамките на задачата, ще
ползваме опростено описание на зоналните файлове.
Под whitespace разбираме произволна комбинация от табове и интервали.
Под FQDN разбираме низ, който има допустими символи малки латински букви, цифри и точка; не
може да започва с точка, не може да има две или повече съседни точки, задължително завършва с
точка.
Зоналните файлове съдържат ресурсни записи, по един на ред. Общият вид на даден ресурсен запис e
<ключ> <TTL> <клас> <тип> <RDATA>, разделени с whitespace, например:
astero.openfmi.net. 3600 IN A 185.117.82.99
Където:
• ключ (astero.openfmi.net.) – FQDN
• TTL (3600) – цифри; полето може да липсва
• клас (IN) - главни латински букви; класът винаги е IN
• тип (A) - главни латински букви; някое от SOA, NS, A, AAAA
• RDATA (185.117.82.99) - данни на записа; различни за различните типове ресурсни записи;
всичко след типа до края на реда.

Знакът точка-и-запетая ; е знак за коментар, и всичко след него до края на реда се игнорира.
Във всеки зонален файл трябва да има точно един SOA запис, и той трябва да е първият запис във
файла. Пример за едноредов SOA запис:
openfmi.net. 3600 IN SOA nimbus.fccf.net. root.fccf.net. 2021041901 86400 7200 3024000 3600
RDATA-та на SOA запис се състои от два FQDN-а и пет числа, разделени с whitespace.
Въпреки, че горното е валиден SOA запис, за прегледност в зоналните файлове често се ползва следният синтаксис (многоредов SOA запис, еквивалентен на горния):
openfmi.net. 3600 IN SOA nimbus.fccf.net. root.fccf.net. (
2021041901 ; serial
86400 ; refresh
7200 ; retry
3024000 ; expire
3600 ; negative TTL
)
т.е., поредицата от числа се разбива на няколко реда, оградени в обикновенни скоби, и за всяко число
се слага коментар какво означава.
Първото от тези числа (serial) представлява серийният номер на зоната, който трябва да се увеличава
всеки път, когато нещо в зоналният файл се промени. Изключително важно е това число само да
нараства, и никога да не намалява.
Един от често използваните формати за сериен номер, който показва кога е настъпила последната
промяна в зоналния файл представлява число във вида YYYYMMDDTT, т.е., четири цифри за година,
две цифри за месец, две цифри за дата и още две цифри за поредна промяна в рамките на описания
ден. За последните две цифри (ТТ) има ограничение да са от 00 до 99 (естествено, така не може да има
повече от 100 промени в рамките на един ден).
За удобство приемаме, че конкретен сериен номер (точната поредица цифри) се среща само на едно
място в зоналния файл.
Напишете шел скрипт, който по подадени имена на зонални файлове променя серийният номер в
SOA записа на всеки файл по следният алгоритъм:
• ако датата в серийният номер е по-стара от днешната, новият сериен номер трябва да е от вида
днешнадата00
• ако датата в серийният номер е равна на днешната, серийният номер трябва да се увеличи с
единица
Важат следните условия:
• скриптът трябва да може да обработва и едноредови, и многоредови SOA записи
• за всеки зонален файл, който не е успял да обработи, скриптът трябва да вади съобщение за
грешка, което включва и името на зоналния файл. Съобщенията трябва да са лесно обработваеми с познатите инструменти за обработка на текст.

```bash
#!/bin/bash

if [[ "${#}" -lt 1 ]] ; then
    echo "Provide at least one parameter"
    exit 1
fi

validate_content() {
    grep -E -q "^([a-zA-Z0-9]+\.)+[[:space:]]+([0-9]*){0,1}[[:space:]]+IN[[:space:]]+(SOA|NS|A|AAA).*" "${1}"
}

get_RDATA() {

    sed -E "s/^([a-zA-Z0-9]+\.)+[[:space:]]+([0-9]*){0,1}[[:space:]]+IN[[:space:]]+(SOA|NS|A|AAA)//g" "${1}"
}

change_date() {
    current_date=$(date +%Y%m%d)
    date_in_file="${1}"
    file="${2}"
    serial_number=$(echo "${date_in_file}" | cut -c 9-)
    date_without_serial_number=$(echo "${date_in_file}" | cut -c 1-8)
    if [[ "${date_without_serial_number}" -ne ${current_date} ]] ; then
      sed -i "s/${date_in_file}/${current_date}00/" "${file}"
    else
        ((serial_number += 1))
        if [[ ${serial_number} -lt 10 ]] ; then
            serial_number=$(printf "%02d" "$serial_number")
        fi
        sed -i "s/${date_in_file}/${date_without_serial_number}${serial_number}/" "${file}"
    fi
}

handle_case1() {
    date_in_file=$(echo "${2}" | grep -E -o  "[0-9]{10}" )
    change_date "${date_in_file}" "${1}"
}

handle_case2() {
    line=$(head -n 2 <(echo "${2}") | tail -n 1)
    date_in_file=$(echo "${line}" | awk '{print $1}' )
    change_date "${date_in_file}" "${1}"
}

for file in "${@}" ; do
    if [[ ! -f "${file}" ]] ; then
        echo "Invalid file: ${file}"
        continue
    fi
    validate_content "${file}"

    if [[ "$?" -eq 1 ]] ; then
        echo "Invalid content for file: "${file}". Skipping..."
        continue
    fi

    if ! grep -q 'IN[[:space:]]\+SOA' "$file"; then
        echo "Not a SOA type. Skiping file: ${file}"
        continue
    fi

    RDATA=$(get_RDATA "${file}")
    if [[ $(echo "$RDATA" | wc -l) -eq 1 ]] ; then
        handle_case1 "${file}" "${RDATA}"
    else
        handle_case2 "${file}" "${RDATA}"
    fi
done

```

---

### `2022-IN-03`
Вашите колеги от съседната лаборатория работят по проект, в който до сега се е ползвала нестандартна имплементация на командата java. По време на миграцията с цел използване на
стандартна имплементация на командата се оказало, че на много места командата се вика по стария, грешен начин, който вече е невалиден. Вашата задача е да напишете shell скрипт iwojima.sh,
който работи подобно на старата имплементация на java, но вътрешно вика новата имплементация
по правилния начин.
Файловият формат JAR (Java ARchive) представлява пакет, който обединява няколко Java class файлове, допълнителни мета-данни и различни ресурси. Файлове в този формат обикновенно (но не задължително) имат разширение .jar. Част от метаданните във файла указват кой е главният клас, чийто
метод main() се изпълнява по подразбиране. Въпросният main() метод ще получи списък с аргументите, указани при извикването на командата java (подобно на main() функцията в езика C).
Едно от извикванията на стандартната имплементация на командата java (единственото, което
разглеждаме в тази задача) има следният синтаксис: java [options] -jar filename [args] където:
• options – незадължителна поредица от опции на командата java, разделени с интервал; всяка
опция задължително започва с тире;
• filename – име на JAR файл, който да бъде изпълнен, използва се винаги с опция -jar;
• args – незадължителни аргументи, с които ще бъде стартиран main() методът на класа по подразбиране (аргументи на самата Java програма).
Като пример разполагаме с JAR файл (app.jar), с един клас в него, чийто main() метод реализира
единствено функционалност, еквивалента на следният bash код:
if [ -z "${qux}" ]; then
qux=default
fi
echo "${qux}"
Общият вид на изпълнение с командата java би бил:
$ java -jar ./app.jar
default
Една от стандартните опции на командата java е -Dproperty=value, с което се дефинира променлива
с име property и със стойност value. При нужда от няколко променливи, за всяка се ползва отделна
опция -D. Пример за правилно ползване на опцията в стандартната имплементация:
$ java -Dqux=foo -jar /path/to/app.jar
foo
За съжаление, нестандартната имплементация на java, която е ползвана до момента, работи по
друг начин. Пример:

```
$ java -jar ./app.jar -Dqux=foo
default
$ java -Dqux=foo -jar ./app.jar -hoge fuga
default
$ java -jar -Dqux=foo app.jar
foo
```

Ако трябва да обобщим как работи нестандартната имплементация:
• всичко след името на JAR файла са аргументи за main() метода (напр. -hoge и fuga);
• името на JAR файла е някъде след опцията -jar;
• опции за java командата може да има и преди, и след опцията -jar;
• опцията -Dproperty=value работи само ако е след опцията -jar.
Забележки: Вашият скрипт iwojima.sh няма нужда да поддържа извиквания, които не подават опция -jar. За удобство приемаме, че опциите (вкл. property и value), имената на JAR файловете и
аргументите не могат да съдържат интервали.

```bash
#!/bin/bash

if [[ "${#}" -eq 0 ]] ; then
    echo "Invalid arguments"
    exit 1
fi

if ! grep -q "-jar" ; then
    echo "Invalid arguments"
    exit 2
fi

saw_jar=0
is_jar=0
filename=""
options=""

for arg in "${@}" ; do
    if [[ "${arg}" == "-jar" ]] ; then
        saw_jar=1
        continue
    fi

    if [[ -f "${arg}"]] && [[ "${arg}" =~ ^.*\.jar ]] ; then
        if [[ "${saw_jar}" -eq 1 ]] ; then
            is_jar=1
            filename="${arg}"
        else
            echo ".jar should be after -jar"
            exit 3
        fi
        continue
    fi

    if echo "${arg}" | grep -q -E "^-D[^=]+=.*$" ; then
         options+="${arg} "
         continue
    fi

    if echo "${arg}" | grep -q -E "^-.*" ; then
        options+="${arg}"
    fi

done
```

---

### `2022-SE-01`
Съвременните компютри могат да влизат в различни режими за енергоспестяване (suspend)
и излизат от този режим (wake-up) при настъпването на определени събития. Linux kernel предоставя
специален виртуален файл /proc/acpi/wakeup, чрез който може да се проверява или променя настройката за “събуждане” при настъпване на различни събития, в общия случай - при активност на
някое устройство. Тъй като този файл е интерфейс към ядрото, “четенето” от файла и “писането” във
файла изглеждат различно.
За улеснение на задачата ще ползваме опростено описание на този файл.
Под whitespace разбираме произволна комбинация от табове и интервали. При четене от файла изходът представлява четири колони, разделени с whitespace, в полетата не може да има whitespace;
първият ред е header на съответната колона. Примерно съдържание на файла:
Device S-state Status Sysfs node
GLAN S4 *enabled pci:0000:00:1f.6
25
XHC S3 *enabled pci:0000:00:14.0
XDCI S4 *disabled
LID S4 *enabled platform:PNP0C0D:00
HDAS S4 *disabled pci:0000:00:1f.3
RP01 S4 *enabled pci:0000:00:1c.0
където:
• първата колона дава уникално име на устройство, което може да събуди машината, като името
е ограничено до четири знака главни латински букви и цифри;
• третата колона описва дали това устройство може да събуди машината. Възможните стойности
са enabled/disabled, предхождани от звездичка;
• втората и четвъртата колона ги игнорираме в рамките на задачата.
При записване на име на устройство във файла /proc/acpi/wakeup, неговото състояние се променя
от disabled на enabled или обратно. Например, ако файлът изглежда както примера по-горе, при
запис на XHC в него, третият ред ще се промени на:
XHC S3 *disabled pci:0000:00:14.0
При запис на HDAS, шестият ред ще се промени на:
HDAS S4 *enabled pci:0000:00:1f.3
Дефиниран е формат на конфигурационен файл, който описва желан комплект от настройки на wakeup
събития. Примерен файл:
# comment bar
GLAN disabled
LID enabled # comment foo
където:
• знакът диез (#) е знак за коментар до края на реда;
• редовете би трябвало да са комбинация от име на устройство и желаното състояние на настройката за събуждане при събития от това устройство, разделени с whitespace.
Напишете скрипт, който при подаден първи параметър име на конфигурационен файл в горния формат осигурява исканите настройки за събуждане. Ако във файла има ред за устройство, което не съществува, скриптът да извежда предупреждение. Обърнете внимание на обработката за грешки и съобщенията към потребителя – искаме скриптът да бъде удобен и валиден инструмент.

```bash
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
    echo "Invalud number of arguments: only one allowed"
    exit 1
fi

config="${1}"
wakeup="${HOME}/example-wakeup"
if [[ ! -f "${config}" ]] ; then
    echo "Invalid config file"
    exit 2
fi

while read line ; do

    valid_line=$(echo "${line}" | sed -E "s/([^#]*)#.*/\1/")

    if [[ -z "${valid_line}" ]] ; then
        continue
    fi

    name=$(echo "${valid_line}" | awk '{print $1}')
    status=$(echo "${valid_line}" | awk '{print $2}')
    if [[ ! "${status}" =~ (enabled|disabled) ]] ; then
        echo "Invalid status for: ${name}"
    fi
    if [[ ! "${name}" =~ ^[A-Z]{4}$ ]] || ! grep -q -E "${name}" "${wakeup}" ; then
        echo -e "Inavlid device name: ${name} \n Skipping..."
        continue
    else
        status_in_wakeup=$(grep "${name}" "${wakeup}"| awk '{print $3}' | tr -d "*")
        if [[ ${status} == ${status_in_wakeup} ]] ; then
            echo "Device: ${name} allready in status: ${status}"
        else
            sed -i -E "s/^([[:space:]]*${name}[[:space:]]+S[0-9][[:space:]]+)\*${status_in_wakeup}([[:space:]]*.*)/\1\*${status}\2/" "${wakeup}"
        fi
    fi

done < "${config}"
```
---

### `2023-IN-02`
Вашите колеги от съседната лаборатория ползват специализиран софтуер за оптометрични изследвания, който записва резултатите от всяко измерване в отделен файл. Файловете имат уникално съдържание, по което се определя за кое измерване се отнася файла. За съжаление, тъй като колегите
ви ползват бета версия на софтуера, той понякога записва по няколко пъти резултатите от дадено измерване в произволна комбинация от следните варианти:
• нула или повече отделни обикновенни файла с еднакво съдържание;
• нула или повече групи от hardlink-ове, като всяка група съдържа две или повече имена на даден
файл с измервания.
Помогнете на колегите си, като напишете shell скрипт, който приема параметър – име на директория,
съдържаща файлове с измервания. Скриптът трябва да извежда на стандартния изход списък с имена
на файлове, кандидати за изтриване, по следните критерии:
• ако измерването е записано само в отделни файлове, трябва да остане един от тях;
• ако измерването е записано само в групи от hardlink-ове, всяка група трябва да се намали с едно
име;
• ако измерването е записано и в групи, и като отделни файлове, за групите се ползва горния
критерий, а всички отделни файлове се премахват.

# Не е напълно коректен

```bash
#!/bin/bash

if [[ ${#} -ne 1 ]] ; then
    echo "Invalid number of parameters: 1 allowed"
    exit 1
fi

dir="${1}"

if [[ ! -d "${dir}" ]] ; then
    echo "Invalid directory: ${dir}"
    exit 2
fi

files=$(find "${dir}" -mindepth 1 -type f)

group_files=$(mktemp)
while read line ; do
    if [[ ! -f "${line}" ]] ; then
        echo "Not a file: ${line}"
    fi

    hash=$(sha256sum "${line}" | awk '{print $1}')

    echo "${line} ${hash} $(stat -c '%i' "${line}")" >> "${group_files}"

done < <(echo "${files}")

uniq_hash=$(cat "${group_files}" | awk '{print $2}' | sort | uniq)

while read line ; do
    same_content=$(grep "${line}" "${group_files}"| awk '{print $1} ' )

    if [[ $(echo "${same_content}" | wc -l ) -eq 1 ]] ; then
        continue
    fi

    inodes=$(grep "$line" "$group_files" | awk '{print $3}' | sort )
    if [[ $(echo "${inodes}" | uniq | wc -l) -eq 1 ]] ; then
#        echo "All hardlinks for content with hash: ${line}"
        echo "${same_content}" | head -n 1
        continue
    fi

    if [[ $(echo "${same_content}" | wc -l) -eq $(echo "${inodes}" | uniq | wc -l) ]] ; then
#        echo "All files with same content with hash: ${line}"
        echo "${same_content}" | tail -n +2
        continue
    fi
#    echo "Files for mixed condition:"
    groups_inodes=$(echo "${inodes}" | sort | uniq -c | sort -n -r | awk '{if ($1>1) print $2}')
    echo "$(grep -v "${groups_inodes}" "${same_content}")"

    while read line2 ; do
        same_content2=$(grep "${line2}" "${group_files}"| awk '{print $1} ' )
#        echo "Hardlinks for content with inode: ${line2} for hash: ${line}"
        echo "${same_content2}" | head -n 1
    done < <(echo "${groups_inodes}")

done < <(echo "${uniq_hash}")


rm "${group_files}"

```