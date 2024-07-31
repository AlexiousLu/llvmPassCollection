#ifndef LLVM_ALIAS_ANALYSIS_HH
#define LLVM_ALIAS_ANALYSIS_HH

#include <include_llvm.hh>
#include <include_stl.hh>
#include <iostream>

using namespace llvm;
using namespace std;

class AliasNode {
public:
    set<Value*> aliasclass;
    AliasNode() {}
    int count(Value* V){
        return aliasclass.count(V);
    }

    void insert(Value* V){
        aliasclass.insert(V);
    }

    bool empty(){
        return aliasclass.empty();
    }

    void erase(Value* V){
        aliasclass.erase(V);
    }

    void print_set(){
        for(auto it = aliasclass.begin(); it != aliasclass.end(); it++){
            Value *v = *it;

            //Func definition is too long, just print its name
            if(Function *F = dyn_cast<Function>(v)){
                lerr <<"func: "<< F->getName().str() <<"\n"; 
                continue;
            }
            lerr << *v <<"\n";
        }
    }

};

template <int EdgeType>
class AliasEdge {
public:
    AliasNode *fromNode;
    AliasNode *toNode;
    int type = EdgeType; 

    AliasEdge() { fromNode = nullptr; toNode = nullptr; }

    AliasEdge(AliasNode *fromNode, AliasNode *toNode) {
        this->fromNode = fromNode; this->toNode = toNode;
    }
    
};

class AliasContext {
public:
    map<Value*, AliasNode*> nodeMap; //Record one value belongs to which alias node

    //Note: for * edge, there should be only one node

    //One node points to which node
    map<AliasNode*, map<int,AliasNode*>> toNodeMap; 

    //One node is pointed by which node
    map<AliasNode*, map<int,set<AliasNode*>>> fromNodeMap; 


    set<Function*> analyzedFuncSet;

    AliasContext(){
        nodeMap.clear();
        toNodeMap.clear();
        fromNodeMap.clear();
        analyzedFuncSet.clear();
    }

    AliasContext(AliasContext *C){
        nodeMap = C->nodeMap;
        toNodeMap = C->toNodeMap;
        fromNodeMap = C->fromNodeMap;
    }

    ~AliasContext(){
        set<AliasNode*> nodeSet;
        for(auto it = nodeMap.begin(); it != nodeMap.end(); it++){
            nodeSet.insert(it->second);
        }

        for(AliasNode* n : nodeSet){
            delete n;
        }
    }


    static void mergeNode(AliasNode* n1, AliasNode* n2);
    static bool isConnected(AliasNode* node1, AliasNode* node2);

};



//InstHandler
void HandleInst(Instruction* I, AliasContext *aliasCtx, GlobalContext *Ctx, bool handle_const = true);
void HandleLoad(LoadInst* LI, AliasContext *aliasCtx);
void HandleStore(StoreInst* STI, AliasContext *aliasCtx, bool handle_const = true);
void HandleStore(Value* vop, Value* pop, AliasContext *aliasCtx, bool handle_const = true);
void HandleGEP(GEPOperator* GEP, AliasContext *aliasCtx);
void HandleAlloc(AllocaInst* ALI, AliasContext *aliasCtx);
void HandleCai(CallInst* CAI, AliasContext *aliasCtx, GlobalContext *Ctx);
void HandleMove(Value* v1, Value* v2, AliasContext *aliasCtx);
void HandleReturn(Function* F, CallInst* cai, AliasContext *aliasCtx);
void HandleOperator(Value* v, AliasContext *aliasCtx);

//InstHandler-path based
void HandleInst_PB(Instruction* I, AliasContext *aliasCtx, GlobalContext *Ctx);
void HandleLoad_PB(LoadInst* LI, AliasContext *aliasCtx);
void HandleStore_PB(StoreInst* STI, AliasContext *aliasCtx);
void HandleStore_PB(Value* vop, Value* pop, AliasContext *aliasCtx);
void HandleGEP_PB(GEPOperator* GEP, AliasContext *aliasCtx);
void HandleAlloc_PB(AllocaInst* ALI, AliasContext *aliasCtx);
void HandleCai_PB(CallInst* CAI, AliasContext *aliasCtx, GlobalContext *Ctx);
void HandleMove_PB(Value* v1, Value* v2, AliasContext *aliasCtx);
void HandleReturn_PB(Function* F, CallInst* cai, AliasContext *aliasCtx);
void HandleOperator_PB(Value* v, AliasContext *aliasCtx);
bool is_alias_PB(Value* v1, Value* v2, AliasContext *aliasCtx);

//Tools
AliasNode* getNode(Value *V, AliasContext *aliasCtx);
AliasNode* PBgetNode(Value *V, AliasContext *aliasCtx);
bool isUselessInst(Instruction* I, GlobalContext *Ctx);

bool getGEPLayerIndex(GEPOperator *GEP, int &Index);
void analyzeFunction(Function* F, AliasContext *aliasCtx, GlobalContext *Ctx, bool handle_const = true);
void getClusterValues(Value* v, set<Value*> &valueSet, AliasContext *aliasCtx);
void getClusterNodes(AliasNode* startNode, set<AliasNode*> &nodeSet, AliasContext *aliasCtx);
void valueSetMerge(set<Value*> &S1, set<Value*> &S2);
void getPreviousValues(Value* v, set<Value*> &valueSet, AliasContext *aliasCtx);
void getPreviousNodes(AliasNode* startNode, set<AliasNode*> &nodeSet, AliasContext *aliasCtx);

//Debug
void showGraph(AliasContext *aliasCtx);
void showGraph_PB(AliasContext *aliasCtx);


#endif