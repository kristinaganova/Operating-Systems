## Shell Tasks Archive

###  `2016-SE-04`
В текущата директория има само обикновени файлове (без директории). Да се напише
bash script, който приема 2 позиционни параметъра – числа, който мести файловете от текущата директория 
към нови директории (a, b и c, които тря да бъдат създадени), като определен файл се
мести към директория ’a’, само ако той има по-малко редове от първи позиционен параметър, мести
към директория ’b’, ако редове са между първи и втори позиционен параметър и в ’c’ в останалите
случаи.

```bash
#!/bin/bash

if [[ "${#}" -ne 2 ]] ;
then 
    echo "Two parameters allowed"
    exit 1
fi

if [[ ! "${1}" =~ ^[0-9]+$ ]] || [[ ! "${2}" =~ ^[0-9]+$ ]] ;
then 
    echo "Parameters should be numbers"
    exit 2
fi

for file in $(find . -mindepth 1 -maxdepth 1 -type f ) 
do
    lines_count=$(cat "${file}" | wc -l)
    if [[ "${lines_count}" -lt "${1}" ]] ;
    then 
        mv "${file}" ./a
    elif [[ "${lines_count}" -gt "${1}" ]] || [[ "${lines_count}" -lt "${2}" ]] ;
    then 
        mv "${file}" ./b
    else
        mv "${file}" ./c
    fi
done
```

---

###  `2017-IN-01`
Напишете скрипт, който приема три задължителни позицонни аргумента:
• име на фаил
• низ1
• низ2
Файлът е текстови, и съдържа редове във формат:
ключ=стойност
където стойност може да бъде:
• празен низ, т.е. редът е ключ=
• низ, състоящ се от един или повече термове, разделени с интервали, т.е., редът е ключ=𝑡1
𝑡2
𝑡3
Някъде във файла:
• се съдържа един ред с ключ първия подаден низ (низ1);
• и може да се съдържа един ред с ключ втория подаден низ (низ2).
Скриптът трябва да променя реда с ключ низ2 така, че обединението на термовете на редовете с ключове низ1 и низ2 да включва всеки терм еднократно.
Примерен входен файл:
```
$ cat z1.txt
FOO=73
BAR=42
BAZ=
ENABLED_OPTIONS=a b c d
ENABLED_OPTIONS_EXTRA=c e f
Примерно извикване:
$ ./a.sh z1.txt ENABLED_OPTIONS ENABLED_OPTIONS_EXTRA
Изходен файл:
$ cat z1.txt
FOO=73
BAR=42
7
BAZ=
ENABLED_OPTIONS=a b c d
ENABLED_OPTIONS_EXTRA=e f
```

```bash
#!/bin/bash

if [[ "${#}" -ne 3 ]] ;
then
    echo "Invalid number of parameters"
    exit 1
fi

if [[ ! -f "${1}" ]] ;
then
    echo "Invalid file"
    exit 2
fi

if [[ -z "${3}" ]] || [[ -z "${2}" ]] ;
then
    echo "Empty string"
    exit 3
fi

terms1=$(egrep "^${2}=.*" "${1}" | cut -d '=' -f 2 | tr ' ' '\n' )
terms2=$(egrep "^${3}=.*" "${1}" | cut -d '=' -f 2 | tr ' ' '\n' )

for line in ${terms2} ;
do
    if ! echo "${terms1}" | grep -qxF "${line}" ;
    then
       result+="${line} "
    fi
done

sed -i -E "s:(${3}=).*:\1${result}:g" "${1}"
```

---

###  `2017-SE-01`
Напишете скрипт, който получава задължителен първи позиционен параметър – директория и незадължителен втори – число. Скриптът трябва да проверява подадената директория и нейните под-директории и да извежда имената на:
а) при подаден на скрипта втори параметър – всички файлове с брой hardlink-ове поне равен на
параметъра;
б) при липса на втори параметър – всички symlink-ове с несъществуващ destination (счупени symlinkове).
Забележка:За удобство приемаме, че ако има подаден втори параметър, то той е число


```bash
#!/bin/bash

if [[ ! -d "${1}" ]] ;
then
    echo "First parameter should be a directory"
    exit 1
fi

if [[ "${#}" -eq 2 ]] ;
then
    if [[ "${2}" =~ ^[0-9]+$ ]] ;
    then
        find "${1}" -mindepth 1 -type f  -exec stat -c "%h %n\n" {} \; | awk -v var=${2} '{if ($1>=var) print $2}'
    else
        echo "Invalid second parameter"
        exit 2
    fi
elif [[ "${#}" -eq 1 ]] ;
then
    find "${1}" -mindepth 1 -type l -exec test ! -e {} \; -print
fi
```

