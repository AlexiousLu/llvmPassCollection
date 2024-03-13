import os

# Exception_dir = [
#     "Lib",
#     "Model",
#     "Data",
#     "Common",
#     "utils",
# ]

Module_dirs = [
    "ModuleLoader",
    "ModuleAnalyzer",
    "StructAnalyzer",
    "DatabaseManager",
] # Dependency-Ordered


def remake_module(module_name: str, cmake_command: str="-DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_VERBOSE_MAKEFILE=OFF"):
    os.makedirs(f"{module_name}/build", exist_ok=True)
    os.system(f"cd {module_name}/build; cmake .. {cmake_command}; make -j4")

def remake_modules():
    for d in Module_dirs:
        remake_module(d)

if __name__ == "__main__":
    remake_modules()
