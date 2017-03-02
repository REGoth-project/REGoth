#!/usr/bin/env python3

import re
import operator

class DaedalusFunction(object):
    def __init__(self, returntype, func_name, params, comments):
        self.returntype = returntype
        self.func_name = func_name
        self.params = params
        self.comments = comments

    def __str__(self, print_comments=True):
        params_formatted = ", ".join("{} {}".format(*t) for t in self.params)
        signature = "func {} {}({});".format(self.returntype, self.func_name, params_formatted)
        lines = [signature]
        if print_comments:
            lines.extend(self.comments)
        return "\n".join(lines) + "\n"


def parse_functions(external_filename):
    with open(external_filename, "r") as f:
        lines = f.readlines()

    known_types = ["C_Item", "C_Npc", "int", "bool", "float", "string", "func", "instance"]
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
                functions[-1].comments.append(comment)
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
            params.append([param_type, param_name])
        comments = []
        functions.append(DaedalusFunction(returntype, func_name, params, comments))
    d = dict()
    for f in functions:
        assert f.func_name.lower() not in d, "duplicate function: " + f.func_name
        d[f.func_name.lower()] = f
    return d

def upperCount(s):
    return sum(1 for c in s if c.isupper())

def decapitalize(s):
    return s[0].lower() + s[1:]

def main():
    modkit_fn = "modkit_processed.d"
    wog_fn = "wog_processed.d"
    modkit_fs = parse_functions(modkit_fn)
    wog_fs = parse_functions(wog_fn)

    # decapitalize param names
    for d in [modkit_fs, wog_fs]:
        for f in d.values():
            f.params = [[param_type, decapitalize(param_name)] for param_type, param_name in f.params]

    # select the camel case one
    for common in set(modkit_fs.keys()) & set(wog_fs.keys()):
        modkit_params = modkit_fs[common].params
        wog_params = wog_fs[common].params
        winners = []
        for i, (a, b) in enumerate(zip(modkit_params, wog_params)):
            if a[1].lower() == b[1].lower() and a[1] != b[1]:
                winner = b[1] if upperCount(a[1]) < upperCount(b[1]) else a[1]
                winners.append((i, winner))
        for i, param_name in winners:
            modkit_params[i][1] = param_name
            wog_params[i][1] = param_name

    # add missing entries to each other
    for a, b in [(modkit_fs, wog_fs), (wog_fs, modkit_fs)]:
        for k in a.keys():
            if k not in b:
                b[k] = a[k]

    # add missing comments to each other
    for fname in set(modkit_fs.keys()) & set(wog_fs.keys()):
        c1 = modkit_fs[fname].comments
        c2 = wog_fs[fname].comments
        if (len(c1) == 0) != (len(c2) == 0):
            comments = c1 + c2
            modkit_fs[fname].comments = comments
            wog_fs[fname].comments = comments


    for fname, fs in [("modkit_processed2.d", modkit_fs), ("wog_processed2.d", wog_fs)]:
        with open(fname, "w") as f:
            for fname in sorted(fs.keys()):
                f.write(str(fs[fname]) + "\n")

    print("modkit: {}".format(len(modkit_fs)))
    print("wog: {}".format(len(wog_fs)))

main()
