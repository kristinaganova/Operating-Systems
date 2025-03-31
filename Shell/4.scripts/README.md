## Task 05-b-2000
### Да се напише shell скрипт, който приканва потребителя да въведе низ (име) и изпечатва "Hello, низ".

#### Commands:
```bash
#!/bin/bash

while true;
do
    echo "Enter your name: "
    read name

    if [ -z "$name" ];
    then
        echo "Invalid name"
    else
        echo "Hello, $name"
        break
    fi
done
```

## Task 05-b-2800
### Да се напише shell скрипт, който приема точно един параметър и проверява дали подаденият му параметър се състои само от букви и цифри.

#### Commands:
```bash
#!/bin/bash

if [[ $# -ne 1 ]];
then
    echo "Invalid number of arguments"
    exit 1
fi

if [[ ! $1 =~ ^[a-zA-Z0-9]+$ ]];
then
    echo "Invalid input"
    exit 2
fi

echo 'correct'
```

## Task 05-b-3100
### Да се напише shell скрипт, който приканва потребителя да въведе низ - потребителско име на потребител от системата - след което извежда на стандартния изход колко активни сесии има потребителят в момента.

#### Commands:
```bash
#!/bin/bash

echo "Enter username: "
read username

if ! grep -q "${username}" /etc/passwd ;
then
    echo "Username $username does not exists"
    exit 1
fi

echo "Active sessions: "
grep $username <( who | cut -d ' ' -f 1 | sort | uniq -c ) | awk '{ print $1 }'
```

## Task 05-b-3200
### Да се напише shell скрипт, който приканва потребителя да въведе пълното име на директория и извежда на стандартния изход подходящо съобщение за броя на всички файлове и всички директории в нея.

#### Commands:
```bash
#!/bin/bash

echo "Enter path to folder"
read path

if [ ! -d "$path" ];
then
    echo "The path is incorrect"
    exit 1
fi

echo "Number of files: "
find "$path" -type f | wc -l

echo "Number of directories: "
find "$path" -type d | wc -l
```

## Task 05-b-3300
### Да се напише shell скрипт, който чете от стандартния вход имената на 3 файла, обединява редовете на първите два (man paste), подрежда ги по азбучен ред и резултата записва в третия файл.

#### Commands:
```bash
#!/bin/bash

if [[ ! -f ${1} ]];
then
    echo "${1} is not found"
elif [[ ! -f ${2} ]];
then
    echo "${2} is not found"
elif [[ ! -f ${3} ]];
then
    echo "${3} is not found. Creating..."
    touch ${3}
fi

paste "${1}" "${2}" | sort > "${3}"
cat "${3}"
```

## Task 05-b-3400
### Да се напише shell скрипт, който чете от стандартния вход име на файл и символен низ, проверява дали низа се съдържа във файла и извежда на стандартния изход кода на завършване на командата с която сте проверили наличието на низа.
NB! Символният низ може да съдържа интервал (' ') в себе си.

#### Commands:
```bash
#!/bin/bash

if [[ "${#}" -ne 2 ]];
then
    echo "Not enough arguments"
    exit 1
fi

if [[ ! -f "${1}" ]];
then
    echo "File does not exist"
    exit 1
fi

if [[ -z "${2}" ]];
then
    echo "String is empty"
    exit 2
fi

grep -q "${2}" "${1}";

if [[ $? == 0 ]];
then
    echo "${1} constains ${2}"
else
    echo "${1} does not contain ${2}"
fi
```

## Task 05-b-4200
### Имате компилируем (a.k.a няма синтактични грешки) source file на езика C. Напишете shell script, който да покaзва колко е дълбоко най-дълбокото nest-ване (влагане).
Примерен .c файл:

#include <stdio.h>

int main(int argc, char *argv[]) {

  if (argc == 1) {
		printf("There is only 1 argument");
	} else {
		printf("There are more than 1 arguments");
	}

	return 0;
}
Тук влагането е 2, понеже имаме main блок, а вътре в него if блок.

Примерно извикване на скрипта:

./count_nesting sum_c_code.c

Изход:
The deepest nesting is 2 levels

#### Commands:
```bash
#!/bin/bash

if [[ $# -ne 1 ]];
then
    echo "There should be one arguemnt"
    exit 1
fi

file=$1
max=0
currentCount=0

while read ch; do
    if [[ ${ch} == "{" ]];
    then
        currentCount=$(( currentCount + 1 ))
    else
        if [[ max -lt currentCount ]];
        then
            max=${currentCount}
        fi

        currentCount=$(( currentCount - 1 ))
    fi
done < <( cat "${1}" | grep -E -o '[{}]')

echo ${max}
```

