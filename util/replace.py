#!/usr/bin/python3

import re
from os import listdir
from os import system
from os.path import isfile, join

checkre = re.compile("\.checked")

def ischecked(f):
    return '.checked.c' in f or '.checked.h' in f

checked = [f for f in listdir('.') if isfile(f) and ischecked(f)]

for f in checked:
    dest = re.sub(checkre, '', f)
    system("mv %s %s" % (f, dest))

print('Done')
