#include <StructExtracter.hh>


/*
exType                <--|
  |                      |
  |-->  exDerivedType  --|
  |--> exCompositeType --|
  |-->   exBasicType   --|

*/


void StructExtracter::exDerivedType(DIDerivedType* derived_type, ExtractInfo& info) {
    switch (derived_type->getTag()) {
        default: throw; // todo
        case dwarf::DW_TAG_member: // member should only enter once each member
            if ((info.curr_member_node != nullptr && derived_type->getName().str() != "" &&
                 info.curr_member_node->getMemberName() == derived_type->getName().str()) 
              || info.curr_struct_node == nullptr) 
                throw;
            info.curr_member_node = new MemberNode(info.curr_struct_node);
            (*info.curr_struct_node)[derived_type->getName().str()] = info.curr_member_node;
            info.curr_member_node->setMemberName(derived_type->getName().str());
            info.curr_member_node->setSize(derived_type->getSizeInBits());
            info.curr_member_node->setOffset(derived_type->getOffsetInBits());
            info.curr_member_node->setDerivedStructName(derived_type->getScope()->getName().str());
            return exType(derived_type->getBaseType(), info); // finish typestr
        case dwarf::DW_TAG_pointer_type:
            if (info.curr_member_node != nullptr) {
                if (derived_type->getBaseType() == nullptr) {
                    info.curr_member_node->setTypeStr(info.curr_member_node->getTypeStr() + "void *");
                    return;
                } else {
                    info.curr_member_node->setTypeStr(info.curr_member_node->getTypeStr() + "*");
                }
            }
            if (derived_type->getBaseType() == nullptr) return;
            return exType(derived_type->getBaseType(), info);
        case dwarf::DW_TAG_const_type:
        case dwarf::DW_TAG_typedef:
        case dwarf::DW_TAG_volatile_type:
            if (derived_type->getBaseType() == nullptr) {
                if (info.curr_member_node != nullptr) 
                    info.curr_member_node->setTypeStr("null" + info.curr_member_node->getTypeStr());
                return;
            }
            return exType(derived_type->getBaseType(), info);
    }
}

void StructExtracter::exCompositeType(DICompositeType* composite_type, ExtractInfo& info) {
    switch (composite_type->getTag()) {
        default: throw;
        case dwarf::DW_TAG_structure_type:
        {
            if(info.curr_member_node != nullptr) { // sturct as end of a member recursive -> finish type str.
                info.curr_member_node->setTypeStr(
                    "struct " + composite_type->getName().str() + info.curr_member_node->getTypeStr()
                );
                info.curr_member_node->setDerivedStruct(true);
            }
            // add this struct to curr module (if there is a same struct, skip)
            if (info.curr_module_node->has(composite_type->getName().str())) {
                return;
            }
            ExtractInfo old_info = ExtractInfo(info);
            ExtractInfo new_info = ExtractInfo(info.curr_module_node);
            new_info.curr_struct_node = new StructNode(info.curr_module_node);
            new_info.curr_struct_node->setStructName(composite_type->getName().str());
            (*info.curr_module_node)[composite_type->getName().str()] = new_info.curr_struct_node;
            for (auto member : composite_type->getElements()) {
                if (DIDerivedType* mem_type = dyn_cast<DIDerivedType>(member)) {
                    exType(mem_type, new_info);
                } else {
                    throw;
                }
            }
            return;
        }
        case dwarf::DW_TAG_union_type: // treat union like struct (the union member as struct members) TODO: merge
        {
            if(info.curr_member_node != nullptr) { // union as end of a member recursive -> finish type str.
                info.curr_member_node->setTypeStr(
                    "union " + composite_type->getName().str() + info.curr_member_node->getTypeStr()
                );
                info.curr_member_node->setDerivedStruct(true);
            }

            // add union's member to curr module, skip if exists.
            if (info.curr_module_node->has(composite_type->getName().str())) {
                return;
            }
            ExtractInfo old_info = ExtractInfo(info);
            ExtractInfo new_info = ExtractInfo(info.curr_module_node);
            new_info.curr_struct_node = new StructNode(info.curr_module_node);
            new_info.curr_struct_node->setStructName(composite_type->getName().str());
            (*info.curr_module_node)[composite_type->getName().str()] = new_info.curr_struct_node;
            for (auto member : composite_type->getElements()) {
                if (DIDerivedType* mem_type = dyn_cast<DIDerivedType>(member)) {
                    exType(mem_type, new_info);
                } else {
                    throw;
                }
            }
            return;
        }
        case dwarf::DW_TAG_array_type:
        {
            if(info.curr_member_node != nullptr) { // array as end of a member recursive -> finish type str.
                string array_len = "";
                if(DISubrange* len = dyn_cast<DISubrange>(composite_type->getElements()[0])){
                    array_len = to_string(len->getCount().get<ConstantInt*>()->getSExtValue());
                };
                exType(composite_type->getBaseType(), info); // finish type_str
                info.curr_member_node->setTypeStr(info.curr_member_node->getTypeStr() + "[" + array_len + "]");
            } else { // var declearation outside a struct member;
                exType(composite_type->getBaseType(), info);
            }
            return;
        }
        case dwarf::DW_TAG_enumeration_type:
            exType(composite_type->getBaseType(), info); // skip enum name.
            return;

    }
}

