### `2024-SE-06`
Напишете скрипт reconcile.sh, който по подаден файл описващ желано състояние на файловата
система, осигурява, че то е спазено.
Скриптът ви трябва да приема един аргумент - път до файл. Всеки ред в този файл ще бъде в един от
следните формати:
• <filename> file <permissions>
Скриптът ви трябва да подсигури, че съществува обикновеният файл <filename> с права за
достъп <permissions>.
• <dirname> dir <permissions>
Скриптът ви трябва да подсигури, че съществува директорията <dirname> с права за достъп
<permissions>.
• <linkname> symlink <target>
Скриптът ви трябва да подсигури, че съществува файлът <linkname> от тип symbolic link, който
да сочи към файла <target>.
• <filename> nonexistant
Скриптът ви трябва да подсигури, че файлът <filename> не съществува.
Преди всяка поява на <permissions> е възможно да е подадена и незадължителна двойка <user owner>:<group
owner>, които представляват потребител собственик и група собственик за съответния файл. Скриптът ви трябва да подсигури, че файлът ще притежава именно тези собственици.
Примерно съдържание на файла:
/home dir root:users 0755
/home/pesho dir pesho:pesho 0700
/home/pesho/some_file file 0644
/home/pesho/some_dir dir 0755
/home/pesho/some_dir/some_link symlink ../some_file
/home/pesho/some_dir file 0755
/home/pesho/some_other_dir nonexistant
За всеки <filename>, <dirname> и <linkname>, ако съответният файл:
• вече съществува, но е от друг тип, то следва той да бъде изтрит и пресъздаден с желания тип и
метаданни.
• вече съществува и е от желания тип, то трябва единствено метаданните да бъдат обновени, ако
е нужно.
• не съществува, то той трябва да бъде създаден. Ако съответната директория, в която се намира,
не съществува, тя трябва да бъде създадена спрямо текущата маска.
Възможно е няколко реда във файла да реферират към едно и също име във файловата система. В
такъв случай приемаме, че последният ред е финалното желано състояние.
ВАЖНО: Скриптът ви може да бъде изпълняван от всякакъв потребител във всякаква среда. Важна е
проверката за грешки и обратната връзка към потребителя при неуспешна операция. При неуспешна
операция, скриптът ви не трябва да приключва работа.

```bash
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
    echo "Invalid number of parameters"
    exit 1
fi

if [[ ! -f "${1}" ]] ; then
    echo "Invalid file"
    exit 2
fi

file="${1}"

change_owners() {
    path="${1}"
    user_group="${2}"
    real_owners=$(stat -c "%U:%G" "${path}")
    if [[ -n ${user_group} ]] && [[ "${real_owners}" != "${user_group}" ]] ;
        chown "${user_group}" "${path}"
    fi
}

change_perms() {
    path="${1}"
    permissions="${2}"
    real_perms=$(stat -c '%a' "${path}")
    if [[ $(echo "${permissions}" | cut -c 2-) -ne "${real_perms}" ]] ;
        chmod "${permissions}" "${path}"
    fi
}

delete_file() {
    path="${1}"
    real_type=$(stat -c '%F' "$path")
    if [[ "${real_type}" == "directory" ]] ; then
        rm -r "${path}"
    else 
        rm "${path}"
    fi
}

to_ignore=$(mktemp)

clear_data() {
    file="${1}"
    while read line ; do
        type=$(echo "${line}" | cut -d ' ' -f 2) 
        if [[ "${type}" == "symlink" ]] ; then
            path=$(echo "${line}" | cut -d ' ' -f 3)
        else
            path=$(echo "${line}" | cut -d ' ' -f 1)
        fi

        same_files=$(echo ${line} | grep "${path}")
        if [[ $(echo "${same_files}" | wc -l) -gt 1 ]] ; then
            echo "${same_files}" | head -n -1  >> "${to_ignore}"
        fi
    done < "${file}"
}

clear_data "${file}"

while read line ; do
    path=$(echo "${line}" | cut -d ' ' -f 1)
    type=$(echo "${line}" | cut -d ' ' -f 2) 
    permissions=$(echo "${line}" | grep -E -o " [0-7]{4}")

    if [[ "${type}" == "nonexistant" ]] ; then
        if [[ -e "${path}" ]] ; then
            delete_file "${path}"
        fi
        continue
    fi

    user_group=$(echo "${line}" | sed -E "/^${path} ${type}/d" | sed -E "s/((a-zA-Z]+:[a-zA-Z]+){0,1}) [0-9]{4}/\1/")

    if [[ "${type}" == "symlink" ]] ; then
        target=$(echo "${line}" | cut -d ' ' -f 3)
        if [[ -e "${path}" ]] && [[ ! -L "${path}" ]] ; then
            delete_file "${path}"
        fi
        ln -s "${target}" "${path}"
    elif [[ "${type}" == "file" ]] ; then
        if [[ -e "${path}" ]] && [[ ! -f "${path}" ]] ; then
            delete_file ${path}
            
            dir=$(dirname "${path}")
            mkdir -p "${dir}"
            touch "${path}"
            chmod -r "${permissions}" "${path}"
            if [[ -n "${user_group}" ]] ; then
                chown "${user_group}" "${path}"
            fi
        elif [[ -f "${perms}" ]] ; then
            change_perms "${path}" "${permissions}"
            change_owners "${path}" "${user_group}"
        else
            dir=$(dirname "${path}")
            mkdir -p "${dir}"
            touch "${path}"
        fi
    elif [[ "${type}" == "dir" ]] ; then
        if [[ -e "${path}" ]] && [[ ! -d  "${path}" ]] ; then
            delete_file ${path}

            mkdir -p "${path}"        
            chmod -r "${permissions}" "${path}"
            if [[ -n "${user_group}" ]] ; then
                chown "${user_group}" "${path}"
            fi
        elif [[ -d "${path}" ]] ;
            change_perms "${path}" "${permissions}"
            change_owners "${path}" "${user_group}"
        else 
            mkdir -p "${path}"        
        fi
    fi
done < <(grep -v -f "${to_ignore}" "${file}")

rm ${to_ignore}
```