---

###  `2017-SE-02`
Напишете скрипт, който приема три задължителни позиционни параметра - директория
SRC, директория DST (която не трябва да съдържа файлове) и низ АBC. Ако скриптът се изпълнява
от root потребителя, то той трябва да намира всички файлове в директорията SRC и нейните поддиректории, 
които имат в името си като под-низ АBC, и да ги мести в директорията DST, запазвайки
директорийната структура (но без да запазва мета-данни като собственик и права, т.е. не ни интересуват 
тези параметри на новите директории, които скриптът би генерирал в DST).
Пример:
• в SRC (/src) има следните файлове:
/src/foof.txt
/src/1/bar.txt
/src/1/foo.txt
/src/2/1/foobar.txt
/src/2/3/barf.txt
• DST (/dst) е празна директория
• зададения низ е foo
Резултат:
• в SRC има следните файлове:
/src/1/bar.txt
/src/2/3/barf.txt
• в DST има следните файлове:
8
/dst/foof.txt
/dst/1/foo.txt
/dst/2/1/foobar.txt

```bash
#!/bin/bash

if [[ "${#}" -ne 3 ]] ;
then
    echo "There should be 3 parameters"
    exit 1
fi

src="${1}"

if [[ ! -d "${src}" ]] ;
then
    echo "1st param should be a directory"
    exit 2
fi

dst="${2}"

if [[ -d "${dst}" ]] ;
then
    if [[  $(find "${dst}" -mindepth 1 | wc -l) -ne 0 ]] ;
    then
        echo "The destination folder should be empty"
        exit 3
    fi
else
    echo "Incorrect dest dir"
    exit 4
fi

abc="${3}"
if [[ -z "${abc}" ]]
then
    echo "Empty string"
    exit 5
fi

if [[ $(id -u) -ne 0 ]] ;
then
    echo "Only root cat execute this script"
    exit 6
fi

files=$(find "${src}" -type f -name "*${abc}*")
echo "${files}"
for file in $files ;
do
    src_path=$(realpath "${src}")
    path=$(realpath "${file}" | rev | cut -d '/' -f 2- | rev | sed "s:^${src_path}/::g")
    mkdir -p "${dst}/${path}"

    mv "${file}" "${dst}/${path}/"

done

```

---

###  `2018-SE-02`
Напишете скрипт, който приема два позиционни аргумента – име на текстови файл и
директория. Директорията не трябва да съдържа обекти, а текстовият файл (US-ASCII) е стенограма
и всеки ред е в следния формат:
ИМЕ ФАМИЛИЯ (уточнения): Реплика
където:
• ИМЕ ФАМИЛИЯ присъстват задължително;
• ИМЕ и ФАМИЛИЯ се състоят само от малки/главни латински букви и тирета;
• (уточнения) не е задължително да присъстват;
• двоеточието ‘:’ присъства задължително;
10
• Репликата не съдържа знаци за нов ред;
• в стринга преди двоеточието ‘:’ задължително има поне един интервал между ИМЕ и ФАМИЛИЯ;
• наличието на други интервали където и да е на реда е недефинирано.
Примерен входен файл:
John Lennon (The Beatles): Time you enjoy wasting, was not wasted.
Roger Waters: I'm in competition with myself and I'm losing.
John Lennon:Reality leaves a lot to the imagination.
Leonard Cohen:There is a crack in everything, that's how the light gets in.
Скриптът трябва да:
• създава текстови файл dict.txt в посочената директория, който на всеки ред да съдържа:
ИМЕ ФАМИЛИЯ;НОМЕР
където:
– ИМЕ ФАМИЛИЯ е уникален участник в стенограмата (без да се отчитат уточненията);
– НОМЕР е уникален номер на този участник, избран от вас.
• създава файл НОМЕР.txt в посочената директория, който съдържа всички (и само) редовете на
дадения участник.

