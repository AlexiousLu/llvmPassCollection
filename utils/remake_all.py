import os
import shutil
import sys


Module_dirs = [
    "ModuleLoader",
    "DatabaseManager",
    "AliasAnalyzer",
    "ModuleAnalyzer",
    "StructAnalyzer",
] # Dependency-Ordered

Log_path = "logs/remake.log"

def remake_module(module_name: str, cmake_command: str="-DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_VERBOSE_MAKEFILE=ON"):
    os.makedirs(f"{module_name}/build", exist_ok=True)
    os.system(f"cd {module_name}/build; cmake .. {cmake_command} >> ../../{Log_path} 2>&1; make -j24 >> ../../{Log_path} 2>&1")

def remake_modules():
    os.remove(Log_path)
    for d in Module_dirs:
        remake_module(d)

def clean_module(module_name: str):
    shutil.rmtree(f"{module_name}/build")
    os.makedirs(f"{module_name}/build")


def clean_modules():
    for d in Module_dirs:
        clean_module(d)

if __name__ == "__main__":
    if len(sys.argv) == 2:
        if sys.argv[1] == "clean":
            clean_modules()
    else:
        remake_modules()
