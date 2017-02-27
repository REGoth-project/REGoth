#!/usr/bin/env python3

import re

def get_externals(filename):
    with open(filename, "r") as f:
        text = f.read()
    l = []
    for match in re.finditer("registerExternalFunction\(\"(?P<funcname>.*?)\"", text):
        l.append(match.group("funcname").lower())
    return set(l)

def main():
    implemented = "Externals.cpp"
    stubs = "Stubs.cpp"
    impl_list = get_externals(implemented)
    stubs_list = get_externals(stubs)

    both = impl_list & stubs_list
    only_stub = stubs_list - both
    only_impl = impl_list - both
    print("in both:")
    print("\t" + "\n\t".join(sorted(both)))
    print("only in stub:")
    print("\t" + "\n\t".join(sorted(only_stub)))
    print("only in implemented:")
    print("\t" + "\n\t".join(sorted(only_impl)))

    print("stubs length: {}".format(len(stubs_list)))

main()
