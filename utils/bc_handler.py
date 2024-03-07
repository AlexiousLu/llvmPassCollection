import os
import shutil
from multiprocessing.pool import Pool


def dis_assemble(bc_list:str = "/data/llvm-project/build_bc/bc.list"):
    p = Pool(30)
    with open(bc_list, "r") as f:
        for line in f.readlines():
            line = line.replace("\n", "")
            print(f"llvm-dis {line} -o {line.replace('.o.bc', '.o.ll')}")
            p.apply_async(os.system, (f"llvm-dis {line} -o {line.replace('.o.bc', '.o.ll')}", ))
    p.close()
    p.join()

def bc_join(bc_list: str, joint_path: str, replace_header: str):
    os.makedirs(joint_path, exist_ok=True)
    with open(bc_list, "r") as f:
        for line in f.readlines():
            line = line.replace("\n", "")
            root, file = os.path.split(line)
            if replace_header in root:
                root = root.replace(replace_header, joint_path + "/", 1)
            else:
                raise
            os.makedirs(root, exist_ok=True)
            print(f"copy {line} {os.path.join(root,file)}")
            shutil.copy(line, os.path.join(root, file))

if __name__ == "__main__":
    bc_join("/data/mongo/bc.list", "/data/mongo/mongo_bc", "/data/mongo/build")