```bash
#!/bin/bash

if [[ ${#} -ne 2 ]] ;
then
    echo "Invalid number of parameters"
    exit 1
fi

file=${1}
dir=${2}

if [[ ! -f "${file}" ]] ;
then
    echo "Invalid file parameter"
    exit 2
fi

if [[ ! -d "${dir}"  ]] ;
then
    echo "Invalid directory parameter"
    exit 3
fi

valid_lines=$(egrep "^[a-zA-Z-]+ [a-zA-Z-]+( \(.*\))?:.*" "${file}")

if [[ ! -f "${dir}/dict.txt" ]] ;
then
    touch "${dir}/dict.txt"
fi

dict="${dir}/dict.txt"
counter=1

while read person ;
do
    if grep -q "${person}" "${dict}" ;
    then
        continue
    fi
    echo "${person}:${counter}" >> "${dict}"
    if [[ ! -f "${counter}" ]] ;
    then
        touch "${dir}/${counter}.txt"
    fi

    grep "${person}" <(echo "${valid_lines}") | cut -d ":" -f 2  | sed "s:^ ::g" > ${dir}/${counter}.txt
    ((counter+=1 ))
done < <(echo "${valid_lines}" | cut -d ':' -f 1 | sed "s:(.*)::g" | sort | uniq ) ;

```
---

###  `2018-SE-03`
Напишете скрипт, който приема два позиционни аргумента – имена на текстови файлове
в CSV формат:
8,foo,bar,baz
2,quz,,foo
12,1,3,foo
3,foo,,
5,,bar,
7,,,
4,foo,bar,baz
Валидни са следните условия:
• CSV файловете представляват таблица, като всеки ред на таблицата е записан на отделен ред;
• на даден ред всяко поле (колона) е разделено от останалите със запетая;
• броят на полетата на всеки ред е константа;
• в полетата не може да присъства запетая, т.е., запетаята винаги е разделител между полета;
• ако във файла присъстват интервали, то това са данни от дадено поле;
• първото поле на всеки ред е число, което представлява идентификатор на реда (ID).
Примерно извикване: ./foo.sh a.csv b.csv
Скриптът трябва да чете a.csv и на негова база да създава b.csv по следния начин:
• някои редове във файла се различават само по колоната ID, и за тях казваме, че формират множество 𝐴𝑖
• за всяко такова множество 𝐴𝑖
да се оставя само един ред - този, с най-малка стойност на ID-то;
• редовете, които не са членове в някое множество 𝐴𝑖
се записват в изходния файл без промяна.

```bash
#!/bin/bash

if [[ "${#}" -ne 2 ]] ;
then
    echo "Two parameters"
    exit 1
fi

input="${1}"

if [[ ! -f "${input}" ]] ;
then
    echo "Invalid file"
    exit 2
fi

output="${2}"
if [[ -e "${output}" ]] ;
then
    > "${output}"
else
    touch "${output}"
fi

removed_id=$(cut -d ',' -f 2- "${input}" | sort | uniq)
while read line ;
do
    count=$(egrep -o "^[0-9]+,${line}$" "${input}" | wc -l)
    if [[ "${count}" -lt 2 ]] ;
    then
        whole_line=$(egrep "^[0-9]+,${line}$" "${input}")
        echo "${whole_line}" >> "${output}"
    else
        echo "$(egrep "^[0-9]+,${line}$" "${input}" | sort -n -k 1 -t ',' | head -n 1)" >> "${output}"
    fi
done <<< "${removed_id}"
```

---

###  `2019-SE-01`
Напишете два скрипта (по един за всяка подточка), които четат редове от STDIN. Скриптовете трябва да обработват само редовете, които съдържат цели положителни или отрицателни числа;
останалите редове се игнорират. Скриптовете трябва да извежда на STDOUT:
а) всички уникални числа, чиято абсолютна стойност е равна на максималната абсолютна стойност сред всички числа
б) всички най-малки уникални числа от тези, които имат максимална сума на цифрите си
Примерен вход:
We don't
n11d n0
educat10n
11
12.3
6
33
-42
-42
111
111
-111
Примерен изход за а):
-111
111
Примерен изход за б):
-42