---

### `2024-SE-04`
Напишете скрипт bake.sh, който приема един задължителен аргумент - име на файл. Скриптът трябва да реализира build система, която изгражда дадения файл по следния алгоритъм.
36
Правила за изграждане на файлове
Скриптът се управлява от файл с име bakefile, който би трябвало да съществува в текущата директория, от която извикваме bake.sh. Файлът bakefile описва правила, по които се изграждат различните файлове, които bake.sh може да изгражда. Всеки ред дефинира правило за някой файл във вида:
<файл>:<зависимост 1> <зависимост 2> ... <зависимост N>:<команда>
Където:
• <файл>: името на файла, за който се отнася този ред
• <зависимост ...>: имена на файлове, които трябва да бъдат изградени преди да може да се
изгради файлът <файл>. Може да приемете, че зависимостите няма да образуват цикъл.
• <команда>: произволна команда, която изгражда файла. Може да приемете, че след извикването
на тази команда, ще е бил създаден файл с име <файл>.
Алгоритъм за изграждане на файл
• Ако за файла има запис в bakefile:
1. Изграждаме всички негови зависимости по същия алгоритъм
2. Изпълняваме изграждащата команда, но само ако ако някоя от зависимостите е по-нова
от самия файл
• Ако за файла няма запис в bakefile:
– Ако файлът съществува, bake.sh го приема за изграден и не прави нищо
– Ако файлът не съществува, това води до грешка и няма как да продължим нататък
Пример
Нека е даден следният bakefile:
chapter1.pdf:chapter1.md:pandoc -o chapter1.pdf chapter1.md
chapter2.pdf:chapter2.md drawing42.svg:pandoc -o chapter2.pdf chapter2.md
book.pdf:chapter1.pdf chapter2.pdf:pdfunite chapter1.pdf chapter2.pdf book.pdf
drawing42.svg:drawing42.dia:dia_to_svg drawing42.dia > drawing42.svg
Ако извикваме ./bake.sh book.pdf, скриптът ще трябва да изгради book.pdf. За целта първо трябва
да изгради зависимостите на book.pdf (chapter1.pdf и chapter2.pdf, които пък от своя страна имат
други зависимости), след което да изпълни командата pdfunite chapter1.pdf chapter2.pdf book.pdf,
за която приемаме, че ще създаде book.pdf (без да се интересуваме каква е командата и какво точно
прави, стига да е завършила успешно).

