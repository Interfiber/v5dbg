import io

"""
stub.py generates stub versions of macros so you can easily disable the debugger with V5DBG_DISABLE
"""

def gen_macro(params: list[str]) -> str:
    macro_name = params[0].strip()
    macro_params = params[1:len(params)] # Collect rest of the macro params

    print(f"> stub macro: {macro_name}")
    print(f"> stub macro arguments: {macro_params}")

    result = f"\n/// Stubbed version of normal {macro_name}, automatically generated\n#define {macro_name}({','.join(macro_params).strip()})"
    result = result.replace("()", "") # Replace empty parenthesis with nothing as they cause compiler errors

    return result

def gen_stub(stub_str: str) -> str:
    result = ""

    stub_str = stub_str.replace("// stub:", "")

    if stub_str.startswith("macro="):
        stub_str = stub_str.replace("macro=", "")
        result = gen_macro(stub_str.split(","))
    else:
        print("gen_stub found invalid expression")

    return result

def scan_file(file: io.FileIO):
    # Load all lines from this file
    lines = file.readlines()

    file_contents_new = "#pragma once"

    for line in lines:
        if line.startswith("// stub:"):
            # Handle macro stubbing

            file_contents_new += gen_stub(line)
    
    # File must have an extension
    file_split = file.name.split(".")
    ext = file_split[1]

    new_file = file_split[0] + ".stub." + ext

    print(f"> stub write to: {new_file}")

    new_file_io = open(new_file, "w")
    new_file_io.write(file_contents_new)
    new_file_io.close()

stubbed_files = ["debug.h", "pretty.h"]

for file in stubbed_files:
    scan_file(open(f"include/v5dbg/{file}", "r"))