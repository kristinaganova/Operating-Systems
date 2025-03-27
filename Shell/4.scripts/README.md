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