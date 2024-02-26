import os
from multiprocessing.pool import Pool
bc_list = "./Data/linux_5.18bcs_noinline/bc.list"

if __name__ == "__main__":
    p = Pool(30)
    with open(bc_list, "r") as f:
        for line in f.readlines():
            line = line.replace("\n", "")
            print(f"llvm-dis {line} -o {line.replace('.o.bc', '.o.ll')}")
            p.apply_async(os.system, (f"llvm-dis {line} -o {line.replace('.o.bc', '.o.ll')}", ))
    p.close()
    p.join()
            