void StructExtracter::exBasicType(DIBasicType* base_type, ExtractInfo& info) {
    if (info.curr_member_node != nullptr) {
        info.curr_member_node->setBaseType(true);
        info.curr_member_node->setTypeStr(base_type->getName().str() + info.curr_member_node->getTypeStr());
    }
    return;
}


void StructExtracter::exType(DIType* type, ExtractInfo& info){
    switch (type->getMetadataID()) {
        default: throw;
        case Metadata::DIDerivedTypeKind:
        {
            DIDerivedType* derived_type = dyn_cast<DIDerivedType>(type);
            exDerivedType(derived_type, info);
            return;
        }
        case Metadata::DICompositeTypeKind:
        {
            DICompositeType* composite_type = dyn_cast<DICompositeType>(type);
            exCompositeType(composite_type, info);
            return;
        }
        case Metadata::DIBasicTypeKind:
        {
            DIBasicType* base_type = dyn_cast<DIBasicType>(type);
            exBasicType(base_type, info);
            return;
        }
        case Metadata::DISubroutineTypeKind: // function pointer -> treat as base type 
        {
            if (info.curr_member_node != nullptr) {
                info.curr_member_node->setBaseType(true);
                info.curr_member_node->setTypeStr("RESERVED: function pointer" + info.curr_member_node->getTypeStr());
            }
            return;
        }
    }
}


optional<StructNode*> StructExtracter::exVarDeclear(CallInst* variable_declear, ModuleNode* curr_module_node){
    if(!(variable_declear->getCalledFunction()->isIntrinsic()) || 
         variable_declear->getCalledFunction()->getName() != "llvm.dbg.declare"){ //directly getName cause segfault
        return nullopt;
    }

    StructNode* res = new StructNode();
    ExtractInfo* info = new ExtractInfo(curr_module_node);
    for(auto arg = variable_declear->arg_begin(); arg != variable_declear->arg_end(); arg++) {
        if(MetadataAsValue* mav = dyn_cast<MetadataAsValue>(arg)){
            // giving type of each variable declear (global && local) into recurExtractStruct.
            if(DILocalVariable* local_var = dyn_cast<DILocalVariable>(mav->getMetadata())){
                local_var->printAsOperand(errs(), variable_declear->getModule()); errs() << "\n";
                DIType* var_type = local_var->getType();
                exType(var_type, *info);
                // if(var_type->getTag() == dwarf::DW_TAG_pointer_type){
                //     var_type = this->exDerivedType(dyn_cast<DIDerivedType>(var_type)).second;
                // }
                // if(var_type->getTag() == dwarf::DW_TAG_structure_type){
                //     string struct_name = var_type->getName().str();
                //     res = get<StructNode*>(this->exType(var_type, curr_module_node)); // must return a value
                //     return res;
                // } else {
                //     return nullopt; // FIXME: not only handle with struct.
                // }
            } else if (DIGlobalVariable* global_var = dyn_cast<DIGlobalVariable>(mav->getMetadata())) { // todo
                cout << "a global var found" << endl;
            }
        }
    }
    return res;
}


