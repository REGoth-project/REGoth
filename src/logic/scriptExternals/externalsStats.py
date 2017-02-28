#!/usr/bin/env python3

import re

def print_it(name, it):
    print("in {}:".format(name))
    print("\t" + "\n\t".join(sorted(it)))

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

    print("\n".join(sorted(stubs_list)))

main()