#### Решение:
```bash
#a)
#!/bin/bash

numbers_file=$(mktemp)

while read line ;
do
    if [[ ${line} =~ ^((-)?[0-9]+( )?)+$ ]] ;
    then
        echo "${line}" >> "${numbers_file}"
    fi
done

sed "s: :\n:g"

max=$(cat "${numbers_file}" | tr -d '-' | sort -n -r | head -n 1)

echo "Result:"
cat "${numbers_file}" | egrep "^(-)?${max}$" | sort | uniq


#b)
#!/bin/bash

temp=$(mktemp)

while read line ;
do
    if [[ ${line} =~ ^((-)?[0-9]+( )?)+$ ]] ;
    then
        echo "${line}" >> "${temp}"
    fi
done

max_sum=0
max_sum_num=0
nums_with_sums=$(mktemp)

while read line ;
do
    current_sum=$(echo "${line}" | sed -E "s:-?([0-9]):\1\n:g" | awk '{sum += $1} END {print sum}')
    echo "${line} ${current_sum}" >> "${nums_with_sums}"

    if [[ ${current_sum} -gt ${max_sum} ]] ;
    then
        max_sum=${current_sum}
        max_sum_num=${line}
    fi
done < <(cat "${temp}" | tr ' ' '\n')

echo "Result:"
cat "${nums_with_sums}" | egrep "^-?[0-9]+ ${max_sum}$" | sort -n -k 1 | head -n 1 | cut -d ' ' -f 1

```

###  `2019-SE-02`
Напишете шел скрипт, който приема множество параметри. Общ вид на извикване:
./foo.sh [-n N] FILE1...
В общия случай параметрите се третират като имена на (.log) файлове, които трябва да бъдат обработени от скрипта, със следното изключение: ако първият параметър е стрингът -n, то вторият параметър е число, дефиниращо стойност на променливата N, която ще ползваме в скрипта. Въвеждаме
понятието идентификатор на файл (ИДФ), което се състои от името на даден файл без разширението
.log. За удобство приемаме, че скриптът:
• ще бъде извикван с аргументи имена на файлове, винаги завършващи на .log
• няма да бъде извикван с аргументи имена на файлове с еднакъв ИДФ.
Лог файловете са текстови, като всеки ред има следния формат:
• време: timestamp във формат YYYY-MM-DD HH:MM:SS
• интервал
• данни: поредица от символи с произволна дължина
За удобство приемаме, че редовете във всеки файл са сортирани по време възходящо.
Примерно съдържание на даден лог файл:
2019-05-05 06:26:54 orthanc rsyslogd: rsyslogd was HUPed
2019-05-06 06:30:32 orthanc rsyslogd: rsyslogd was HUPed
2019-05-06 10:48:29 orthanc kernel: [1725379.728871] Chrome_~dThread[876]: segfault
Скриптът трябва да извежда на STDOUT последните N реда (ако N не е дефинирано - 10 реда) от всеки
файл, в следния формат:
• timestamp във формат YYYY-MM-DD HH:MM:SS
• интервал
• ИДФ
• интервал
• данни
Изходът трябва да бъде глобално сортиран по време възходящо.


```bash
#!/bin/bash

if [[ "${1}" == "-n" ]] ;
then
    if [[ ! "${2}" =~ ^[0-9]+$ ]] ;
    then
        echo "Invalid N"
        exit 1
    else
        N="${2}"
    fi
    shift
    shift
else
    N=10
fi

for file in "${@}" ;
do
    if [[ ! -f "${file}" || ! "${file}" =~ .*\.log ]] ;
    then
        echo "Invalid argument"
        exit 1
    fi
done

for file in "${@}" ;
do
   content="$(cat "${file}")"
   idf="$(echo "${file}" | sed -E 's:\.log::g')"
   first_part=$(echo "${content}" | cut -d ' ' -f 1,2)
   second_part=$(echo "${content}" | cut -d ' ' -f 3-)
   echo "${first_part} ${idf} ${second_part}"
done | sort | head -n "${N}"

```

###  `2020-SE-01`
Напишете shell скрипт, който получава два задължителни позиционни параметъра - име
на файл (bar.csv) и име на директория. Директорията може да съдържа текстови файлове с имена
от вида foobar.log, всеки от които има съдържание от следния вид:
Пример 1 ( loz-gw.log):
Licensed features for this platform:
Maximum Physical Interfaces : 8
VLANs : 20
Inside Hosts : Unlimited
Failover : Active/Standby
VPN-3DES-AES : Enabled
*Total VPN Peers : 25
VLAN Trunk Ports : 8
This platform has an ASA 5505 Security Plus license.
Serial Number: JMX00000000
Running Activation Key: 0e268e0c
Пример 2 ( border-lozenets.log):
Licensed features for this platform:
Maximum Physical Interfaces : 4
VLANs : 16
Inside Hosts : Unlimited
Failover : Active/Active
VPN-3DES-AES : Disabled
*Total VPN Peers : 16
VLAN Trunk Ports : 4
This platform has a PIX 535 license.
Serial Number: PIX5350007
Running Activation Key: 0xd11b3d48
13
Имената на лог файловете (loz-gw, border-lozenets) определят даден hostname, а съдържанието им
дава детайли за определени параметри на съответният хост.
Файлът bar.csv, който трябва да се генерира от вашия скрипт, е т.н. CSV (comma separated values)
файл, тоест текстови файл - таблица, на който полетата на всеки ред са разделени със запетая. Първият
ред се ползва за определяне на имената на колоните.
Скриптът трябва да създава файла bar.csv на база на log файловете в директорията. Генерираният
CSV файл от директория, която съдържа само loz-gw.log и border-lozenets.log би изглеждал така:
hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key
loz-gw,8,20,Unlimited,Active/Standby,Enabled,25,8,ASA 5505 Security Plus,JMX00000000,0e268e0c
border-lozenets,4,16,Unlimited,Active/Active,Disabled,16,4,PIX 535,PIX5350007,0xd11b3d48
Полетата в генерирания от скрипта CSV файл не трябва да съдържат излишни trailing/leading интервали. За улеснение, приемете, че всички whitespace символи във входните файлове са символа
“интервал”.

