# Задача 2023-IN-01: 

## Описание
Ваши колеги - асистенти по ОС - имат нужда от демострационна програма на C, която да служи като
пример за конкурентност и синхронизация на процеси.
Дефиниран е нареден списък с три думи 𝐿 = ("𝑡𝑖𝑐 " , "𝑡𝑎𝑐 " , "𝑡𝑜𝑒\𝑛"), като всяка дума е с дължина
четири знака. Напишете програма, приемаща две числа като аргументи (./main NC WC), като 𝑁𝐶 е
ограничено в интервала [1, 7], а𝑊 𝐶 - в интервала [1, 35]. Програмата трябва задължително да работи
по следния общ алгоритъм:
- началният (родителски) процес създава 𝑁𝐶 на брой процеси-наследници;
- групата процеси извеждат на stdout общо 𝑊 𝐶 на брой думи от горния списък при следните
правила:
- ако броят на думите за извеждане 𝑊 𝐶 е по-голям от общия брой думи в𝐿, след изчерпване
на думите в 𝐿 програмата започва списъка отначало колкото пъти е нужно;
- всеки процес извежда само една дума;
- първата дума се извежда от родителския процес;
- ако броят на думите за извеждане 𝑊 𝐶 е по-голям от общия брой процеси, след изчерпване
на процесите програмата започва да ги ползва от начало. Например при родителски процес
𝑃 и два процеса-наследници 𝐶1 и 𝐶2
редът на използване на процесите ще бъде 𝑃, 𝐶1, 𝐶2, 𝑃, 𝐶1, 𝐶2, 𝑃, 𝐶1, ...
- изведените думи гарантирано трябва да са по реда, дефиниран в 𝐿;
- всеки процес гарантирано започва да извежда на stdout поредната дума, след като предходния процес е приключил с извеждането на предишната.
Забележка: За конвертиране на аргументите погледнете strtol(3).
---