ModuleNode* StructExtracter::exModule(Module* module){
    ModuleNode* res = new ModuleNode();
    // using filename in compile Unit as module name.
    if (!module->getNamedMetadata("llvm.dbg.cu")) {
        cerr << "No dbg info in module: " << module->getName().str() << endl;
        return nullptr;
    }
    if(module->getNamedMetadata("llvm.dbg.cu")->getNumOperands() != 1){
        throw;
    }
    if(DICompileUnit* CU = dyn_cast<DICompileUnit>(module->getNamedMetadata("llvm.dbg.cu")->getOperand(0))){
        string module_name = CU->getFile()->getFilename().str();
        res->setModuleName(module_name);
    } else {
        throw;
    };
    for(Module::iterator f = module->begin(), fe = module->end(); f != fe; f++){
        for (inst_iterator i = inst_begin(*f), ie = inst_end(*f); i != ie; ++i){
            if(CallInst* CI = dyn_cast<CallInst>(&*i)){
                if(CI->getCalledFunction() != nullptr){ // idk why calledfFunction could be nullptr
                    this->exVarDeclear(CI, res);
                }
            }
        }
    }
    return res;
}


RootNode* StructExtracter::exModules(map<string, Module*>* modules) {
    for(auto pair : *modules){
        if(pair.second == nullptr){
            cerr << "Cannot got module for " << pair.first;
        } else {
            cout << "Extracting " << pair.first << endl;
            ModuleNode* mn = this->exModule(pair.second);
            if (mn != nullptr) {
                mn->setParent(this->root);
                (*this->root)[pair.first] = mn;
            } else {
                continue;
            }
        }
    }
    return this->root;
}

RootNode* StructExtracter::loadFromSqlite3(string sqlite3_path) {
    if (!filesystem::exists(sqlite3_path)) {
        cerr << "sqlite does not exist: " << sqlite3_path << endl;
        return nullptr;
    }
    sqlite3 *db;
    int ret = sqlite3_open(sqlite3_path.c_str(),&db);
    if (ret != SQLITE_OK) {
		cerr << "open db" << sqlite3_path << "error!" << endl;
		return nullptr;
	}
    
    const string sql_str = "select module_name, struct_name, member_node_str from STRUCT_INFO";
    sqlite3_stmt *stmt = NULL;
    int result = sqlite3_prepare(db, sql_str.c_str(), -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        cerr << "Wrong sqlite3 statment: " << sql_str << endl;
        sqlite3_close(db);
        return nullptr;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char * module_name_cstr = sqlite3_column_text(stmt, 0);
        string module_name((char*)module_name_cstr);
        const unsigned char * struct_name_cstr = sqlite3_column_text(stmt, 1);
        string struct_name((char*)struct_name_cstr);
        const unsigned char * member_node_str_cstr = sqlite3_column_text(stmt, 2);
        string member_node_str((char*)member_node_str_cstr);

        json j_member_node = json::parse(member_node_str);
        ModuleNode* mn = new ModuleNode();
        StructNode* sn = new StructNode();
        MemberNode* memn = new MemberNode(j_member_node);

        auto modules = this->root->getChildren();
        if (modules.find(module_name) == modules.end()) {
            mn->setParent(this->root);
            (*this->root)[module_name] = mn;
        }
        auto structs = (*this->root)[module_name]->getChildren();
        if (structs.find(struct_name) == structs.end()) {
            sn->setParent((*this->root)[module_name]);
            (*(*this->root)[module_name])[struct_name] = sn;
        }
        memn->setParent((*(*this->root)[module_name])[struct_name]);
        (*(*(*this->root)[module_name])[struct_name])[memn->getMemberName()] = memn;    
    }
    sqlite3_finalize(stmt); sqlite3_close(db);
    return this->root;
}

