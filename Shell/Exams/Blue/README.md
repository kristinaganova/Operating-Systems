##  Shell Tasks 

###  `2017-SE-01`
**Task**: Find the names of the top 5 files in the current directory with the most hardlinks.

```bash
find . -exec stat -c "%h %n" {} \; | sort -k 1 -t ' ' -nr | head -n 5 | cut -f 2 -d ' '
```

---

###  `2018-SE-02`
**Task**: Print only the inode of the most recently modified file (by content) under `/home/pesho` that has more than one name (hardlink).

```bash
find /home/pesho -links +1 -printf "%T@ %i %f\n" | sort -n -k 1 -t ' ' | cut -f 2 -d ' ' | head -n 1
```

---

###  `2018-SE-03 (Зад. 8)`
**Task**:  
Among all user accounts in the system, the 201st account sorted by UID belongs to the SI specialty.  
Print a list of names (first and last) and home directories of all SI accounts, sorted by faculty number.

#### Example `/etc/passwd` entries:
```
s61988:x:1219:504:Stoian Genchev,SI,2,5:/home/SI/s61988:/bin/bash
s81430:x:1234:503:Iordan Petkov, KN, k2, g7:/home/KN/s81430:/bin/bash
s61807:x:1248:504:Elica Venchova:/home/SI/s61807:/bin/bash
s62009:x:1254:504:Denitsa Dobreva, 2, 6:/home/SI/s62009:/bin/bash
s61756:x:1258:504:Katrin Kartuleva, SI, 4, 1:/home/SI/s61756:/bin/bash
s855287:x:1195:504:Vaska Kichukova,SI,2,5:/home/SI/s855287:/bin/bash
```

#### Expected output:
```
Katrin Kartuleva:/home/SI/s61756
Elica Venchova:/home/SI/s61807
Stoian Genchev:/home/SI/s61988
Denitsa Dobreva:/home/SI/s62009
Vaska Kichukova:/home/SI/s855287
```

#### Solution (system):
```bash
grep --color "$(cat /etc/passwd | grep 'SI:/home' | sort -n -k 3 -t ':' | tail -n +201 | head -n 1 | cut -d ':' -f 4)" /etc/passwd | cut -d ':' -f 5,6 | sed 's/,,,,SI//g'
```

#### Solution (with file):
```bash
grep --color $(grep '/home/SI' example-passwd | head -n 201 | tail -n 1 | cut -d ':' -f 4) example-passwd | cut -d ':' -f 5,6 | sed 's/,.*:/:/g' | sort -n -k 4 -t '/'
```

---

###  `2019-SE-03 (Зад. 11)`
**Task**:  
From all files under `/home/velin`, find the one:
- that shares the same inode as the most recently modified file
- and has the **minimum directory depth**

> *Note: Depth is defined as the number of slashes (e.g. `/foo/bar/baz` has depth 3).*

```bash
find /home/velin -inum $(find /home/velin -printf '%i %T@\n' 2>/dev/null | sort -n -r -k 2 -t ' ' | head -n 1 | cut -d ' ' -f 1) -printf '%d\n' | sort -n | head -n 1
```

---

### `2023-CE-02 (Зад. 16)`
**Task**:  
You are given a directory `/var/log/my_logs` containing log files named in the format:
```
<server_name>_<unix_timestamp>.log
```

Where:
- `server_name` may include letters, digits, and underscores
- `unix_timestamp` is the number of seconds since 1970-01-01 00:00:00 UTC

Write a command to find the **total number of occurrences of the word `error`** in all valid log files.

#### Example log file name:
```
correlator_1692117813.log
```

#### Solution:
```bash
cat $(find my-logs -name "*log" | grep -E "^my-logs\/[0-9a-zA-Z]+_[0-9]+\.log$") | grep -o 'error' | wc -l
```

