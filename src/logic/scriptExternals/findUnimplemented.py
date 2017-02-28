#!/usr/bin/env python3


import re

def find_externals(filename):
    with open(filename, "r") as f:
        text = f.read()
    l = []
    for match in re.finditer("registerExternalFunction\(\"(?P<name>.*?)\"", text):
        l.append(match.group("name"))
    return l


def main():
    stubs = find_externals("Stubs.cpp")
    implemented = find_externals("Externals.cpp")
    implemented, stubs = stubs, implemented
    for s in stubs:
        if s not in implemented:
            print(s)


main()