```bash
#!/bin/bash

if [[ "${#}" -ne 1 ]] ; then
    echo "Invalid number of arguments"
    exit 1
fi

if [[ ! -f "${1}" ]] ; then
    echo "Invalid parameter: Not a file"
    exit 2
fi

bakefile=${1}

files_to_create=$(cut -d ':' -f 1 "${bakefile}")

find_rule() {
    grep -E "^${1}" "${2}"
}

created_files=$(mktemp)

create_files() {

    file_name="${1}"
    comd="${2}"

    if grep -q "^${file_name}" "${created_files}" ; then 
        return
    fi

    dependencies=$(find_rule ${file_name} | cut -d ':' -f 2 )
    while IFS= read -r dep ; do
        if [[ ! -f "${dep}" ]] ; then
            echo "Cannot continue creating file beacuse of a missing resourse: ${file_name}"
            exit 3
        fi 

        create_files "${file_name}" "${comd}" "${created_files}"
    done < <(echo "${dependencies}")

    if [[ ! -f "${file_name}" || $(find "${dependencies}" -newer "${file_name}") ]]; then
        eval "${comd}"
        if [[ "${?}" -ne 0 ]] ; then
            echo "Not created: ${file_name} "
            exit 4
        fi
    fi
    echo "${file_name}" >> "${created_files}"
}

while read line ; do
    file_name=$(echo "${line}" | cut -d ' ' -f 1)
    comd=$(echo "${line}" | cut -d ' ' -f 3)
    create_files "${file_name}" "${comd}"
done < "${bakefile}"

rm "${created_files}"
```


---

### `2024-SE-03`
ChordPro е текстов формат за представяне на текстове на песни, анотирани с акорди. Примерен откъс:
For [Fmaj7]here am I [Em]sitting in a tin can
[Fmaj7]Far above the [Em]world
[Bb]Planet Earth is [Am]blue and there's [G]nothing I can [F]do
Всяка поредица от символи, оградена в квадратни скоби, ще наричаме акорд. Първите един или два
символа на акорда наричаме основен тон.
Напишете скрипт transpose.sh, който приема един аргумент (неотрицателно число 𝑁). Скриптът
трябва да чете текст във формàта ChordPro от stdin и да изписва аналогичен текст на stdout, заменяйки единствено основните тонове по следната схема:
A Bb B C Db D Eb E F Gb G Ab
Числото 𝑁 задава брой преходи по ребрата за замяна. Например, при изпълнение на скрипта с 𝑁 = 3
върху горния пример, резултатът би бил:
For [Abmaj7]here am I [Gm]sitting in a tin can
[Abmaj7]Far above the [Gm]world
[Db]Planet Earth is [Cm]blue and there's [Bb]nothing I can [Ab]do
Забележки:
• Моля, избягвайте повторение на код.
• Ако свирите на тромпет, използвайте 𝑁 = 3. За валдхорна, използвайте 𝑁 = 8.

```bash
#!/bin/bash

if [[ "${#}" -ne 1 ]]; then
    echo "Invalid number of parameters"
    exit 1
fi

chords="A Bb B C Db D Eb E F Gb G Ab"

num="${1}"
if [[ ! "${num}" =~ ^[0-9]+$ ]] || [[ "${num}" -eq 0 ]]; then
    echo "Invalid first parameter"
    exit 2
fi

((num = num % 12))

get_position() {
    count=1
    for chord in $chords; do
        if [[ "$chord" == "$1" ]]; then
            echo "$count"
            return
        fi
        ((count++))
    done
    echo ""
}

get_chord_by_pos() {
    pos=$1
    count=1
    for chord in $chords; do
        if [[ "$count" -eq "$pos" ]]; then
            echo "$chord"
            return
        fi
        ((count++))
    done
}

new_text=$(mktemp)
while read -r line; do
    echo "$line" >> "$new_text"
done

to_replace=$(grep -Eo "\[[^]]+\]" "$new_text")

while read -r replace; do
    full_acord="${replace:1:-1}"
    main=""
    for c in $chords; do
        if [[ "$full_acord" == "$c"* ]]; then
            main="$c"
            break
        fi
    done

    if [[ -z "$main" ]]; then
        continue
    fi

    rest="${full_acord#$main}"
    pos=$(get_position "$main")
    new_pos=$(( (pos + num - 1) % 12 + 1 ))
    new_main=$(get_chord_by_pos "$new_pos")
    new_acord="[$new_main$rest]"

    sed -i "s|\[$full_acord\]|$new_acord|g" "$new_text"

done <<< "$to_replace"

cat "$new_text"
rm "$new_text"

```

 ---

