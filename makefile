# Alex St.Clair, tug89508, TUID: 915495534
# 3207 Project4: Virtual file system
# Lab Section 001

filesystem: 3207project4.c disk.c disk.h directory.c
	gcc -o filesystem 3207project4.c disk.c directory.c