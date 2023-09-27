#include <StructExtracter.hh>

pair<string, DIType*> StructExtracter::recurDerefPointer(DIDerivedType* pointer_def){
    // struct_abc ****a -> "struct_abc****", struct_abc(non-pointer type) 
    assert((pointer_def->getTag() == dwarf::DW_TAG_pointer_type));
    DIType * base_Ty = pointer_def->getBaseType();
    if(base_Ty->getTag() == dwarf::DW_TAG_pointer_type){
        DIDerivedType* base_ptr_def = dyn_cast<DIDerivedType>(base_Ty);
        return pair<string, DIType*>(this->recurDerefPointer(base_ptr_def).first + "*", this->recurDerefPointer(base_ptr_def).second);
    } else {
        return pair<string, DIType*>(base_Ty->getName().str() + "*", base_Ty);
    }
}




json* StructExtracter::recurExtractType(DIType* Ty_def, vector<string>& recursived_Ty_name){
    // for each field (DIDerivedType), check whether it's another struct_def (DICompositeType), if so, recurse it.
    // DIType has 5 subclass: DIBasicType, DICompositeType, DIDerivedType, DIStringType, DISubroutineType. handle them one by one.

    json* res = new json();
    switch(Ty_def->getMetadataID()){
        default:
            throw;
        case Metadata::DIBasicTypeKind: // int, float, etc...
        {
            (*res)["type"] = Ty_def->getName().str();
            return res;
        }
        case Metadata::DIDerivedTypeKind: // pointer, member, etc... 
        {
            DIDerivedType* DerTy = dyn_cast<DIDerivedType>(Ty_def);
            switch(DerTy->getTag()){
                default:
                    throw;
                case dwarf::DW_TAG_member:
                {
                    json* temp_res = new json();
                    if(json* base_Ty_res = this->recurExtractType(DerTy->getBaseType(), recursived_Ty_name)){
                        (*temp_res)[DerTy->getName().str()] = *base_Ty_res;
                    } else {
                        throw; // base type of a member should always return non-nullptr
                    }
                    return temp_res;
                }
                case dwarf::DW_TAG_pointer_type:
                {
                    pair<string, DIType*> ptr_res = this->recurDerefPointer(DerTy);
                    string type_str = ptr_res.first;
                    DIType* deref_type = ptr_res.second; //TODO: using a new task to handle the base type parsing, shouldn't deref it as a subfield.
                    (*res)["type"] = type_str;
                    return res;
                }
            }
            return nullptr;
        }
        case Metadata::DICompositeTypeKind: // struct, array, etc...
        {
            DICompositeType* ComTy = dyn_cast<DICompositeType>(Ty_def);
            string Ty_name = ComTy->getName().str();
            switch(ComTy->getTag()){
                default:
                    throw;
                case dwarf::DW_TAG_structure_type:
                    json* temp_res = new json();
                    for(auto element : ComTy->getElements()){
                        if(DIType* eleTy = dyn_cast<DIType>(element)){
                            if(json* inner_res = this->recurExtractType(eleTy, recursived_Ty_name)){
                                temp_res->merge_patch(*inner_res);
                            }
                        } else {
                            throw; // unexpected situation.
                        }
                    }
                    return temp_res;
            }
        }
    }
}

json* StructExtracter::extractCallInst(CallInst* CI, vector<string> handled_Ty_name){
    if(!(CI->getCalledFunction()->isIntrinsic()) || CI->getCalledFunction()->getName() != "llvm.dbg.declare"){ //directly getName cause segfault
        return nullptr;
    }

    json* res = new json();
    for(auto arg = CI->arg_begin(); arg != CI->arg_end(); arg++){
        if(MetadataAsValue* mav = dyn_cast<MetadataAsValue>(arg)){
            // giving type of each variable declear (global && local) into recurExtractStruct.


            if(DILocalVariable* local_var = dyn_cast<DILocalVariable>(mav->getMetadata())){
                DIType* base_Ty = local_var->getType();
                if(base_Ty->getTag() == dwarf::DW_TAG_pointer_type){
                    base_Ty = this->recurDerefPointer(dyn_cast<DIDerivedType>(base_Ty)).second;
                }
                if(json* temp_res = this->recurExtractType(base_Ty, *new vector<string>())){
                    (*res)[base_Ty->getName().str()] = *temp_res;
                }; // FIXME: potential duplication of name.
            } else if (DIGlobalVariable* global_var = dyn_cast<DIGlobalVariable>(mav->getMetadata())) {
                ;;;;;;
            }
        }
    }
    return res;
}

json* StructExtracter::extractModule(Module* module){
    json* res = new json(); 
    vector<string> handled_Ty_name; // same_struct from same module will be extracted only once.
    for(Module::iterator f = module->begin(), fe = module->end(); f != fe; f++){
        for (inst_iterator i = inst_begin(*f), ie = inst_end(*f); i != ie; ++i){
            if(CallInst* CI = dyn_cast<CallInst>(&*i)){
                if(CI->getCalledFunction() != nullptr){ // idk why calledfFunction could be nullptr
                    if(json* temp_res = this->extractCallInst(CI, handled_Ty_name)){ // nullptr means to irrelevant CI
                        res->merge_patch(*temp_res);
                    }
                }
            }
        }
    }
    return res;
}

json* StructExtracter::extractModules(map<string, Module*>* modules){
    cout << "start" << endl;
    json* res = new json();
    for(auto pair : *modules){
        if(pair.second == nullptr){
            cerr << "Cannot got module for " << pair.first;
        } else {
            (*res)[pair.first] = *(this->extractModule(pair.second));
        }
    }
    return res;
}

json* StructExtracter::loadFrom(string jsonfile){
    json * res = new json();
    ifstream inf(jsonfile);
    if(!inf.fail()){
        *res = json::parse(inf);
        return res;
    } else {
        return nullptr;
    }
}

bool StructExtracter::saveTo(string jsonfile, json* data, unsigned int indent){
    string j_str = data->dump(indent);
    ofstream of(jsonfile);
    if(!of.fail()){
        of << j_str << endl;
        return true;
    } else {
        return false;
    }
}