```bash
#!/bin/bash

if [[ "${#}" -ne 2 ]] ;
then
    echo "Invalid number of parameters"
    exit 1
fi

file="${1}"
dir="${2}"

if [[ -e "${file}" ]] ;
then
    > "${file}"
else
    touch "${file}"
fi

if [[ ! -d "${dir}" ]]
then
    echo "Invalid directory"
    exit 2
fi

files=$(find "${dir}" -type f -name "*.log")
if [[ $(echo "${files}" | wc -l) -lt 1 ]] ;
then
    echo "Empty directory"
    exit 3
fi

#search() {
#    sed -E "s/^${2}[[:space:]]*: ([^:]+)$/\1/g" "${1}" | tr -d ' '
#}

header="hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key"
echo "${header}" >> "${file}"

for current in ${files} ; do
    hostname=$(sed "s:^(.*).log$:\1:g" "${current}")
    filed="${hostname},"
    while read line ; do
        if [[ ${line} =~ ^This platform has an (.*) license\.$ ]] ; then
            license=$(echo "${line}" | sed -E "^s/This platform has an (.*) license\.$/\1/g" )
            field+="${license},"
            continue
        else
            value="$(echo "${line}" | sed -E "s/^[^:]+: ([^ :]+)/\1/g" ),"
            field+="${value},"
        fi
    done < "${current}"

#    hostname=$(sed "s:^(.*).log$:\1:g" "${current}")
#    mpi=$(search "${current} " "Maximum Physical Interfaces")
#    vlans=$(search "${current}" "VLANs")
#    insize_hosts=$(search "${current}" "Inside Hosts")
#    failover=$(search "${current}" "Failover")
#    VPN=$(search "${current}" "VPN-3DES-AES")
#    total_vpn_peers=$(search "${current}" "*Total VPN Peers")
#    vlan_trunk_ports=$(search "${current}" "VLAN Trunk Ports")
#    license=$(sed "^s/This platform has an (.*) license\.$/\1/g" "${file}")
#    serial_number=$(search "${current}" "Serial Number")
#    activ_key=$(search "${current}" "Running Activation Key")
#    echo "${hostname},${mpi},${vlans},${insize_hosts},${failover},${VPN},${total_vpn_peers},${vlan_trunk_ports},${license},${serial_number},${activ_key}" >> "${file}"
     echo "${field}" | sed -E "s/^(.*),$/\1/g" >> "${file}"
done

```

