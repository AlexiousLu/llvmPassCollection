import os

Exception_dir = [
    "Lib",
    "Model",
    "Data",
    "Common",
    "utils",
]

def remake_module(module_name: str, cmake_command: str="-DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_VERBOSE_MAKEFILE=OFF"):
    os.makedirs(f"{module_name}/build", exist_ok=True)
    os.system(f"cd {module_name}/build; cmake .. {cmake_command}; make -j4")

def remake_modules():
    for r, ds, fs in os.walk("."):
        for d in ds:
            if d not in Exception_dir and not d.startswith(".") and r == ".":
                remake_module(d)
                # print(f"{r} {d}")

if __name__ == "__main__":
    remake_modules()
