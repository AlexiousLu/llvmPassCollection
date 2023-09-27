#ifndef LLVM_STRUCT_EXTRACTER_HH
#define LLVM_STRUCT_EXTRACTER_HH
#include <include_llvm.hh>
#include <iostream>
#include <fstream>
#include <json.hpp>

using json = nlohmann::ordered_json;
using namespace std;
using namespace llvm;

//extract all structures from modules, only the newest extract would be valid.
class StructExtracter {
private:
//  module_name: struct_name : field_name: {"type": xx, "otherinfo...."}
    pair<string, DIType*> recurDerefPointer(DIDerivedType* pointer_def);
    json* recurExtractType(DIType* struct_def, vector<string>& recursived_Ty_name);
public:
    StructExtracter() {}

    json* extractModule(Module* module);
    json* extractCallInst(CallInst* CI, vector<string> handled_Ty_name);
    json* extractModules(map<string, Module*>* modules);

    json* loadFrom(string jsonfile);

    bool saveTo(string jsonfile, json* data, unsigned int indent=4);

};


#endif