###  `2020-SE-02`
Напишете shell скрипт, който приема задължителен параметър - име на файл. Файлът е
log файл на HTTP сървър, в който се записват всички получени от сървъра request-и, които клиентите
са изпратили. Файлът е текстови, като на всеки ред има информация от следния вид:
35.223.122.181 dir.bg - [03/Apr/2020:17:25:06 -0500] GET / HTTP/1.1 302 0 "-" "Zend_Http_Client"
94.228.82.170 del.bg - [03/Apr/2020:17:25:06 -0500] POST /auth HTTP/2.0 400 153 "foo bar" "<UA>"
Всеки ред на файла се състои от полета, разделени с интервал. Описание на полетата с пример спрямо
първият ред от горните:
• адрес на клиент - 35.223.122.181
• име на виртуален хост (сайт) - dir.bg
• име на потребител - -
• timestamp на заявката - [03/Apr/2020:17:25:06 -0500]
• заявка - GET / HTTP/1.1 - състои се от три компонента, разделени с интервал: метод на заявката
(за удобство приемаме, че може да има само GET и POST заявки), ресурсен идентификатор, и
протокол (приемаме, че може да има само HTTP/1.0, HTTP/1.1 и HTTP/2.0 протоколи)
• код за статус на заявката - 302
• брой байтове - 0
• referer - "-" - ограден в двойни кавички, подава се от HTTP клиента, произволен низ
• user agent - "Zend_Http_Client" - ограден в двойни кавички, подава се от HTTP клиента, произволен низ
За всеки от top 3 сайта, към които има най-много заявки, скриптът трябва да изведе в долния формат:
• брой на HTTP/2.0 заявките
• брой на не-HTTP/2.0 заявките
• top 5 клиента, направили най-много заявки, завършили с код, по-голям от 302 (и броя на съответните им зявки)
dir.bg HTTP/2.0: 0 non-HTTP/2.0: 5
del.bg HTTP/2.0: 5 non-HTTP/2.0: 0
5 94.228.82.170
2 34.73.112.204
1 185.217.0.138

```bash
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
    echo "Invalid number of parameters"
    exit 1
fi

file="${1}"
if [[ ! -f "${file}" ]] ; then
    echo "Not a valid file"
    exit 2
fi

top_tree=$(cat "${file}" | cut -d ' ' -f 2 | sort | uniq -c | sort -nr  | head -n 3 | awk '{print $2}')

while read line ; do
    count1=$( grep "${line}" "${file}" | grep -E " HTTP/2.0 " | wc -l )
    count2=$( grep "${line}" "${file}" | grep -E -v " HTTP/2.0 " | wc -l )
    echo "${line} HTTP/2.0: ${count1} non-HTTP/2.0: ${count2}"
done <<< ${top_tree}


while read line ; do
    cat "${file}" | grep "${line}" | awk '$6 > 320 {print $0}' | cut -d ' ' -f 1 | sort | uniq -c | sort -nr  | head -n 5
done < <(cat "${file}" | cut -d ' ' -f 2 | sort | uniq -c | sort -nr | awk '{print $2}'  | head -n 5)

```

###  `2020-SE-06`
Под конфигурационен файл ще разбираме файл, в който има редове от вида key=value,
където key и value могат да се състоят от букви, цифри и знак “долна черта” (“_”). Освен това, във
файла може да има празни редове; може да има произволен whitespace в началото и в края на редовете,
както и около символа “=”. Също така са допустими и коментари в даден ред: всичко след символ “#”
се приема за коментар.
Под <date> ще разбираме текущото време, върнато от командата date без параметри; под <user> ще
разбираме името на текущият потребител.
Напишете shell скрипт set_value.sh, който приема 3 позиционни аргумента – име на конфигурационен файл, ключ (foo) и стойност (bar). Ако ключът:
• присъства във файла с друга стойност, скриптът трябва да:
– да закоментира този ред като сложи # в началото на реда и добави в края на реда # edited
at <date> by <user>
– да добави нов ред foo = bar # added at <date> by <user> точно след стария ред
• не присъства във файла, скриптът трябва да добави ред от вида foo = bar # added at <date>
by <user> на края на файла
Примерен foo.conf:
# route description
from = Sofia
to = Varna # my favourite city!
type = t2_1
Примерно извикване:
./set_value.sh foo.conf to Plovdiv
17
Съдържание на foo.conf след извикването:
# route description
from = Sofia
# to = Varna # my favourite city! # edited at Tue Aug 25 15:48:29 EEST 2020 by human
to = Plovdiv # added at Tue Aug 25 15:48:29 EEST 2020 by human
type = t2_1

```bash
#!/bin/bash

if [[ "${#}" -ne 3 ]] ; then
    echo "Invalid number of parameters"
    exit 1
fi

file="${1}"
if [[ ! -f "${file}" ]] ; then
    echo "Invalid first parameter: Should be a file"
    exit 2
fi

key="${2}"
value="${3}"

if [[ ! "${key}" =~ ^[a-zA-Z0-9_]+$ ]] ; then
    echo "Invalid key"
    exit 3
fi

if [[ ! "${value}" =~ ^[a-zA-Z0-9_]+$ ]] ; then
    echo "Invalid value"
    exit 4
fi

temp=$(mktemp)
while read line ; do
    if [[ ${line} =~ ^[[:space:]]*${key}[[:space:]]*=[[:space:]].*$ ]] ; then
        check_value=$(echo "${line}" | sed -E "s/^[[:space:]]*${key}[[:space:]]*=[[:space:]]*([^[:space:]]+).*$/\1/")
        if [[ "${check_value}" == "${value}" ]] ; then
            echo "${line}" >> "${temp}"
        else
            today=$(date)
            user=$(whoami)
            echo "${line}" | sed -E "s/^([[:space:]]*${key}[[:space:]]*=[[:space:]]*[^[:space:]]*.*)$/#\1 # edited at ${today} by ${user}/" >> "${temp}"
            echo "${key} = ${value} added at ${today} by ${user}" >> "${temp}"
        fi
    else
        echo "${line}" >> "${temp}"
    fi
done < "${file}"

mv "${temp}" "${file}"

```

