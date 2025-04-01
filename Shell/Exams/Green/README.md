##  Shell Tasks 

###  `2016-SE-01`
**Task**: Даден е текстов файл с име philip-j-fry.txt. Напишете shell script и/или серия от
команди, които извеждат броя редове, съдържащи поне една четна цифра и несъдържащи малка
латинска буква от a до w.

Примерно съдържание на файла:
123abv123
123zz123
MMU_2.4

Примерен изход:
Броят на търсените редовете е 2

```bash
cat philip-j-fry.txt | grep -e '^[^a-w]*[2468][^a-w]*$' | wc -l

#or

cat /srv/sample-data/01-shell/2016-SE-01/philip-j-fry.txt | grep -E '[02468]' | grep -v '[a-w]' | wc -l

#or

cat /srv/sample-data/01-shell/2016-SE-01/philip-j-fry.txt | grep -E "[02468]" | grep -E "^[^a-w]*$" | wc -l
```

---

###  `2017-IN-03`
**Task**: Напишете серия от команди, които от файла /etc/passwd да вземат под-низ, състоящ се
от втора и трета цифра на факултетния номер на студентите от специалност Информатика, чиито
фамилии завършват на “а”. Изведете коя комбинация от цифри се среща най-често и коя е тя.
Примерно съдържание на файла:
s45194:x:1255:502:Elizabet Mihaylova, Inf, k3, g1:/home/Inf/s45194:/bin/bash
s45139:x:1261:502:Vasilena Peycheva:/home/Inf/s45139:/bin/bash
s81257:x:1079:503:Vasilena Nikolova, KN, 2kurs, 5gr:/home/KN/s81257:/bin/bash
s81374:x:1117:503:Ivan Kamburov, KN, 2kurs, 7gr:/home/KN/s81374:/bin/bash
kiril:x:508:500:Kiril Varadinov:/home/kiril:/bin/bash
s61812:x:1128:504:Vladimir Genchev:/home/SI/s61812:/bin/bash
user:x:1000:99:Inactive user just to start UID from 1000:/home/user:/sbin/nologin
s81254:x:1077:503:Mariela Tihova, KN, 2kurs, 5gr:/home/KN/s81254:/bin/bash
s81386:x:1121:503:Daniela Ruseva, KN, 2kurs, 7gr:/home/KN/s81386:/bin/bash
s45216:x:1235:502:Aleksandar Yavashev, Inf, k3, g3:/home/Inf/s45216:/bin/bash
Примерен изход:
2 51

```bash
grep --color ',I:' /etc/passwd | cut -d ':' -f 1,5 | sed 's/,,,,I//g' | rev | grep -E '^а.*$' | rev | cut -c 3,4 | sort -n | uniq -c | sort -n -r
#or if we use the file 
grep --color '/home/Inf' example-passwd | cut -d ':' -f1,5 | cut -d ',' -f 1 | rev | grep '^a.*$' | rev | cut -c 3,4 | sort -n | uniq -c
```

---

###  `2019-SE-01`
**Task**:  
Даден е текстовият файл planets.txt, който съдържа информация за гравитационно
закръглените обекти в дадена слънчева система. На всеки ред има информация за точно един обект
в следните колони, разделени с ’;’:
• име на обекта
• тип на обекта (един знак)
– Т - земен тип
– G - газов гигант
– I - леден гигант
• средно разстояние на обекта до локалната звезда
• маса на обекта (относителна величина)
• обем на обекта (относителна величина)
• плътност (𝑔/𝑐𝑚3
)
• средна орбитална скорост (𝑘𝑚/𝑠)
Първият ред във файла e header, който описва имената на колоните.
Данните за обектите не са сортирани.
Намерете и изведете разделени с таб името и масата на обекта, който е едновременно:
• най-близкият до локалната звезда
• от същия тип като типа на най-далечният до локалната звезда обект


