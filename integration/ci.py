import sys
import os
import shutil

def exe(file: str):
    if os.name == 'nt':
        return file + '.exe'

    return file

def zip_artifact(name: str, dir: str):
    print(f'archive zip {name}.zip from {dir}')

    shutil.make_archive(f'{name}', 'zip', dir)

    shutil.rmtree(dir)

# Create a temporary directory
def tempdir(name: str) -> str:
    dir = f"/tmp/{name}-ci-temp"
    os.makedirs(dir)

    return dir

# Build the debug server
def build_server():
    server_dir = tempdir("v5dbg")
    print(f'server_dir = {server_dir}')

    print('copy include')
    shutil.copytree('include/v5dbg', f'{server_dir}/include/v5dbg')

    print('copy source')
    shutil.copytree('src/v5dbg', f'{server_dir}/src/v5dbg')

    shutil.copy('integration/README_server.txt', f'{server_dir}/README_SERVER.txt')

    zip_artifact('v5dbg_server', server_dir)


# Build the debugger client and v5dbg-server comms package
def build_client():
    client_dir_build = tempdir("debugger-build")
    client_dir = tempdir("debugger")

    print("build debugger dist")

    os.system(f"pyinstaller --noconfirm --onefile debugger/main.py --name v5dbg --console --workpath {client_dir_build} --distpath {client_dir}")

    print("clear debugger build")

    shutil.rmtree(client_dir_build)

    print("build comms dist")

    os.chdir("v5dbg-server")
    os.system("cargo build --release")

    shutil.copy(exe('target/release/v5dbg-server'), f'{client_dir}/v5dbg-server')

    os.chdir("../")

    zip_artifact("v5dbg_debugger", client_dir)

# Arguments
args = sys.argv

if len(args) != 2:
    raise Exception("No mode specified to CI build script")

mode = args[1]

print(f"CI run mode: {mode}")

if mode == "server":
    build_server()
elif mode == "client":
    build_client()
else:
    raise Exception(f"Invalid mode '{mode}'")