###  `2022-IN-01`
Както знаете, при отваряне на файл с редактора vi, той създава в същата директория временен файл
с име в следния формат: точка, името на оригиналния файл, точка, swp. Например, при редактиране
на файл с име foo.txt ще се създаде временен файл с име .foo.txt.swp.
Напишете shell скрипт, който приема два задължителни позиционни аргумента – имена на директории. Примерно извикване: ./foo.sh ./dir1 /path/to/dir2/
В dir1 може да има файлове/директории, директорията dir2 трябва да е празна.
Скриптът трябва да копира всички обикновенни файлове от dir1 (и нейните под-директории) в dir2,
запазвайки директорийната структура, но без да копира временните файлове, създадени от редактора
vi (по горната дефиниция).
Забележка: За удобство приемаме, че не ни вълнува дали метаданните на обектите (owner, group,
permissions, etc.) ще се запазят.
Примерни обекти:
dir1/
dir1/a
dir1/.a.swp
dir1/b
dir1/c/d
dir1/c/.bar.swp
Обекти след изпълнението:
dir2/
dir2/a
dir2/b
dir2/c/d
dir2/c/.bar.swp

```bash
#!/bin/bash

if [[ "${#}" -ne 2 ]] ; then
    echo "Invalid number of parameters"
    exit 1
fi

dir1="${1}"
dir2="${2}"

if [[ ! -d "${dir1}" ]] || [[ ! -d "${dir2}" ]] ; then
    echo "Parameters should be directories"
    exit 2
fi

if [[ $(find "${dir2}" -mindepth 1 | wc -l) -ne 0 ]] ; then
    echo "Second dir is not empty"
    exit 3
fi

dir1_realpath=$(realpath "${dir1}")

for file in $(find "${dir1}" -type f 2>/dev/null) ; do
    if [[ ${file} =~ ^.*\.swp$ ]] ; then
        continue
    fi
    file_path=$(realpath "${file}")
    path=$(echo "${file_path}" | sed -E "s:^${dir1_realpath}/::" )

    dir_path=$(echo "$path" | rev | cut -d '/' -f 2- | rev)

    mkdir -p "${dir2}/${dir_path}" 2>/dev/null
    cp "${file}" "${dir2}/${path}"
done
```

###  `2023-CE-02`
Вие сте част от екип от физици от бъдещето, които анализират данни, събрани от различни наблюдателни точки в космоса. Разполагате с файлове с данни, които съдържат информация за разстоянието
от наблюдателна точка до различни черни дупки в космоса. Един ред от такъв файл има следния вид:
<име на черна дупка>: <разстояние> megaparsecs
Примерни файлове (point1.opt отляво и point2.opt отдясно)
Phoenix A: 1793 megaparsecs SMSS J215728.21-360215.1: 8211 megaparsecs
H1821+643: 900 megaparsecs Holmberg 15A: 195 megaparsecs
Holmberg 15A: 216 megaparsecs Phoenix A: 1823 megaparsecs
SMSS J215728.21-360215.1: 7665 megaparsecs Ton 618: 3211 megaparsecs
Вашата задача е да напишете скрипт, който приема три аргумента от командния ред (имената на два
файла с данни от наблюдателни точки и името на черна дупка) и извежда коя от двете наблюдателни
точки се намира по-близо до черната дупка.
Примерно извикване:
```bash
$ ./find_closest_point.sh point1.opt point2.opt 'Phoenix A'
point1.opt
```