#### Example entries:
```
name;type;distance;mass;volume;density;speed
earth;T;1.00000011;1;1;5.52;29.7859
mars;T;1.52366231;0.107;0.151;3.94;24.1309
saturn;G;9.53707032;95;763.62;0.7;9.6724
mercury;T;0.38709893;0.055;0.056;5.43;47.8725
venus;T;0.72333199;0.815;0.857;5.24;35.0214
jupiter;G;5.20336301;318;1321.3;1.33;13.0697
neptune;I;30.06896348;17;57.747;1.76;5.4778
uranus;I;19.19126393;14.5;63.102;1.3;6.8352
```

#### Solution:
```bash
sort -n -k 3 -t ';' input.data | grep $(sort -n -r -k 3 -t ';' input.data | head -n 1 | cut -d ';' -f 2) | head -n 1 | cut -d ';' -f 1,5 | tr ';' ' '

```

---

###  `2019-SE-02`
**Task**:  
Вие сте асистент по ОС. На първото упражнение казвате на студентите да си напишат
данните на лист, взимате го и им правите акаунти. След упражнението обаче, забравяте да вземете
листа със себе си - сещате се половин час по-късно, когато трябва да въведете имената на студентите
в таблица, но за зла беда в стаята вече няма ни помен от листа (вероятно иззет от спешния отряд на
GDPR-полицията)
Сещате се, че в началото на упражнението UNIX-часовникът е показвал 1551168000, а в края 1551176100.
Напишете команда, която изкарва разделени с таб факултетните номера и имената на потребителите
от специалност СИ, чиито home директории са променили статуса си (status change time) в зададения
времеви интервал.
Приемете, че всички потребители от СИ имат home директории под /home/SI.
Примерен изход:
62198 Ivaylo Georgiev
62126 Victoria Georgieva
62009 Denitsa Dobreva
62208 Trayana Nedelcheva
Няколко реда от /etc/passwd за справка:
s62136:x:1302:503:Alexander Ignatov, SI, 2, 2:/home/KN/s62136:/bin/bash
s62171:x:1031:504:Deivid Metanov:/home/SI/s62171:/bin/bash
s62126:x:1016:504:Victoria Georgieva:/home/SI/s62126:/bin/bash
s62009:x:1170:504:Denitsa Dobreva,SI,3,3:/home/SI/s62009:/bin/bash
s62196:x:1221:504:Elena Tuparova,SI,2,1:/home/SI/s62196:/bin/bash

```bash
grep <(find $(grep "/home/SI" example-passwd | cut -d ':' -f 6) -type f -printf "%p %C@\n") | awk '{if ($2<1551176100 && $2>1551168000) print $1}' example-passwd | cut -d ':' -f 1,5 | tr ':' ' ' | cut -d ',' -f1
```

---

### `2020-SE-02`
**Task**:  
Даден е текстовият файл spacex.txt, който съдържа информация за полети на ракети на
SpaceX. На всеки ред има информация за един такъв полет в следните колони, разделени с вертикална
черта ’|’:
• дата на полета в UNIX формат
• космодрум
• успешност на полета (две възможни стойности)
– Success - успешен полет
– Failure - неуспешен полет
• полезен товар
Първият ред във файла e header, който описва имената на колоните. Данните във файла не са сортирани.
Намерете и изведете разделени с двоеточие (’:’) успешността и информацията за полезния товар на
най-скорощния полет, който е изстрелян от космодрума с най-много неуспешни полети.
Примерен входен файл:
date|launch site|outcome|payload
1291759200|CCAFS|Success|Dragon demo flight and cheese
1435438800|CCAFS|Failure|SpaceX CRS-7
1275666300|CCAFS|Success|Dragon Spacecraft Qualification Unit
4
1452981600|VAFB|Success|Jason-3
1498165200|KSC|Success|BulgariaSat-1
1473454800|CCAFS|Failure|Amos-6
1517868000|KSC|Success|Elon Musk's Tesla
1405285200|CCAFS|Success|Orbcomm

