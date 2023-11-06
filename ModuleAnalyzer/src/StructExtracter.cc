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




variant<MemberNode, StructNode> StructExtracter::recurExtractType(DIType* Ty_def, vector<string>& recursived_Ty_name, ModuleNode* curr_module_node){
    // for each field (DIDerivedType), check whether it's another struct_def (DICompositeType), if so, recurse it to add a new node under module_node.
    // DIType has 5 subclass: DIBasicType, DICompositeType, DIDerivedType, DIStringType, DISubroutineType. handle them one by one.

    switch(Ty_def->getMetadataID()){
        default:
            throw;
        case Metadata::DIBasicTypeKind: // int, float, etc...
        {
            MemberNode res;
            res.setBaseType(true);
            res.setTypeStr(Ty_def->getName().str()); 
            return res; // name should be set in DW_TAG_member-step
        }
        case Metadata::DIDerivedTypeKind: // pointer, member, etc... 
        {
            DIDerivedType* DerTy = dyn_cast<DIDerivedType>(Ty_def);
            switch(DerTy->getTag()){
                default:
                    throw;
                case dwarf::DW_TAG_member:
                {
                    auto node = this->recurExtractType(DerTy->getBaseType(), recursived_Ty_name, curr_module_node);
                    MemberNode res; 
                    if(StructNode* sn = get_if<StructNode>(&node)){ // member is a in-place, non-pointer struct and return a struct node.
                        res.setDerived(true);
                        res.setTypeStr(sn->getStructName());
                    } else if (MemberNode* mn = get_if<MemberNode>(&node)){ // member is a base type or a pointer;
                        res = *mn;
                    } else {
                        throw;
                    }
                    res.setMemberName(DerTy->getName().str());
                    res.setSize(DerTy->getSizeInBits());
                    res.setOffset(DerTy->getOffsetInBits());
                    return res;
                }
                case dwarf::DW_TAG_pointer_type: // "member_name" is set in DW_TAG_member case;
                {
                    MemberNode res;
                    pair<string, DIType*> ptr_res = this->recurDerefPointer(DerTy);
                    res.setTypeStr(ptr_res.first);
                    if(ptr_res.second->getMetadataID() == Metadata::DIBasicTypeKind){
                        res.setBaseType(true);
                        return res;
                    } else { // point to a composite type
                        string deref_type_str = ptr_res.first; // after c++11 it's a "deepcopy"(alloc and copy)
                        deref_type_str.erase(remove(deref_type_str.begin(), deref_type_str.end(), '*'), deref_type_str.end()); // standard usage to eliminate the last '*'
                        StructNode recur_struct_node = get<StructNode>(this->recurExtractType(ptr_res.second, recursived_Ty_name, curr_module_node));
                        recur_struct_node.setParent(curr_module_node);
                        (*curr_module_node)[deref_type_str] = recur_struct_node;
                        
                        res.setDerived(true);
                        res.setDerivedPointer(&((*curr_module_node)[deref_type_str]));
                        return res;
                    }
                }
            }
        }
        case Metadata::DICompositeTypeKind: // struct, array, etc...
        {
            DICompositeType* ComTy = dyn_cast<DICompositeType>(Ty_def);
            string Ty_name = ComTy->getName().str();
            switch(ComTy->getTag()){
                default:
                    throw;
                case dwarf::DW_TAG_structure_type:
                    StructNode res;
                    res.setStructName(Ty_name);
                    for(auto element : ComTy->getElements()){
                        if(DIType* eleTy = dyn_cast<DIType>(element)){
                            variant<MemberNode, StructNode> ele_node = this->recurExtractType(eleTy, recursived_Ty_name, curr_module_node);
                            
                            if(MemberNode* member_node = get_if<MemberNode>(&ele_node)){
                                member_node->setParent(&res);
                                res[member_node->getMemberName()] = *member_node;
                            } else {
                                throw; // must return a membernode;
                            }
                        } else {
                            throw; // unexpected situation.
                        }
                    }
                    return res;
            }
        }
    }
}

optional<StructNode> StructExtracter::extractVariableDeclear(CallInst* variable_declear, vector<string>& handled_Ty_name, ModuleNode* curr_module_node){
    if(!(variable_declear->getCalledFunction()->isIntrinsic()) || 
         variable_declear->getCalledFunction()->getName() != "llvm.dbg.declare"){ //directly getName cause segfault
        return nullopt;
    }

    StructNode res;
    for(auto arg = variable_declear->arg_begin(); arg != variable_declear->arg_end(); arg++){
        if(MetadataAsValue* mav = dyn_cast<MetadataAsValue>(arg)){
            // giving type of each variable declear (global && local) into recurExtractStruct.
            if(DILocalVariable* local_var = dyn_cast<DILocalVariable>(mav->getMetadata())){
                DIType* base_Ty = local_var->getType();
                if(base_Ty->getTag() == dwarf::DW_TAG_pointer_type){
                    base_Ty = this->recurDerefPointer(dyn_cast<DIDerivedType>(base_Ty)).second;
                }
                if(base_Ty->getTag() == dwarf::DW_TAG_structure_type){
                    string struct_name = base_Ty->getName().str();
                    res = get<StructNode>(this->recurExtractType(base_Ty, handled_Ty_name, curr_module_node)); // must return a value
                    return res;
                } else {
                    return nullopt; // FIXME: not only handle with struct.
                }
            } else if (DIGlobalVariable* global_var = dyn_cast<DIGlobalVariable>(mav->getMetadata())) {
                ;;;;;;
            }
        }
    }
    return res;
}

ModuleNode StructExtracter::extractModule(Module* module){
    ModuleNode res;
    // using filename in compile Unit as module name.
    
    if(module->getNamedMetadata("llvm.dbg.cu")->getNumOperands() != 1){
        throw;
    }
    if(DICompileUnit* CU = dyn_cast<DICompileUnit>(module->getNamedMetadata("llvm.dbg.cu")->getOperand(0))){
        string module_name = CU->getFile()->getFilename().str();
        res.setModuleName(module_name);
    } else {
        throw;
    };
    vector<string> handled_Ty_name; // same_struct from same module will be extracted only once. TODO: maybe could be deleted
    for(Module::iterator f = module->begin(), fe = module->end(); f != fe; f++){
        for (inst_iterator i = inst_begin(*f), ie = inst_end(*f); i != ie; ++i){
            if(CallInst* CI = dyn_cast<CallInst>(&*i)){
                if(CI->getCalledFunction() != nullptr){ // idk why calledfFunction could be nullptr
                    if(optional<StructNode> temp_res = this->extractVariableDeclear(CI, handled_Ty_name, &res)){ // nullptr means CI is irrelevant to struct definition
                        StructNode sn = temp_res.value();
                        sn.setParent(&res);
                        res[temp_res->getStructName()] = sn;
                    }
                }
            }
        }
    }
    return res;
}

RootNode* StructExtracter::extractModules(map<string, Module*>* modules){
    for(auto pair : *modules){
        if(pair.second == nullptr){
            cerr << "Cannot got module for " << pair.first;
        } else {
            cout << "Extracting " << pair.first << endl;
            ModuleNode mn = this->extractModule(pair.second);
            mn.setParent(&this->root);
            this->root[pair.first] = mn;
        }
    }
    return &this->root;
}

RootNode StructExtracter::loadFrom(string jsonfile){
    // json * res = new json();
    // ifstream inf(jsonfile);
    // if(!inf.fail()){
    //     *res = json::parse(inf);
    //     return res;
    // } else {
    //     return nullptr;
    // }
    return *new RootNode(); // TODO: pending.
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
