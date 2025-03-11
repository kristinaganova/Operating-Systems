# Linux Tasks

## Task 02-a-4000
### Set permissions for `permissions.txt`
Create a file `permissions.txt` in your home directory and set the following permissions:
- **User:** Read-only (`r`)
- **Group:** Write and execute (`wx`)
- **Others:** Read and execute (`rx`)

#### Commands:
```bash
chmod 435 permissions.txt
chmod u=r,g=wx,o=rx permissions.txt
```

---
## Task 02-a-4100
### Find files modified in the last 1 hour
Find all files in your home directory that have been modified in the last 60 minutes.
#### Command:
```bash
find ~ -mmin -60
```

---
## Task 02-a-5000
### Copy and read `/etc/services`
Copy `/etc/services` to your home directory and read it using `cat`.
#### Commands:
```bash
cp /etc/services ~/my_services
cat ~/my_services
```

---
## Task 02-a-5400
### Find regular files in `/etc` and its direct subdirectories
#### Command:
```bash
find /etc -type f -mindepth 1 -maxdepth 2
```

---
## Task 02-a-5401
### Display the first 5 lines of `/etc/services`
#### Command:
```bash
head -n 5 /etc/services
```

---
## Task 02-a-5402
### Find files at depth 2 in `/etc`
#### Command:
```bash
find /etc -type f -mindepth 2 -maxdepth 2
```

---
## Task 02-a-5403
### List direct subdirectories of `/etc`
#### Command:
```bash
find /etc -type d -mindepth 1 -maxdepth 1
```

---
## Task 02-a-5500
### Save the last 10 lines of a command output to a file
#### Commands:
```bash
touch text.txt
find /etc -type d -mindepth 1 -maxdepth 1 | tail > text.txt
cat text.txt
```

---
## Task 02-a-5501
### Find regular files larger than 42 bytes in home directory
#### Command:
```bash
find ~ -type f -size +42c
```

---
## Task 02-a-5504
### Find files in `/tamp` owned by your group, that have write permissions for the group or for others(o=w)
#### Command:
```bash
find /tmp -group $(id -g) '(' -perm -g=w ')' -or \( -perm -o=w \)
```

---
## Task 02-a-5505
### Find files newer than `practice/01/f1`
#### Command:
```bash
find -type f -newer practice/01/f1
```

---
## Task 02-a-5506
### Delete files newer than `practice/01/f1`
#### Command:
```bash
find ~ -type f -newer practice/01/f1 -exec rm -i {} \;
```

---
## Task 02-a-6000
### Find files in `/bin` that are readable, writable, and executable by all
#### Command:
```bash
find /bin -perm 777
```

---
## Task 02-a-8000
### Copy all world-readable files from `/etc` to `~/myetc`
#### Commands:
```bash
mkdir -p ~/myetc
cp -r $(find /etc -perm /o+r,g+r,u+r) ~/myetc
```
**or**
```bash
cp -r $(find /etc -perm -444) ~/myetc

---
## Task 02-a-9500
### Using find, print the lines count in every regular file in /etc
#### Commands:
```bash
find /etc -type f -exec wc -l {} \;
```