## Task 05-b-4400
### Напишете shell script, който да приема параметър име на директория, от която взимаме файлове, и опционално експлицитно име на директория, в която ще копираме файлове. Скриптът да копира файловете със съдържание, променено преди по-малко от 45 мин, от първата директория във втората директория. Ако втората директория не е подадена по име, нека да получи такова от днешната дата във формат, който ви е удобен. При желание новосъздадената директория да се архивира.

#### Commands:
```bash
#!/bin/bash

dir=${1}
copyDir=${2}

if [[ ${#} -lt 1 ]];
then
    echo "There should be 1 or 2 "
    exit 1
fi

if [[ ${#} -gt 2 ]];
then
    echo "There should be 1 or 2 "
    exit 1
fi

if [[ ! -d ${dir} ]];
then
    echo "Not valid directory"
    exit 2
fi

if [[ -z ${2} ]];
then
    copyDir=$(date -I)
    mkdir -p ${copyDir}
fi

find "${dir}" -mindepth 1 -maxdepth 1 -type f -mmin -45 -exec cp -r {} "${copyDir}" \;
```

## Task 05-b-4500
### Да се напише shell скрипт, който получава при стартиране като параметър в командния ред идентификатор на потребител. Скриптът периодично (sleep(1)) да проверява дали потребителят е log-нат, и ако да - да прекратява изпълнението си, извеждайки на стандартния изход подходящо съобщение.

NB! Можете да тествате по същият начин като в 05-b-4300.txt

#### Commands:
```bash
id=${1}

if [[ -z ${id} ]];
then
    echo "Id is empty"
    exit 1
fi

if ! (cat /etc/passwd | grep -q "${1}");
then
    echo "User ${id} does not exist"
    exit 2
fi

while true; do
    if who | grep -q "${id}";
    then
        echo "User ${id} is logged in"
        exit 0
    fi

    sleep 1
done
```
## Task 05-b-4600
### Да се напише shell скрипт, който валидира дали дадено цяло число попада в целочислен интервал.
Скриптът приема 3 аргумента: числото, което трябва да се провери; лява граница на интервала; дясна граница на интервала.
Скриптът да връща exit status:
- 3, когато поне един от трите аргумента не е цяло число
- 2, когато границите на интервала са обърнати
- 1, когато числото не попада в интервала
- 0, когато числото попада в интервала

Примери:
$ ./validint.sh -42 0 102; echo $?
1

$ ./validint.sh 88 94 280; echo $?
1

$ ./validint.sh 32 42 0; echo $?
2

$ ./validint.sh asdf - 280; echo $?
3

#### Commands:
```bash
#!/bin/bash

if [[ ${#} -ne 3 ]];
then
    echo "The number of arguments should be 3"
fi

for arg in "${@}" ; do
    if ! ( echo "${arg}" | grep -q -E '^(-){0,1}[0-9]+$' ) ; then
        echo "Argument "${arg}" is not a number"
        exit 3
    fi
done

number=${1}
lowerBound=${2}
upperBound=${3}

if [[ ${upperBound} -lt ${lowerBound} ]];
then
    echo "Upper and lower bound are reversed"
    exit 2
fi

if [[ ${number} -ge ${lowerBound} && ${number} -le ${upperBound} ]];
then
    echo "Number is in interval"
    exit 0
else
    echo "Number is not in interval"
    exit 1
fi

```

## Task 05-b-4700
### Да се напише shell скрипт, който форматира големи числа, за да са по-лесни за четене. Като пръв аргумент на скрипта се подава цяло число. Като втори незадължителен аргумент се подава разделител. По подразбиране цифрите се разделят с празен интервал.

Примери:
$ ./nicenumber.sh 1889734853
1 889 734 853

$ ./nicenumber.sh 7632223 ,
7,632,223
#### Commands:
```bash
#!/bin/bash

if ! ( echo "${1}" | grep -E -q '^(-){0,1}[0-9]+$') ;
then
    echo "The first argument is not a number"
    exit 1
fi

sep=' '
if [[ ${#} -eq 2 ]];
then
    if  echo "${2}" | grep -E -q '^.$' ;
    then
        sep=${2}
    else
        echo "The separattor should single character"
        exit 2
    fi
fi

echo "${1}" | rev | sed -E "s/(...)/\1${sep}/g" | rev | sed -E "s/^${sep}(.*)$/\1/g"

```