#### Solution:
```bash
cp /srv/sample-data/01-shell/2020-SE-02/spacex.txt spacex.txt
grep $(cat spacex.txt | cut -d '|' -f 2,3 | grep 'Failure' | sort -r -k 1 -t '|' | uniq -c | head -n 1 | cut -d '|' -f 1 | rev | cut -d ' ' -f 1 | rev) spacex.txt | sort -n -r -k 1 -t '|' | cut -d '|' -f 3,4 | head -n 1 | tr '|' ':'
```

---



### `2016-SE-03`
**Task**: Напишете shell скрипт който, ако се изпълнява от root, проверява кои потребители на
системата нямат homedir или не могат да пишат в него.
Примерен формат:
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin

#### Solution:
```bash
#!/bin/bash

#if [[ $(id -u) != "root" ]] ;
#then
#    echo "Only root user can run this script"
#    exit 1
#fi

cat /etc/passwd | grep -v "/home" | cut -d ":" -f 1

#for user in $(cat /etc/passwd | cut -d ':' -f 1) ;
#do
#    if echo "${no_home}" | grep "${user}"; then
#        continue
#    fi

#    home_dir=$(grep "^.*${user}.*$" | cut -d ':' -f 6)
#    perms=$(stat -c "%A" ${home_dir})

#    if [[ "${perms:2:1}" != 'w' ]] ;
#    then
#        echo "${user}: ${home_dir}"
#        continue
#    fi
#done

for file in $(cat /etc/passwd | cut -d ':' -f 6) ; do
    if [[ -d ${file} ]] ;
    then
        perm=$(stat -c "%A" ${file} | cut -c 3)
        if [[ ${perm} != 'w' ]] ;
        then
            user=$(stat -c "%u" ${file})
            echo "${user}"
        fi
    fi
done
```

---

### `2016-SE-05`
**Task**:  
Файловете във вашата home директория съдържат информация за музикални албуми и
имат специфична структура. Началото на всеки ред е годината на издаване на албума, а непосредствено, след началото на всеки ред следва името на изпълителя на песента. Имената на файловете се
състоят от една дума, която съвпада с името на изпълнителя.
Примерно съдържание на файл с име ”Bonnie”:
2005г. Bonnie - "God Was in the Water" (Randall Bramblett, Davis Causey) – 5:17
2005г. Bonnie - "Love on One Condition" (Jon Cleary) – 3:43
2005г. Bonnie - "So Close" (Tony Arata, George Marinelli, Pete Wasner) – 3:22
2005г. Bonnie - "Trinkets" (Emory Joseph) – 5:02
2005г. Bonnie - "Crooked Crown" (David Batteau, Maia Sharp) – 3:49
2005г. Bonnie - "Unnecessarily Mercenary" (Jon Cleary) – 3:51
2005г. Bonnie - "I Will Not Be Broken" - "Deep Water" (John Capek, Marc Jordan) – 3:58
Да се напише shell скрипт приемащ два параметъра, които са имена на файлове от вашата home директория. Скриптът сравнява, кой от двата файла има повече на брой редове, съдържащи неговото
име (на файла). За файлът победител изпълнете следните действия:
• извлечете съдържанието му, без годината на издаване на албума и без името на изпълнителя
• сортирайте лексикографски извлеченото съдържание и го запишете във файл с име ’изпълнител.songs’
Примерен изходен файл (с име Bonnie.songs):
"Crooked Crown" (David Batteau, Maia Sharp) – 3:49
"God Was in the Water" (Randall Bramblett, Davis Causey) – 5:17
"I Will Not Be Broken" - "Deep Water" (John Capek, Marc Jordan) – 3:58
"Love on One Condition" (Jon Cleary) – 3:43
"So Close" (Tony Arata, George Marinelli, Pete Wasner) – 3:22
"Trinkets" (Emory Joseph) – 5:02
"Unnecessarily Mercenary" (Jon Cleary) – 3:51