### `2024-IN-02`
Вашите колеги от съседната лаборатория се нуждаят от shell скрипт, с който да автоматизират създаването на графични диаграми, визуализиращи йерархии на наследяване на класове в C++ проекти.
Самото създаване на диаграмите се извършва от инструмента dag-ger, който приема аргумент – име
на текстови файл с описание на насочен ацикличен граф и генерира на стандартния изход визуализация на графа в SVG формат. Примерно съдържание на текстови файл с описание на граф:
Dog
Animal
Animal -> Dog
Object -> Dog
Thread -> Dog
Thread
Object
Alsatian
Dog -> Alsatian
Всеки ред на файла може да бъде в един от следните два формата:
• node – връх в графа, (Dog);
• pnode -> cnode – ребро в графа, показващо връзката от родител (pnode) към наследник (cnode),
(Animal -> Dog).
Всички файлове на даден C++ проект са в определена директория (и евентуално нейните под-директории)
като всеки клас е деклариран в някой заглавен (header) файл. Първият ред от декларацията на даден
клас е задължително в следния формат:
class CName : mode1 P1, mode2 P2,..., modeN PN
където:
• CName – име на класа; всичко след името на класа е незадължително;
• mode1, …, modeN – спецификатор за достъп (private, public или protected);
• P1,…,PN – име на родителски клас.
Примерен първи ред от декларацията на клас:
class Dog : public Animal, private Object, protected Thread
Вашият скрипт трябва да приема два позиционни аргумента – път до директория, съдържаща (гарантирано валиден) C++ проект и име на изходен графичен файл за визуализирания граф. Графът
трябва да визуализира пълната йерархия на наследяване на класовете в проекта.
Забележки: За удобство приемаме, че идентификаторите в C++ могат да съдържат малки/главни латински букви, цифри и подчертавка ’_’ и не могат да започват с цифра. За реализъм приемаме, че в
кода няма коментари.

```bash
#!/bin/bash

if [[ "${#}" -ne 2 ]] ; then
    echo "Invalid number of parameters"
    exit 1
fi

dir="${1}"
output="${2}"

if [[ ! -d "${dir}" ]] ; then
    echo "Invalid parameter: Not a dir"
    exit 2
fi

if [[ ! -f "${output}" ]] ; then
    echo "Invalid parameter: Not a file"
    exit 3
fi

files=$(find "${dir}" -type f -name "*.h")
if [[ -z "${files}" ]] ; then
    echo "No files to read"
    exit 4
fi

temp=$(mktemp)

while read line ; do

    content=$(grep -E "^class" "${line}")
    name=$(echo "${content}" | sed -E "s/^class[[:space:]]+([a-zA-Z_][a-zA-Z0-9_]*).*/\1/")

    if [[ ! "${name}" =~ ^[a-zA-Z_][a-zA-Z0-9_]* ]] ; then
        echo "Invalid class name for: ${line}"
        exit 5
    fi

    if [[ ! "${content}" =~ ":" ]] ; then
        echo "${name}" >> "${temp}"
        continue;
    fi

    parents=$(echo "${content}" | sed -E "s/^class[[:space:]]+[a-zA-Z_][a-zA-Z0-9_]* :[[:space:]]*(.*)/\1/" | tr ',' '\n')

    while read parent ; do
        extract_parent=$(echo "${parent}" | sed -E "s/(public|private|protected)//g")
        if [[ -z "${extract_parent}" ]] ; then
            echo "Invalid number"
            continue
        fi

        echo "${extract_parent}" >> "${temp}"
        echo "${extract_parent} -> ${name}" >> "${temp}"
    done < <(echo "${parents}")

    echo "${name}" >> "${temp}"
done <<< ${files}

cat "${temp}" | sed 's/^[[:space:]]*//' | sort | uniq >> "${output}"
rm "${temp}"
```

---

