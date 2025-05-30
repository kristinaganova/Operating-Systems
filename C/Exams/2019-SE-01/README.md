# Задача 2019-SE-01: Анализ на потребителски сесии

## Описание

Тази програма чете бинарен файл, съдържащ записи за потребителски сесии в система. Всеки запис съдържа следните полета:

- Уникален потребителски идентификатор (UID) – `uint32_t`
- Два резерва (не се използват) – `uint16_t`
- Време на започване на сесията (Unix time) – `uint32_t`
- Време на завършване на сесията (Unix time) – `uint32_t`

Програмата изчислява:

- Средна продължителност на всички сесии
- Дисперсия на продължителностите на всички сесии

След това за всеки потребител, който има поне една сесия с квадрат на продължителността по-голям от дисперсията, програмата извежда на стандартния изход (STDOUT):

- UID на потребителя
- Продължителността на най-дългата сесия, която отговаря на условието

---

## Изисквания

- Файлът съдържа не повече от 16384 записа (сесии).
- Във файла има не повече от 2048 различни потребителски идентификатора.
- Програмата трябва да приема един задължителен параметър – име на бинарния файл.

---