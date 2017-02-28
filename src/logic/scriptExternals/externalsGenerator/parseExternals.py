#!/usr/bin/env python3


import re
import operator

def main():
    external_filename = "externals.d"
    with open(external_filename, "r", encoding="latin-1") as f:
        text = f.read().lower()

    known_types = ["C_Item", "C_NPC", "int", "float", "string", "func"]
    known_types = list(map(str.lower, known_types))

    functions = []

    # optional body: \s*{(?P<statement>.*?)}\s*;
    for match in re.finditer("func\s*(?P<returntype>\w*?)\s*(?P<funcname>\w*?)\s*\((?P<paramters>.*?)\)", text, re.I):
        func_name = match.group("funcname")
        returntype = match.group("returntype")
        parameters = match.group("paramters").strip()
        assert returntype in known_types + ["void"]
        params = []
        for param_info in parameters.split(","):
            if not param_info:
                continue
            var_token, param_type, param_name = param_info.split()
            assert var_token == "var"
            assert param_type in known_types
            params.append((param_type, param_name))
        functions.append((returntype, func_name, params))

    functions = sorted(functions, key=operator.itemgetter(1))
    for func in functions:
        returntype, func_name, params = func
        params_formatted = ", ".join("{} {}".format(*t) for t in params)
        #print("{returntype} {func_name}({params_formatted});".format(**locals()))
        print(func_name)


main()