#### Solution:
```bash
#!/bin/bash

if [[ ${#} -ne 2 ]] ;
then
    echo "Invalid number of parameters"
    exit 1
fi

if [[ ! -f "${1}" ]] || [[ ! -f "${2}" ]] ;
then
    echo "Invalid parameter. Should be a file."
    exit 2
fi

get_text() {
    cut -d ' ' -f 4- "${1}" | sort -r
}

get_count() {
    grep -E "^.*${1}.*$" "${1}" | wc -l
}

fisrt_count=$(get_count "${1}")
second_count=$(get_count "${2}")

if [[ "${first_count}" -eq "${second_count}" ]] ;
then
    echo "It is a tie."
    exit 3
fi

if [[ "${first_count}" -lt "${second_count}" ]] ;
then
    echo "$(get_text ${2})" > "${2}.songs"
    exit 0
fi

if [[ "${first_count}" -gt "${second_count}" ]] ;
then
    echo "$(get_text ${1})" > "${1}.songs"
    exit 0
fi
```

---


### `2017-IN-03`
**Task**:  
Напишете скрипт, който извежда името на потребителския акаунт, 
в чиято home директория има най-скоро променен обикновен файл и кой е този файл. 
Напишете скрипта с подходящите
проверки, така че да бъде валиден инструмент.

#### Solution:
```bash
#!/bin/bash

if [[ ${#} -ne 0 ]] ;
then
    echo "No parameters allowed"
    exit 1
fi

home_dirs=$(cat /etc/passwd | cut -d ':' -f 6)

for home in ${home_dirs} ;
do
    find ${home} -type f -printf "%C@ %f ${home}\n" 2>/dev/null | sort -nr -k 1 -t ' '  | head -n 1
done | sort -r -n -k 1 -t ' '| head -n 1 | cut -d ' ' -f 2,3
```

---


### `2017-SE-05`
**Task**:  
Напишете скрипт, който получава два задължителни позиционни параметъра – директория и низ. Сред файловете в директорията би могло да има такива, чиито имена имат структура
vmlinuz-x.y.z-arch където:
• vmlinuz е константен низ;
• тиретата “-” и точките “.” присъстват задължително;
• x е число, version;
• y е число, major revision;
• z е число, minor revision;
• наредената тройка x.y.z формира глобалната версия на ядрото;
• arch е низ, архитектура (платформа) за която е съответното ядро.
Скриптът трябва да извежда само името на файла, намиращ се в подадената директория (но не и
нейните поддиректории), който:
• спазва гореописаната структура;
• е от съответната архитектура спрямо параметъра-низ, подаден на скрипта;
• има най-голяма глобална версия.
Пример:
• Съдържание на ./kern/:
vmlinuz-3.4.113-amd64
vmlinuz-4.11.12-amd64
vmlinuz-4.12.4-amd64
vmlinuz-4.19.1-i386
```
• Извикване и изход:
$ ./task1.sh ./kern/ amd64
vmlinuz-4.12.4-amd64
```
#### Solution:
```bash
#!/bin/bash

if [[ "${#}" -ne 2 ]] ;
then
    echo "Parameters should be 2"
    exit 1
fi

dir="${1}"

if [[ ! -d  "${dir}" ]] ;
then
    echo "First parameter should be a directory"
    exit 2
fi

string="${2}"

if [[ -z "${string}" ]] ;
then
    echo "Second parameter should be a string"
    exit 3
fi

find "${dir}" -mindepth 1 -maxdepth 1 -type f -printf "%f\n" | grep -E "^vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-${string}$" | sort -V | tail -n 1

```

---

