#! /usr/bin/bash
gcc -o shell shell.c 
./shell << EOF
ls
ls | wc -l

EOF
echo "
Commands executed 
1. ls
2. ls | wc -l
3.  
"
