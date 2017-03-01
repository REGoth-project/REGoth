#!/usr/bin/env python3

import re
import operator

def parse_functions(external_filename):
    with open(external_filename, "r") as f:
        lines = f.readlines()

    known_types = ["C_Item", "C_Npc", "int", "float", "string", "func", "instance"]
    known_types_lower = list(map(str.lower, known_types))

    functions = []

    # optional body: \s*{(?P<statement>.*?)}\s*;
    comments_ended = True
    for line in lines:
        line = line.strip()
        match = re.search("func\s*(?P<returntype>\w*?)\s*(?P<funcname>\w*?)\s*\((?P<paramters>.*?)\)", line, re.I)
        if line == "":
            comments_ended = True
            continue
        elif not match:
            if not comments_ended:
                comment = line
                if not comment.startswith("//"):
                    comment = "// " + comment
                functions[-1][-1].append(comment)
            else:
                pass
                #print("throw away line: " + line)
            continue
        # -> match
        comments_ended = False
        func_name = match.group("funcname")
        returntype = match.group("returntype").lower()
        parameters = match.group("paramters").strip()
        assert returntype in ["void"] + known_types_lower
        params = []
        for param_info in parameters.split(","):
            if not param_info:
                #skip empty string arg: functions with no args
                continue
            param_info = param_info.split()
            if len(param_info) == 3:
                assert param_info[0].lower() == "var"
                param_info.pop(0)
            assert len(param_info) == 2
            param_type, param_name = param_info
            param_type = param_type.lower()
            assert param_type in known_types_lower
            params.append((param_type, param_name))
        comments = []
        functions.append((returntype, func_name, params, comments))
    d = dict()
    for returntype, func_name, params, comments in functions:
        assert func_name.lower() not in d, "duplicate function: " + func_name
        d[func_name.lower()] = (returntype, func_name, params, comments)
    return d

def format_function(f_info, print_comments=True):
    returntype, func_name, params, comments = f_info
    params_formatted = ", ".join("{} {}".format(*t) for t in params)
    signature = "func {returntype} {func_name}({params_formatted});".format(**locals())
    lines = [signature]
    if print_comments:
        lines.extend(comments)
    return "\n".join(lines) + "\n"


def main():
    modkit_fn = "modkit_externals.d"
    wog_fn = "wog2_externals.d"
    modkit_fs = parse_functions(modkit_fn)
    wog_fs = parse_functions(wog_fn)
    
    sortedk = sorted(wog_fs.keys())
    for fname in sortedk:
        print(format_function(wog_fs[fname]))

main()