### `2022-CE-01`
**Task**:  
Описание на формат на CSV (текстови) файл:
• CSV файлът представлява таблица, като всеки ред на таблицата е записан на отделен ред;
• на даден ред всяко поле (колона) е разделено от останалите със запетая;
• в полетата не може да присъства запетая, т.е. запетаята винаги е разделител между полета;
• броят на полетата на всеки ред е константа;
• първият ред във файла e header, който описва имената на колоните.
Разполагате с два CSV файла със следното примерно съдържание:
• base.csv:
unit name,unit symbol,measure
20
second,s,time
metre,m,length
kilogram,kg,mass
ampere,A,electric current
kelvin,K,thermodynamic temperature
mole,mol,amount of substance
candela,cd,luminous intensity
• prefix.csv:
prefix name,prefix symbol,decimal
tera,T,1000000000000
giga,G,1000000000
mega,M,1000000
mili,m,0.001
nano,n,0.000000001
Където смисълът на колоните е:
• за base.csv
– unit name – име на мерна единица
– unit symbol – съкратено означение на мерната единица
– measure – величина
• за prefix.csv
– prefix name – име на представка
– prefix symbol – означение на представка
– decimal – стойност
Забележка: Във файловете може да има и други редове, освен показаните в примера. Приемаме, че
файловете спазват описания по-горе формат, т.е. не е необходимо да проверявате формата.
Напишете shell скрипт, който приема три задължителни параметъра: число, prefix symbol и unit symbol.
Скриптът, ако може, трябва да извежда числото в основната мерна единица без представка, добавяйки
в скоби величината и името на мерната единица.
Примерен вход и изход:

```
$ ./foo.sh 2.1 M s
2100000.0 s (time, second)
Забележка: За изчисления може да ползвате bc.
```

#### Solution:
```bash
#!/bin/bash

base="/home/students/s0600147/2022-CE-01/base.csv"
prefix="/home/students/s0600147/2022-CE-01/prefix.csv"

if [[ "${#}" -ne 3 ]];
then
    echo "There should be 3 parameters"
    exit 1
fi

if [[ ! "${1}" =~ ^[0-9]+(\.[0-9]+)?$ ]] ;
then
    echo "First parameter should be a number"
    exit 2
fi

if ! cut -d ',' -f 2 "${prefix}" | grep -q "${2}"  ;
then
    echo "Invalid prefix symbol"
    exit 3
fi

if ! cut -d ',' -f 2 "${base}" | grep -q "${3}"  ;
then
    echo "Invalid unit symbol"
    exit 4
fi

mult=$( egrep "^[^,]*,${2},[^,]*$" "${prefix}" | cut -d ',' -f 3 )

if [[ "${mult}" =~ ^[0-9]+(\.[0-9]+)?$ ]] ;
then
    num=$( echo "${1}*${mult}" | bc -l )
    echo "${num}"
else
    echo "Invalid multiplier"
    exit 5
fi

comment=$(egrep "^[^,]*,${3},[^,]*$" "${base}" | cut -f 1,3 -d ',' )

echo "${num} ${3} (${comment})"
```

---

### `022-CE-02`
**Task**:  
Съвременните компютри могат да влизат в различни режими за енергоспестяване (suspend)
и излизат от този режим (wake-up) при настъпването на определени събития. Linux kernel предоставя
специален виртуален файл /proc/acpi/wakeup, чрез който може да се проверява или променя настройката 
за “събуждане” при при настъпване на различни събития. Тъй като този файл е интерфейс
към ядрото, “четенето” от файла и “писането” във файла изглеждат различно.
За улеснение на задачата ще ползваме опростено описание на този файл.
Под whitespace разбираме произволна комбинация от табове и интервали.
При четене от файла изходът представлява четири колони, разделени с whitespace, в полетата не може
да има whitespace; първият ред е header на съответната колона.
Примерно съдържание на файла:
Device S-state Status Sysfs node
GLAN S4 *enabled pci:0000:00:1f.6
XHC S3 *enabled pci:0000:00:14.0
XDCI S4 *disabled
LID S4 *enabled platform:PNP0C0D:00
HDAS S4 *disabled pci:0000:00:1f.3
RP01 S4 *enabled pci:0000:00:1c.0
21
където:
• първата колона дава уникално име на устройство, което може да събуди машината, като името
е ограничено до четири знака главни латински букви и цифри;
• третата колона описва дали това устройство може да събуди машината. Възможните стойности
са enabled/disabled, предхождани от звездичка;
• втората и четвъртата колона ги игнорираме в рамките на задачата.
При записване на име на устройство във файла /proc/acpi/wakeup, неговото състояние се променя
от disabled на enabled или обратно.
Например, ако /proc/acpi/wakeup изглежда както примера по-горе, при изпълнение на командата
echo XHC > /proc/acpi/wakeup, третият ред ще се промени на:
XHC S3 *disabled pci:0000:00:14.0
При изпълнение на командата echo HDAS > /proc/acpi/wakeup, шестият ред ще се промени на:
HDAS S4 *enabled pci:0000:00:1f.3
Напишете shell скрипт, който при подаден първи параметър име на устройство (напр. LID) настройва
съответното устройство да не може да събуди машината.