## Task 05-b-4800
### Да се напише shell скрипт, който приема файл и директория. Скриптът проверява в подадената директория и нейните под-директории дали съществува копие на подадения файл и отпечатва имената на намерените копия, ако съществуват такива.

NB! Под 'копие' разбираме файл със същото съдържание.

#### Commands:
```bash
#!/bin/bash

if [[ ! -f ${1} ]];
then
    echo "File does not exist"
    exit 1
fi

if [[ ! -d ${2} ]];
then
    echo "Invalid directory"
    exit 2
fi

find ${2} -type f -exec diff {} ${1} 2>/dev/null \; 2>/dev/null

#for file in $( find ${2} -type f 2>/dev/null ) ;
#do
#    diff -q ${file} ${1} > /dev/null
#    if [[ ${?} -eq 0 ]];
#    then
#        echo "${file}"
#    fi
#done

```

## Task 05-b-6600
### Да се напише shell script, който генерира HTML таблица съдържаща описание на потребителите във виртуалката ви. Таблицата трябва да има:
- заглавен ред с имената нa колоните
- колони за username, group, login shell, GECOS field (https://en.wikipedia.org/wiki/Gecos_field)

Пример:
$ ./passwd-to-html.sh > table.html
$ cat table.html

```html
<table>
  <tr>
    <th>Username</th>
    <th>group</th>
    <th>login shell</th>
    <th>GECOS</th>
  </tr>
  <tr>
    <td>root</td>
    <td>root</td>
    <td>/bin/bash</td>
    <td>GECOS here</td>
  </tr>
  <tr>
    <td>ubuntu</td>
    <td>ubuntu</td>
    <td>/bin/dash</td>
    <td>GECOS 2</td>
  </tr>
</table>
```

#### Commands:
```bash

#!/bin/bash

users=$(who | cut -d ' ' -f 1 )

get_field() {
    egrep "^${1}" /etc/passwd | cut -d ":" -f "${2}" | sed -E 's:(.*):<td>\1</td>:g'
}

echo "<table>"

    echo -e "\t<tr>"
    echo -e "\t\t<th>Username</th>"
    echo -e "\t\t<th>group</th>"
    echo -e "\t\t<th>login shell</th>"
    echo -e "\t\t<th>GECOS</th>"
    echo -e "\t</tr>"

for user in $users;
do
    name=$(get_field "${user}" 1)
    group=$(get_field "${user}" 4)
    gecos=$(get_field "${user}" 5 | tr ' ' '.' | sed -E "s:^([^,]*).*$:\1</th>:g" )
    login_shell=$(get_field "${user}" 7 )
    echo -e "\t<tr>"
    echo -en "\t\t"
    echo "${name}"
    echo -en "\t\t"
    echo "${group}"
    echo -en "\t\t"
    echo "${login_shell}"
    echo -en "\t\t"
    echo "${gecos}"
    echo -e "\t</tr>"
done

echo "</table>"

```



## Task 05-b-4800
### 
#### Commands:
```bash
#!/bin/bash
```

## Task 05-b-6600
### Да се напише shell скрипт, който получава единствен аргумент директория и изтрива всички повтарящи се (по съдържание) файлове в дадената директория. Когато има няколко еднакви файла, да се остави само този, чието име е лексикографски преди имената на останалите дублирани файлове.

```
Примери:
$ ls .
f1 f2 f3 asdf asdf2
asdf и asdf2 са еднакви по съдържание, но f1, f2, f3 са уникални

$ ./rmdup .
$ ls .
f1 f2 f3 asdf
asdf2 е изтрит
```

#### Commands:

```bash
#!/bin/bash

if [[ ${#} -ne 1 ]] ;
then
    echo "1 param allowed - directory"
    exit 1
fi

if [[ ! -d ${1} ]] ;
then
    echo "Not valid directory"
    exit 2
fi

files=$(find ${1} -mindepth 1 -maxdepth 1 -type f)

for file in $files ; do
    for file2 in $files ; do
        if [[ "$file" != "$file2" ]] ;
        then
            if diff "$file" "$file2" >& /dev/null ;
            then
                if [[ "${file}" > "${file2}" ]] ;
                then
                    rm -i "${file}"
                    echo "Removing ${file}"
                 else
                    echo "Removing ${file2}"
                    rm -i "${file2}"
                fi
            fi
        fi
    done
done
```

## Task 05-b-6800
### Да се напише shell скрипт, който получава единствен аргумент директория и отпечатва списък с всички файлове и директории в нея (без скритите). До името на всеки файл да седи размера му в байтове, а до името на всяка директория да седи броят на елементите в нея (общ брой на файловете и директориите, без скритите). 

```
a Добавете параметър -a, който указва на скрипта да проверява и скритите файлове и директории.

Пример:
$ ./list.sh .
asdf.txt (250 bytes)
Documents (15 entries)q
empty (0 entries)
junk (1 entry)
karh-pishtov.txt (8995979 bytes)
scripts (10 entries)

Примери:
$ ls .
f1 f2 f3 asdf asdf2
asdf и asdf2 са еднакви по съдържание, но f1, f2, f3 са уникални

$ ./rmdup .
$ ls .
f1 f2 f3 asdf
asdf2 е изтрит
```

#### Commands:

```bash

#!/bin/bash

if [[ ${#} -eq 0 ]] || [[ ${#} -gt 2 ]] ;
then
    echo "Parameturs can only be - Directory -a(optional)"
    exit 1
fi

if [[ ${#} -eq 2 ]] && [[ ${2} != "-a" ]] ;
then
    echo "Incorrect second param"
    exit 2
fi

if [[ ! -d ${1} ]] ;
then
    echo "The argument should be a directory"
    exit 2
fi

if [[ ${#} -eq 2 ]] && [[ ${2} == "-a" ]] ;
then
    results=$(find "${1}" -mindepth 1 -maxdepth 1 -type f)
else
    results=$(find "${1}" -mindepth 1 -maxdepth 1 -type f ! -name ".*")
fi

for line in $results ; do
    if [[ -d $line ]] ;
    then
        count=$(find ${line} -mindepth 1 -maxdepth 1 | wc -l)
        echo "${line} ${count}"
    elif [[ -f $line ]] ;
    then
        size=$(stat -c "%s" ${line})
        echo "${line} ${size}"
    fi
done

```

## Task 05-b-7000
### Да се напише shell скрипт, който приема произволен брой аргументи - имена на файлове. Скриптът да прочита от стандартния вход символен низ и за всеки от зададените файлове извежда по подходящ начин на стандартния изход броя на редовете, които съдържат низа.

NB! Низът може да съдържа интервал.

#### Commands:

```bash
#!/bin/bash

if [[ ${#} -eq 0]] ;
then
    echo "There should be at least one param"
    exit 1
fi

read -p "String: " string

while [[ ${#} -ne 0 ]] ;
do
    file=${1}
    shift
    count=$(grep -E "^.*${string}.*$" "${file}" | wc -l)
    echo "${file} ${count}"
done

```

## Task 05-b-7100
### Да се напише shell скрипт, който приема два параметъра - име на директория и число. Скриптът да извежда на стандартния изход имената на всички обикновени файлове във директорията, които имат размер, по-голям от подаденото число.

#### Commands:

```bash
#!/bin/bash

if [[ ${#} -ne 2 ]] ;
then
    echo "There should be 2 parameters"
    exit 1
fi

if [[ ! -d ${1} ]] ;
then
    echo "First parameter should be a directory"
    exit 2
fi

if [[ ! ${2} =~ ^[0-9]+$ ]] ;
then
    echo "Second parameter should be a number"
    exit 3
fi

echo "$(find "${1}" -mindepth 1 -maxdepth 1 -type f -size +"${2}"c)"
```

## Task 05-b-7200
### Да се напише shell скрипт, който приема произволен брой аргументи - имена на файлове или директории. Скриптът да извежда за всеки аргумент подходящо съобщение:
	- дали е файл, който може да прочетем
	- ако е директория - имената на файловете в нея, които имат размер, по-малък от броя на файловете в директорията.

#### Commands:

```bash
#!/bin/bash

while [[ ${#} -ne 0 ]] ;
do
    echo "Inf for ${1}:"
    if [[ -f ${1} ]] ;
    then
        if cat "${1}" >/dev/null 2>$1; then
            echo "${1} is readable"
        fi
    fi

    if [[ -d ${1} ]] ;
    then
        count_files=$(find "${1}" -mindepth 1 -maxdepth 1 -type f | wc -l)
        echo "Directory ${1} has: ${count_files} files. And these has size smaller than the count of files: "
        echo "$(find "${1}" -mindepth 1 -maxdepth 1 -type f -size -${count_files})c"
    fi
    shift
    echo -e "\n"
done

```

## Task 05-b-7500
### Напишете shell script guess, която си намисля число, което вие трябва да познате. В зависимост от вашия отговор, програмата трябва да ви казва "надолу" или "нагоре", докато не познате числото. Когато го познаете, програмата да ви казва с колко опита сте успели.

./guess (програмата си намисля 5)

Guess? 22
...smaller!
Guess? 1
...bigger!
Guess? 4
...bigger!
Guess? 6
...smaller!
Guess? 5
RIGHT! Guessed 5 in 5 tries!

Hint: Един начин да направите рандъм число е с $(( (RANDOM % b) + a  )), което ще генерира число в интервала [a, b]. Може да вземете a и b като параметри, но не забравяйте да направите проверката.

#### Commands:

```bash
#!/bin/bash

read -p "Enter lower bound: " a

read -p "Enter upper bound: " b

check_input() {
    if [[ ! ${1} =~ ^[0-9]+$ ]] ;
    then
        echo "Invalid input for ${1}"
        exit 1
    fi
}

check_input "${a}"
check_input "${b}"

if [[ ${a} -gt  ${b} ]] ;
then
    temp=$a
    a=$b
    b=$temp
fi

toGuess=$(( (RANDOM%$b) + a ))
count=0

while read -p "Guess: " number ;
do

    if [[ ! ${number} =~ ^[0-9]+$ ]] ;
    then
        echo "Not a number. Try again "
        continue
    fi

    ((count++))
    if [[ ${number} -eq ${toGuess} ]];
    then
        echo "RIGHT! Guessed ${toGuess} in ${count} tries!"
        exit 0
    elif [[ ${number} -lt  ${toGuess} ]] ;
    then
        echo "...bigger"
        continue
    elif [[ ${number} -gt  ${toGuess} ]] ;
    then
        echo "...smaller"
        continue
    fi
done

```

## Task 05-b-7550
### Да се напише shell скрипт, който приема параметър - име на потребител. Скриптът да прекратява изпълненито на всички текущо работещи процеси на дадения потребител, и да извежда колко са били те.

NB! Може да тествате по същият начин като описаният в 05-b-4300

#### Commands:

```bash
#!/bin/bash

if [[ ${#} -gt 1 ]] ;
then
    echo "Only one parameter allowed"
    exit 1;
elif [[ ${#} -eq 0 ]] ;
then
    echo "Provide at least one parameter "
    exit 2
fi

if [[ -z ${1} ]] ;
then
    echo "Username sannot be empty string"
    exit 3

processes=$(ps -e -u ${1} -o pid= )
count=$(echo "${processes}" | wc -l)

for pid in ${processes} ;
do
    kill -9 "${pid}"
done

echo "Killed ${count} processes"

```

## Task 05-b-7700
### Да се напише shell скрипт, който приема два параметъра - име на директория и число. Скриптът да извежда сумата от размерите на файловете в директорията, които имат размер, по-голям от подаденото число.

#### Commands:

```bash
 #!/bin/bash

if [[ ${#} -ne 2 ]] ;
then
    echo "There should be 2 parameters"
    exit 1
fi

dir=${1}
num=${2}

if [[ ! -d ${dir} ]] ;
then
    echo "Invalid argument: 1st parameter should be a directory"
    exit 2
fi

if [[ ! ${num} =~ ^[0-9]+$ ]] ;
then
    echo "Invalid argument: 2nd patameter should be a number"
    exit 3
fi

files=$( find "${dir}" -mindepth 1 -maxdepth 1 -type f -size +"${num}"c -exec stat -c %s {} \; )

sum=0
for size in ${files} ;
do
    ((sum+=size))
done

echo "${sum}"

#or
# find "${dir}" -mindepth 1 -maxdepth 1 -type f -size +"${num}"c -exec stat -c %s {} \; | awk '{sum+=$1} END {print sum}'

```

## Task 05-b-7800
### Да се напише shell скрипт, който намира броя на изпълнимите файлове в PATH.
Hint: Предполага се, че няма спейсове в имената на директориите
Hint2: Ако все пак искаме да се справим с този случай, да се разгледа IFS променливата и констуркцията while read -d

#### Commands:

```bash

#!/bin/bash

if [[ "${#}" -eq 0 ]] ;
then
    echo "There should be 1 parameter"
    exit 1
fi

if [[ ! -d "${1}" ]] ;
then
    echo "The parameter should be a directory"
    exit 2
fi

count=$(find "${1}" -mindepth 1 -maxdepth 1 -type f -executable | wc -l)
echo "${count}"

```

## Task 05-b-8000
### Напишете shell script, който получава като единствен аргумент име на потребител и за всеки негов процес изписва съобщение за съотношението на RSS към VSZ. Съобщенията да са сортирани, като процесите с най-много заета виртуална памет са най-отгоре.

Hint:
Понеже в Bash няма аритметика с плаваща запетая, за смятането на съотношението използвайте командата bc. За да сметнем нампример 24/7, можем да: echo "scale=2; 24/7" | bc
Резултатът е 3.42 и има 2 знака след десетичната точка, защото scale=2.
Алтернативно, при липса на bc ползвайте awk.

#### Commands:

```bash
#!/bin/bash

if [[ ${1} -ne 1 ]] ;
then
    echo "1 parameter"
    exit 1
fi

if ! egrep "${1}" /etc/passwd ;
then
    echo "Invalid user"
    exit 2
fi

processes=$( ps -e -u "${1}" -o user=,pid=,tty=,rss=,vsz= -sort -vsz | awk '{print $4/$5}' )

```

## Task 05-b-9100
### Опишете поредица от команди или напишете shell скрипт, които/който при известни две директории SOURCE и DESTINATION:
- намира уникалните "разширения" на всички файлове, намиращи се някъде под SOURCE. (За простота приемаме, че в имената на файловете може да се среща символът точка '.' максимум веднъж.)
- за всяко "разширение" създава по една поддиректория на DESTINATION със същото име
- разпределя спрямо "разширението" всички файлове от SOURCE в съответните поддиректории в DESTINATION

#### Commands:

```bash
#!/bin/bash

if [[ ${#} -ne 2 ]] ;
then
    echo "Invalid number parameters."
    exit 1
fi

if [[ ! -d "${1}" ]] ;
then
    echo "${1} is not a directory"
    exit 2
fi

if [[ ! -d "${2}" ]] ;
then
    echo "No directory with name ${2}. Creating..."
    mkdir "${2}"
fi

files=$( find ${1} -mindepth 1 -maxdepth 1 -type f)

for line in ${files} ;
do
    extension=$(echo "${line}" | awk -F '.' '{print $NF}')
    if [[ ! -d "${destination}/${extension}" ]] ;
    then
        mkdir "${destination}/${extension}"
    fi
    cp "${line}" "${destination}/${extension}"
done

```

## Task 05-b-9200
### Да се напише shell скрипт, който получава произволен брой аргументи файлове, които изтрива. Ако бъде подадена празна директория, тя бива изтрита. Ако подадения файл е директория с поне 1 файл, тя не се . За всеки изтрит файл (директория) скриптът добавя ред във log файл с подходящо съобщение.

Името на log файла да се чете от shell environment променлива, която сте конфигурирали във вашия .bashrc.
Добавете параметър -r на скрипта, който позволява да се изтриват непразни директории рекурсивно.
Добавете timestamp на log съобщенията във формата: 2018-05-01 22:51:36

Примери:
$ export RMLOG_FILE=~/logs/remove.log
$ ./rmlog -r f1 f2 f3 mydir/ emptydir/
$ cat $RMLOG_FILE
[2018-04-01 13:12:00] Removed file f1
[2018-04-01 13:12:00] Removed file f2
[2018-04-01 13:12:00] Removed file f3
[2018-04-01 13:12:00] Removed directory recursively mydir/
[2018-04-01 13:12:00] Removed directory emptydir/

#### Commands:

```bash
#!/bin/bash

if [[ "${#}" -lt 1 ]] ;
then
    echo "Not enough parameturs"
    exit 1
fi

flag=0
if [[ "${1}" == "-r" ]] ;
then
    flag=1
    shift
fi

while [[ "${#}" -ne 0 ]] ;
do
    currentDate=$(date +'%Y-%m-%d %H:%M:%S')

    if [[ -d "${1}" ]] ;
    then

        countFiles=$( find "${1}" -mindepth 1 | wc -l )
        if [[ "${countFiles}" -eq 0 ]] ;
        then
            echo "[${currentDate}] removing: ${1}"
            rmdir "${1}"
        elif [[ "${countFiles}" -gt 0 ]] && [[ "${flag}" -eq 1 ]] ;
        then
            echo "[${currentDate}] removing non empty directory ${1}"
            rm -r "${1}"
        fi

    elif [[ -f "${1}" ]] ;
    then
        echo "[${currentDate}] removing file: ${1}"
        rm -i "${1}"
    fi
    shift

done

```