bool StructExtracter::saveToSqlite3(string sqlite3_path, RootNode* root) {
    if (root == nullptr) root = this->root;

    if (filesystem::exists(sqlite3_path)) filesystem::remove(sqlite3_path);

	sqlite3 *db; int ret = 0;
    ret = sqlite3_open(sqlite3_path.c_str(),&db);
	if (ret != SQLITE_OK) {
		cerr << "open db " << sqlite3_path << "error!" << endl; 
		return false;
	}

    //create table
    const char* sql_str = "CREATE TABLE STRUCT_INFO("
                            "module_name       VARCHAR(1024)  NOT NULL,"
                            "struct_name       VARCHAR(128)   NOT NULL,"
                            "member_node_str   VARCHAR(2048)  NOT NULL,"
                            "PRIMARY KEY (module_name, struct_name, member_node_str) );";
    char* errMsg = 0;
    int (*f)(void *, int, char **, char **) = [](void *, int, char **, char **) -> int {return 0;};

    ret = sqlite3_exec(db,sql_str,nullptr,0,&errMsg);
    if(ret != SQLITE_OK) {
        cerr << "create table error: " << errMsg << endl;
        return false;
    }

    for (pair<string, ModuleNode*> rc : root->getChildren()){
        ModuleNode* mn = rc.second;
        for (pair<string, StructNode*> mc : mn->getChildren()){
            StructNode* sn = mc.second;
            for (pair<string, MemberNode*> sc : sn->getChildren()){
                MemberNode* memn = sc.second;
                stringstream insert_sql;
                insert_sql << "INSERT INTO STRUCT_INFO VALUES (\'" 
                           << mn->getModuleName() << "\', \'" 
                           << sn->getStructName() << "\', \'" 
                           << memn->toString() << "\');";
                if((ret = sqlite3_exec(db, insert_sql.str().c_str(), f, 0, &errMsg)) != SQLITE_OK){
                    cerr << "insert record err: " << insert_sql.str() << " error:" << errMsg << endl;
                    // return false;
                };
            }
        }
    }

    sqlite3_close(db);
    return true;
}

RootNode* StructExtracter::loadFromMysql(string config_file, string table_name) {
    if (!filesystem::exists(config_file)) {
        cerr << "Mysql config does not exist: " << config_file << endl;
        return nullptr;
    }

	MYSQL* db = mysql::mysql_connect(config_file);
	if (!db) { return nullptr; }

    vector<string> columns = {"module_name", "struct_name", "member_node_str"};
    mysql::MysqlResult* map_res = mysql::mysql_select(db, table_name, columns, "", true);
    if (!map_res) { return nullptr; };

    for (auto map_raw : map_res->data) {
        string module_name = map_raw["module_name"];
        string struct_name = map_raw["struct_name"];
        string member_node_str = map_raw["member_node_str"];

        json j_member_node = json::parse(member_node_str);
        ModuleNode* mn = new ModuleNode();
        StructNode* sn = new StructNode();
        MemberNode* memn = new MemberNode(j_member_node);

        auto& modules = this->root->getChildren();
        if (modules.find(module_name) == modules.end()) {
            mn->setParent(this->root);
            (*this->root)[module_name] = mn;
        }
        auto& structs = (*this->root)[module_name]->getChildren();
        if (structs.find(struct_name) == structs.end()) {
            sn->setParent((*this->root)[module_name]);
            (*(*this->root)[module_name])[struct_name] = sn;
        }
        memn->setParent((*(*this->root)[module_name])[struct_name]);
        (*(*(*this->root)[module_name])[struct_name])[memn->getMemberName()] = memn; 
    }

    free(map_res); mysql_close(db);
    return this->root;

}

bool StructExtracter::saveToMysql(string config_file, string table_name, RootNode* root) {
    if (root == nullptr) root = this->root;

    if (!filesystem::exists(config_file)) {
        cout << "Could not find mysql config file: " << config_file << endl;
    }

	MYSQL* db = mysql::mysql_connect(config_file);
	if (!db) { return false; }
    
    if(!mysql::mysql_drop_table(db, table_name)) {
        return false;
    }

    vector<string> columns = {
        "module_name VARCHAR(192)  NOT NULL",
        "struct_name VARCHAR(64)   NOT NULL",
        "member_node_str VARCHAR(512)  NOT NULL",
    };
    vector<string> pks = {
        "PRIMARY KEY (module_name, struct_name, member_node_str)"
    };

    if (!mysql::mysql_create_table(db, table_name, columns, pks, "")){
        return false;
    };

    for (pair<string, ModuleNode*> rc : root->getChildren()){
        ModuleNode* mn = rc.second;
        for (pair<string, StructNode*> mc : mn->getChildren()){
            StructNode* sn = mc.second;
            for (pair<string, MemberNode*> sc : sn->getChildren()){
                MemberNode* memn = sc.second;
                vector<string> values = {mn->getModuleName(), sn->getStructName(), memn->toString()};
                if (!mysql::mysql_insert(db, table_name, values)){
                    return false;
                }
            }
        }
    }
    mysql_commit(db);
    mysql_close(db);
    return true;
}