#### Solution:
```bash
#!/bin/bash

if [[ ${#} -ne 1 ]] ;
then
    echo "One parameter allowed"
    exit 1
fi

if ! echo "${1}" | egrep -q "^[0-9A-Z]{,4}$" ;
then
    echo "The name should be only from 0-9 and A-Z"
    exit 2
fi

if grep -q "^${1}" "/proc/acpi/wakeup" ;
then
    echo "Invalid device"
    exit 3
fi

isDisabled=$(egrep -q "^${1}" "/proc/acpi/wakeup" | awk '{print $3}')

if [[ "${isDisabled}" == "*disabled" ]] ;
then
    exit 0
else
    echo "${1}" > "/proc/acpi/wakeup"
    exit 0
fi
```

---

### `2023-CE-01`
**Task**:  
Трябва да напишете shell script, който работи с данни от наблюдения на звезди.
Скриптът приема два параметъра:
• Име на CSV файл, съдържащ данните
• Име на тип звезда (по-долу ще обясним за какво го ползваме)
Част от CSV файла изглежда така:
name          ,id          ,coordinats             ,.... ,type   ,period ,magnitude
Hen 2-99      ,000-BDB-257 ,13 52 30.68 -66 23 26.6, Cir ,PN     ,--     ,13.1
Nova Car 1971 ,--          ,10 39 47.12 -63 14 07.2, Car ,NON-CV ,--     ,--
WR 21a        ,000-BJP-528 ,10 25 56.50 -57 48 43.5, Car ,WR     ,31.673 ,12.82
HD 109857     ,000-BKQ-240 ,12 39 14.58 -75 22 14.1, Mus ,HMXB   ,--     ,6.48
Колонките са разделени със запетайки и имат следните значения:
1. Име на звездата
2. Идентификатор
3. Координати
4. Име на съзвездие
5. Тип на звездата
6. Период
7. Яркост (магнитуд)
• По-ярките звезди имат по-малка стойност за магнитуд
Скриптът трябва да извежда името на най-ярката звезда от съзвездието, в което има най-много звезди
от типа, подаден като втори параметър.
Липсващите стойности са обозначени с две тирета.

#### Solution:
```bash
#!/bin/bash

if [[ ${#} -ne 2 ]] ;
then
    echo "Two parameters"
    exit 1
fi

if [[ ! -f ${1} ]] ;
then
    echo "Invalid file name"
    exit 2
fi

stars_by_type=$(egrep "^([^,]*,){4}${2},.*$" "${1}" )

if ! egrep -q "^([^,]*,){4}${2},.*$" "${1}"
then
    echo "Invalid star type"
    exit 3
fi

most_stars=$(echo "${stars_by_type}" | cut -d ',' -f 4 | sort | uniq -c | sort -n -r | head -n 1 | awk '{print $2}')

echo "${stars_by_type}" | egrep "^([^,]*,){3}${most_stars},.*" | sed  "s:--:0:g" | cut -d ',' -f 1,7 | sort -t ',' -k2,2n | head -n 1

```