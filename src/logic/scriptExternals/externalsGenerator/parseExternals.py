#!/usr/bin/env python3


def main():
    external_filename = "externals.d"
    with open(external_filename, "r", encoding="latin-1") as f:
        text = f.read()

    print(text)


main()