### `2023-SE-06`
Напишете скрипт, който копира снимки от дадена директория (която ще наричаме фотоапарата) в
нова директория (която ще наричаме библиотеката), спазвайки определена структура.
Скриптът приема пътищата на двете директории като аргументи, очаквайки, че библиотеката не
съществува и трябва да я създаде.
Снимките са всички файлове в директорията на фотоапарата и нейните поддиректории, чиито
имена завършват на .jpg. Под дата на снимка ще имаме предвид нейното време на модификация
(mtime).
Библиотеката трябва да се състои от поддиректории с имена от вида 2023-04-20_2023-04-23, означаващи затворени интервали от дни. Скриптът трябва да създаде такива поддиректории и да разположи снимките в тях, така че да са изпълнени следните свойства:
1. Всяка снимка се намира в такава директория, че датата на снимката да принадлежи на съответния интервал от дни
2. За всеки ден от всеки интервал съществува поне една снимка от този ден в този интервал
3. Няма припокриващи се и долепени интервали (между всеки два различни интервала има поне
един ден без снимка)
Оригиналните имена на снимките не ни интересуват: скриптът трябва да им даде нови имена, използвайки техния mtime, с формат, изглеждащ така:
<библиотеката>/2023-04-20_2023-04-23/2023-04-21_13:04:33.jpg
Подсказка:
```
$ date -d '2000-02-29 + 1 day' +'%Y-%m-%d'
2000-03-01
```
Бонус за още няколко точки: направете скрипта така, че да може да работи при вече съществуваща
библиотека, добавяйки снимките от фотоапарата към нея и запазвайки всички свойства. Ако снимка
с дадено име вече съществува в библиотеката, нека скриптът я пропусне.
```bash

#!/bin/bash

if [[ "${#}" -ne 2 ]] ; then
    echo "Invalid number of parameters"
    exit 1
fi

if [[ ! -d "${1}" ]] ; then
    echo "Not a valid dir"
    exit 2
fi

camera="${1}"

lib="${2}"

if [[ ! -e "${2}" ]] || [[ ! -d "${2}" ]] ; then
    mkdir "${lib}"
fi

files=$(mktemp)
find "${camera}" -type f -name '*.jpg' -printf '%TF_%TT %p\n' | sort -t ' ' -k 1,1

current_start=""
current_end=""
current_group=$(mktemp)
prev_date=""

copy_photoes() {
    while read line ; do
        if [[ -e "${1}/${line}" ]] ; then
            continue
        else
            mv "${line}" "${1}"
        fi
    done < "${2}"
}

while read photo ; do
    date=$(stat -c '%y' "${photo}" | awk '${print $1}') 
    name=$(echo "${date}" | sed -E "s/^([0-9]{4}-[0-9]{2}-[0-9]{2}) ([0-9]{2}:[0-9]{2}:[0-9]{2}).*/\1_\2.jpg/")
    if [[ -z "${current_start}" ]] ; then
        current_start="${date}"
        prev_date="${date}"
        echo "${name}" >> "${current_group}"
    else
        if [[ "${date}" == "${prev_date}" || "${date}" == $(date -d "${prev_date} + 1 day" +'%Y-%m-%d') ]] ; then
            echo "${photo}" >> "${current_group}"
            current_end=${date}
            continue
        else
            mkdir -p "${lib}/${current_start}_${current_end}"
            copy_photoes "${lib}/${current_start}_${current_end}" "${current_group}"
            > ${current_group}
        fi
    fi
done < "${files}"
```

## Note: It is not tested 

---

### `2023-SE-04`
Напишете скрипт, който открива еднакви по съдържание файлове в дадена директория и използва
тази информация, за да намали заетото място на диска.
Скриптът приема един параметър — име на директория. Примерно извикване: ./dedup.sh ./mydir
Скриптът трябва да направи две неща:
• ако има файлове с еднакво съдържание, да направи така, че имената на тези файлове да сочат
към едно и също копие на съответните данни
• да изведе съобщение, съдържащо следната информация:
– колко групи файлове са дедупликирани
– колко байта от мястото на диска се е освободило
Забележки:
• считаме, че цялата дадена директория се намира върху една файлова система
• ако два файла имат еднакви хеш-суми, считаме, че са еднакви по съдържание

