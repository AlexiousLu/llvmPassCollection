import sys
import os

LLVM_LIBRARYS_ARGS = "-lLLVMSupport -lLLVMCore -lLLVMAsmParser -lLLVMAnalysis -lLLVMIRReader"

if __name__ == "__main__":
    assert sys.argv.__len__() == 2, "Invaild argument"
    source_file = sys.argv[1]
    command = f"g++ {source_file} -o ./temp_out {LLVM_LIBRARYS_ARGS}"
    os.system(command)
    os.system(f"./temp_out")

    print(command)