```bash
#!/bin/bash

if [[ "${#}" -ne 3 ]] ; then
    echo "Invalid number of parameters"
    exit 1
fi

file1="${1}"
file2="${2}"
if [[ ! -f "${file1}" ]] || [[ ! -f "${file2}" ]] ; then
    echo "Invalid file"
    exit 2
fi

black_hole="${3}"
if [[ -z "${black_hole}" ]] ; then
    echo "Invalid name"
    exit 3
fi


res1=$(cat "${file1}" | grep "${black_hole}" | sed -E "s/^[^:]*:[[:space:]*]([0-9]+) megaparsecs$/\1/g" )
res2=$(cat "${file2}" | grep "${black_hole}" | sed -E "s/^[^:]*:[[:space:]*]([0-9]+) megaparsecs$/\1/g")

if [[ "${res1}" > ${res2} ]] ; then
    echo "${file1}"
if [[ "${res1}" == ${res2} ]] ; then
    echo "Equal"
else
    echo "${file2}"
fi
```

###  `2025-IN-01`
Вашите колеги от съседната лаборатория трябва периодично да променят правата за достъп до обекти
във файловата система на база на текстови файл с описание на необходимата конфигурация. Всеки
ред на файла дефинира правило, което гарантирано се състои от три низа, разделени с whitespace.
Примерно съдържание на файла:
/tmp/one R 700
/tmp/two A 700
/tmp/four A 750
/tmp/twenty T 750
38
Всяко правило описва на кои обекти трябва да се променят правата, като трите низа определят:
• начална директория в която да се търсят обекти за това правило; не съдържа специални символи;
• тип – една от следните три букви, която дефинира как се интерпретират битовете в третия низ:
– R (raw) – обектът трябва да има точно тези битове за права
– A (any) – обектът трябва да има вдигнат поне един от указаните битове
– T (target) – обектът трябва да има вдигнати всичките указани битове
• девет бита, дефиниращи права за достъп във файлова система, представени по стандартния
начин като трицифрено число в осмична бройна система.
Напишете bash скрипт perm.sh, който по подаден аргумент – име на файл с конфигурация, ако се
изпълнява от root потребителя, извършва необходмите промени на правата при следните изисквания:
• правилата се обработват и прилагат линейно;
• като обекти във файловата система се обработват само обикновени файлове и директории;
• всички обекти биха могли да имат специални символи в името си;
• началните директории не се третират като обекти, чиито права трябва да се променят.
Правата на намерените обекти се променят на каквито биха били, ако съответния обект е бил създаден с ефективна umask-а съответно:
• 022 за директории и
• 002 за файлове.

```bash
#!/bin/bash

if [[ "${#}" -ne 1 ]] ;
then
    echo "Invalid number of perameters"
    exit 1
fi

file="${1}"
if [[ ! -f "${file}" ]] ;
then
    echo "Not a valid file"
    exit 2
fi

if [[ $(id -u) -ne 0 ]] ; then
    echo "Only root can execute this script"
    exit 3
fi

validate() {
    if [[ "${1}" != 'R' && "${1}" != 'A' && "${1}" != 'T' ]] ;
    then 
        echo "Invalid type"
        return 1
    fi
    
    if [[ ! -d "${2}" ]] ; then
        echo "Not a valid directory"
        return 2
    fi

    if [[ "${3}" -lt 0 ]] || [[ "${3}" -gt 777 ]] ; then
        echo "Invalid parameters"
        return 3
    fi
    return 0
}

extract_field() {
  echo "${1}" | cut -d ' ' -f "${2}" 
}

find_files() {
    if [[ "${2}" == 'R' ]] ;
        find "${1}" -mindepth 1 \( -type f -o -type d \) -perm "${3}"
        return 0
    elif [[ "${2}" == 'A' ]] ;
        find "${1}" -mindepth 1 \( -type f -o -type d \) -perm /"${3}"
        return 0
    elif [[ "${2}" == 'T' ]] ;
        find "${1}" -mindepth 1 \(-type f -o -type d \) -perm -"${3}"
        return 0
    fi
    return 1
}

while IFS= read -r line ;
do 
    dir=$(extract_field "${line}" 1) 
    t=$(extract_field "${line}" 2) 
    perms=$(extract_field "${line}" 3)

    validate "${t}" "${dir}" "${perms}"
    if [[ $? -ne 0 ]] ; then
        echo "Skipping..."
        continue
    fi
    
    find_files "${dir}" "${t}" "${perms}" | while IFS= read -r -d '' file; do
        if [[ -f "${file}" ]] ;
        then 
            chmod 775 "${file}"
        elif [[ -d "${file}" ]] ;
        then 
            chmod 755 "${file}"
        fi
    done
done < "${file}"
```