```bash
#!/bin/bash

if [[ ${#} -ne 1 ]] ; then
    echo "Wrong number of arguments"
    exit 1
fi

dir="${1}"

if [[ ! -d "${dir}" ]] ; then
    echo "${dir} must be a dir"
    exit 2
fi

temp=$(mktemp)

while read file ; do

    echo "${file} $(sha256sum "${file}")" >> "${temp}"

done < <(find "${dir}" -type f)

count=0
copies_size=0
links_size=0

while read hash ; do

    to_stay=$(grep " ${hash}" "${temp}" | head -n 1 | cut -d ' ' -f 1) 
    copies=$(grep " ${hash}" "${temp}" | tail -n +2 | cut -d ' ' -f 1)

    if [[ -z "${copies}" ]] ; then
        continue
    fi

    while read copy ; do

        (( copies_size += $(stat -c '%s' "${copy}") ))
        rm "${copy}"

        ln "${to_stay}" "${copy}"
        (( links_size += $(stat -c '%s' "${copy}") )) 

    done < <(echo "${copies}" | cut -d ' ' -f 1)

    (( count += 1 ))

done < <(cut -d ' ' -f 2 "${temp}" | sort -n | uniq)

freed_space=$((copies_size - links_size))

echo "Groups files dedublicated: ${count}"
echo "Freed space: ${freed_space}bytes"

rm "${temp}"

#logic
#find all hashs
#iterrate over each hash and from every copy create link to the first
#count size of the copies and then substarct it
#count hashes count to get the groups count
```

---

### `2023-SE-03`
При статистическа обработка на текстове често се налага да имаме списък от думи (наречени “стопдуми”), които се срещат прекалено често и не носят стойност за изследването. Такива думи са например “you”, “then”, “for” и т.н.
Напишете скрипт stopword_candidates.sh, който приема като аргумент име на директория и извежда 10-те думи, които най-много изглеждат като стоп-думи.
• За да бъде стоп-дума, трябва броят файлове, които я съдържат ≥ 3 пъти да е ≥
общия брой файлове
2
• Една дума е по-добър кандидат от друга, ако има по-голям общ брой срещания във всички файлове
Забележки:
• Под “всички файлове” имаме предвид всички обикновени файлове в дадената директория и
нейните поддиректории
• Под “дума” имаме предвид непрекъсната последователност от латински букви (a-z) - всички
останали символи не са част от думите
31
• За улеснение може да приемете, че във файловете няма главни букви

```bash
#!/bin/bash
if [[ ${#} -ne 1 ]] ; then
    echo "Wrong number of parameters"
    exit 1
fi

if [[ ! -d "${1}" ]] ; then
    echo "${1}: Not a valid directory"
    exit 2
fi

dir="${1}"

files_count=$(find "${dir}" -type f | wc -l)
temp=$(mktemp)

while read file ; do 
    words=$(grep -Eo "\<[a-zA-Z]+\>" "${file}" | sort | uniq -c | awk -v count="${files_count}" '$1>=3 && $1>=count/2 {print $2}')
    echo "${words}" >> "${temp}"
done < <(find "${dir}" -type f)

cat "${temp}" | sort | uniq -c | sort -r -n -k 1 -t ' ' | awk '{print $2}' | head -n 10 

rm "${temp}"
```

---

### `2023-SE-01`
Напишете скрипт, който цензурира всички срещания на “забранени” думи в дадени текстове.
Примерно извикване: ./redact.sh bad_words.lst ./my_texts.
Първият аргумент на скрипта е име на текстов файл, съдържащ по една забранена дума на ред:
cake
cakes
shake
banana
pine_apple42
shakinator
Вторият аргумент е име на директория: интересуват ни всички файлове в нея и в нейните поддиректории, чиито имена завършват на .txt.
30
Скриптът ви трябва да подмени всички срещания на забранени думи във въпросните файлове с брой
звездички, съответстващ на дължината на думата. Подменят се само цели срещания на думи.
Например, ако имаме файл ./my_texts/shake.txt със съдържание:
to make banana shake, we start by blending four bananas.
след изпълнение на скрипта, съдържанието му трябва да е:
to make ****** *****, we start by blending four bananas.
Под “дума” разбираме последователност от букви, цифри и долни черти.
За улеснение, може да приемете, че разглеждаме само малки букви (никъде не се срещат главни букви).

```bash
#!/bin/bash

if [[ "${#}" -ne 2 ]] ; then
    echo "Not a valid number of parameters: should be 2"
    exit 1
fi

if [[ ! -f "${1}" ]] ; then
    echo "Not a valid file: ${1}"
    exit 2
fi

if [[ ! -d "${2}" ]] ; then
    echo "Not a valid directory: ${2}"
    exit 3
fi

to_censore="${1}"
dir="${2}"

while read word ; do
    censore=$(echo "${word}" | tr '[a-zA-Z0-9_]' '*')
    while read -r file ; do
        sed -E -i "s/\<${word}\>/${censore}/Ig" "${file}"
    done < <(find "${dir}" -type f -name "*.txt" 2>/dev/null)
done < <(cat "${to_censore}" )
```

