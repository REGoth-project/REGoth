#!/usr/bin/env python3

import re
import operator
import collections
import copy

int_alias = ["bool", "C_Item_ID", "C_Npc_ID", "C_Info_ID"]
cinstance_types = ["C_Item", "C_Npc", "C_Info", "instance"]
primitives = ["int", "float", "string", "func"] + int_alias
known_types = primitives + cinstance_types
known_types_lower = list(map(str.lower, all_types))

class DaedalusFunction(object):
    def __init__(self, returntype, func_name, params, comments):
        self.returntype = returntype
        self.func_name = func_name
        self.params = params
        self.comments = comments

    def pretty(self, print_comments=False, print_varkeyword=False, print_funckeyword=True):
        var_keyword = "var " if print_varkeyword else ""
        params_formatted = ", ".join("{}{} {}".format(var_keyword, ptype, pname) for ptype, pname in self.params)
        funckeyword = "func " if print_funckeyword else ""
        signature = funckeyword + "{} {}({});".format(self.returntype, self.func_name, params_formatted)
        lines = [signature]
        if print_comments:
            lines.extend(self.comments)
        return "\n".join(lines)

    def register_format(self, indent=0):
        lines = []
        blines = []
        lines.extend(["// " + self.pretty(print_funckeyword=False)] + self.comments)
        register = "vm->registerExternalFunction(\"{fname}\", [=](Daedalus::DaedalusVM& vm) {{".format(fname=self.func_name)
        lines.append(register)
        verbose_info = "if (verbose) LogInfo() << \"{fname}\";".format(fname=self.func_name)
        blines.append(verbose_info)
        for param_type, param_name in reversed(self.params):
            if param_type == "string":
                blines.append("std::string " + param_name + ' = vm.popString();')
            elif param_type == "int":
                blines.append("int32_t " + param_name + " = vm.popDataValue();")
            elif param_type == "float":
                blines.append("float " + param_name + " = vm.popFloatValue();")
            elif param_type in ["c_npc", "c_item", "instance"]:
                blines.append("int32_t " + param_name + " = vm.popVar();")
            elif param_type == "bool":
                blines.append("bool " + param_name + " = static_cast<bool>(vm.popDataValue());")
            elif param_type == "func":
                blines.append("uint32_t " + param_name + " = static_cast<uint32_t>(vm.popVar());")
            else:
                assert False, "unknown param type: " + param_type
            blines[-1] += ' LogInfo() << "' + param_name + ': " << ' + param_name + ';'

        if self.returntype == "void":
            default_return = ""
        elif self.returntype in ["int", "bool"]:
            default_return = "vm.setReturn(0);"
        elif self.returntype == "string":
            default_return = 'vm.setReturn("");'
        elif self.returntype == "float":
            default_return = "vm.setReturn(0.0f);"
        elif self.returntype in ["c_npc", "c_item", "instance"]:
            default_return = "vm.setReturnVar(0);"
        else:
            assert False, "unknown return type: " + self.returntype

        c_npcs_indices = [i for i, (pt, pn) in enumerate(self.params) if pt == "c_npc"]

        smart_args = copy.copy(self.params)
        for i in c_npcs_indices:
            n_npc = self.params[i][1]
            vob_name = n_npc + "Vob"
            smart_args[i] = ("VobTypes::NpcVobInformation", vob_name)
            blines.append("VobTypes::NpcVobInformation {} = getNPCByInstance({});".format(vob_name, n_npc))

        if len(c_npcs_indices) != 0:
            condition = " || ".join("!{}.isValid()".format(smart_args[i][1]) for i in c_npcs_indices)
            blines.append("if ({})".format(condition))
            blines.append("{")
            if default_return != "":
                blines.append("\t" + default_return)
            blines.append("\t" + "return;")
            blines.append("}")

        args = ", ".join(pname for ptype, pname in smart_args)
        callobject = "Externals::{}({})".format(self.func_name, args)
        if default_return == "":
            blines.append(callobject + ";")
        else:
            return_fu = "setReturnVar" if self.returntype in ["c_npc", "c_item", "instance"] else "setReturn"
            blines.append("vm.{}({});".format(return_fu, callobject))

        lines.extend("\t" + line for line in blines)
        end_function = "});"
        lines.append(end_function)
        return "".join("\t" * indent + line + "\n" for line in lines)

 

def parse_functions(external_filename):
    with open(external_filename, "r") as f:
        lines = f.readlines()

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
        elif match:
            comments_ended = False
            func_name = match.group("funcname")
            returntype = match.group("returntype").lower()
            parameters = match.group("paramters").strip()
            assert returntype in ["void"] + known_types_lower, "unknown return type: " + returntype
            params = []
            param_idents = list(filter(None, map(str.strip, parameters.split(","))))
            for param_info in param_idents:
                param_info = param_info.strip()
                param_info = list(map(str.strip, param_info.split()))
                if len(param_info) == 3:
                    assert param_info[0].lower() == "var", "expected var, got: " + param_info[0].lower()
                    param_info.pop(0)
                assert len(param_info) == 2, "expected two identifiers: [var] <paramType> <paramName>, got: " + str(param_info)
                param_type, param_name = param_info
                param_type = param_type.lower()
                assert param_type in known_types_lower, "unknown param type: " + param_type
                params.append([param_type, param_name])
            param_names = [pname for _, pname in params]
            for pname, freq in collections.Counter(param_names).items():
                assert freq == 1, "multiple parameters with same name: " + pname + " in function: " + func_name
            comments = []
            functions.append(DaedalusFunction(returntype, func_name, params, comments))
    d = dict()
    for f in functions:
        assert f.func_name.lower() not in d, "duplicate function: " + f.func_name
        d[f.func_name.lower()] = f
    return d


def main():
    filename = "externals.d"
    func_dict = parse_functions(filename)
    for f in func_dict.values():
        pass#print(f.pretty())

    default_category = "other"
    categories = ["doc", "npc", "hlp", "snd", "wld", "mis", "mdl", "ai"]

    category_dict = dict((cat, dict()) for cat in categories + [default_category])
    for fname, f, in func_dict.items():
        prefix = fname.split("_")[0].lower()
        category = prefix if prefix in categories else default_category
        category_dict[category][fname] = f


    for prefix in sorted(category_dict.keys()):
        fdict = category_dict[prefix]
        print("/*")
        print("{} ({})".format(prefix, len(fdict)))
        print("*/")
        for fname in sorted(fdict.keys()):
            f = fdict[fname]
            print(f.register_format(indent=1).replace("\t", 4 * " "))

    #print("count: {}".format(len(func_dict)))

main()
