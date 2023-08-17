#! /usr/bin/bash
gcc -o shell shell.c 
./shell << EOF
ls
ls | wc -l

cat shell.c
cat shell.c | wc -l
echo I love OS
ls w3wef wefgwefwef wefwqefwef ewfewf
ls -l 
dfbrrebetbetb wefwe
history ergrger 
cd New_Dir
history 
ls 
cd ..
sleep 5 
sleep rg3fg4w wrgreg 
history 10
grep fork shell.c | wc 
EOF
echo "
Commands executed 
1. ls
2. ls | wc -l
3. new line character 
4. cat shell.c
5. cat shell.c | wc -l
6. echo I love OS 
7. ls with wrong number and type of arguments
8. ls -l
9. random input no command
10. history with wrong arguments
11. cd New_Dir
12. history 
13. ls
14. cd ..
15. sleep 5
16. sleep with wrong arguments
17. history 10
18. grep fork shell.c | wc
"
