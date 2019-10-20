#include "antlr4-runtime.h"
#include "CymbolLexer.h"
#include "CymbolParser.h"
#include "CymbolListener.h"

using namespace antlr4;
using namespace tree;
using namespace std;


string indent_sp(int size)
{ string result = "";
  for(int i; i < size; i++)
    result += " ";
  return result;
}


string beautify_lisp_string(string in_string)
{
int indent_size = 3;
int curr_indent = indent_size;
string out_string = string(1,in_string[0]);
string indent = "";
for (int i = 1; i < in_string.size(); i++)
        if(in_string[i] == '(' and  in_string[i+1] != ' '){
           curr_indent += indent_size;
           out_string += '\n' + indent_sp(curr_indent) + '(';
        }
        else if(in_string[i] == ')'){
          out_string += ')';
          if(curr_indent > indent_size){
            curr_indent -= indent_size;
          }
        }
        else
          out_string += in_string[i];
 
return out_string;
}


string join(const vector<string>& v, const char* delim = 0) {
    string s;
    if (!v.empty()) {
        s += v[0];
        for (decltype(v.size()) i = 1, c = v.size(); i < c; ++i) {
            if (delim) s += delim;
            s += v[i];
        }
    }
    return s;
}


class Symbol{
    public:    
        enum Type{INVALID, VOID, INT, FLOAT};
        map<int, string> symtype_map;        
        string _name;
        Type _symtype;
        string _symbolTypeName;
        
        Symbol(string name, Type symtype = INVALID){
            _name = name;
            _symtype = symtype;
            symtype_map[INVALID] = "INVALID";
            symtype_map[VOID] = "VOID";
            symtype_map[INT] = "INT";
            symtype_map[FLOAT] = "FLOAT";      
        }

        virtual string getName(){return _name;}
        virtual string getSymtypeName(){return symtype_map[_symtype];}
        virtual string toString(){return "<" + getName() + ":"+ getSymtypeName() + ">";} 
        virtual string getSymbolName(){return "";}      
};
       
            
class VariableSymbol: public Symbol{
    public:
        VariableSymbol(string name, Type stype):Symbol(name, stype){
            _symbolTypeName = "VariableSymbol";
        }
        virtual string getSymbolName(){return "Variable";}
};
        

class Scope{
    public:  
        virtual string getScopeName(){return "";}
        virtual Scope* getEnclosingScope(){return nullptr;}
        virtual void define(Symbol* sym){}
        virtual Symbol* resolve(string name){return nullptr;}
        virtual string toString(){return "Scope";}
};


class BaseScope: public Scope{
    public:
         map<string, Symbol*> symbols;
         Scope* _enclosingScope;

         BaseScope(Scope* scope){
            _enclosingScope = scope;
        }     
        
        virtual Symbol* resolve(string name){
            auto itr = symbols.find(name);
            if (itr != symbols.end())
                return symbols[name];
            if (_enclosingScope != nullptr)
                return _enclosingScope->resolve(name);
            return nullptr;       
        }

        virtual void define(Symbol* sym){
            symbols[sym->_name] = sym;
        }

        virtual Scope* getEnclosingScope(){
            return _enclosingScope;
        }
           
        virtual string toString(){
            vector<string> vec;
            for (auto itr = symbols.begin(); itr != symbols.end(); itr++)
                vec.push_back(itr->second->toString());
                //vec.push_back(itr->first);
                
            return getScopeName() + ":[" + join(vec, ", ") + "]";
        }
};
        
        
class GlobalScope: public BaseScope {
    public:
        GlobalScope(Scope* scope):BaseScope(scope){}

        virtual string getScopeName(){return "globals";}
};


class LocalScope: public BaseScope {
    public:
        LocalScope(Scope* scope):BaseScope(scope){}
        
        virtual string getScopeName(){return "locals";}
};


class FunctionSymbol : public Symbol, public BaseScope {
    public:
        FunctionSymbol(string name, Type symtype, Scope* scope = nullptr)
            :Symbol(name, symtype),BaseScope(scope){  }

        map<string, Symbol*> arguments;

        virtual Symbol* resolve(string name){
            auto itr = arguments.find(name);
            if (itr != arguments.end())
                return arguments[name];
            if (_enclosingScope != nullptr)
                return _enclosingScope->resolve(name);
            return nullptr;       
        }

        virtual string getSymbolName(){return "Function";} 

        virtual Scope* getEnclosingScope(){
            return _enclosingScope; 
        }

        virtual string getScopeName(){
            return _name;
        }

        void define(Symbol* sym){
            arguments[sym->_name] = sym;
        }

        virtual string toString() {
            vector<string> vec;
            for (auto itr = arguments.begin(); itr != arguments.end(); itr++)
                vec.push_back(itr->second->toString());
            return "function" + Symbol::toString() + ":[" + join(vec, ",") + "]";
        }
};


Symbol::Type getType(size_t tokenType){
    if(tokenType == CymbolParser::K_FLOAT)
        return Symbol::FLOAT;
    else if(tokenType == CymbolParser::K_INT)
        return Symbol::INT;
    else if (tokenType == CymbolParser::K_VOID)
        return Symbol::VOID;
    else
        return Symbol::INVALID;
}
    

class DefPhase : public CymbolListener{
    public:
        map<ParserRuleContext*, Scope*> scopes;
        GlobalScope* globals;
        Scope* currentScope;

        void enterFile(CymbolParser::FileContext *ctx) override { 
            globals = new GlobalScope(nullptr);
            currentScope = globals;
        }

        void exitFile(CymbolParser::FileContext *ctx) override { 
            cout << globals->toString() << endl;
        }

        void enterVarDecl(CymbolParser::VarDeclContext *ctx) override { }
        void exitVarDecl(CymbolParser::VarDeclContext *ctx) override { 
            defineVar(ctx->type(), ctx->ID()->getSymbol());
        }

        void enterType(CymbolParser::TypeContext *ctx) override { }
        void exitType(CymbolParser::TypeContext *ctx) override { }

        void enterFunctionDecl(CymbolParser::FunctionDeclContext *ctx) override {
            string name = ctx->ID()->getText();
            int typeTokenType = ctx->getStart()->getType();           
            Symbol::Type type = getType(typeTokenType);
            // push new scope by making new one that points to enclosing scope
            FunctionSymbol* function = new FunctionSymbol(name, type, currentScope);
            currentScope->define(function); // Define function in current scope
            scopes[ctx] = function;      // Push: set function's parent to current
            currentScope = function; // Current scope is now function scope
        }

        void exitFunctionDecl(CymbolParser::FunctionDeclContext *ctx) override { 
            cout << currentScope->toString() << endl;
            currentScope = currentScope->getEnclosingScope(); // pop scope
        }

        void enterFormalParameters(CymbolParser::FormalParametersContext *ctx) override { }
        void exitFormalParameters(CymbolParser::FormalParametersContext *ctx) override { }

        void enterFormalParameter(CymbolParser::FormalParameterContext *ctx) override { }
        void exitFormalParameter(CymbolParser::FormalParameterContext *ctx) override { 
            defineVar(ctx->type(), ctx->ID()->getSymbol());
        }

        void enterBlock(CymbolParser::BlockContext *ctx) override { 
            // push new local scope
            currentScope = new LocalScope(currentScope);
            scopes[ctx] = currentScope;
        }

        void exitBlock(CymbolParser::BlockContext *ctx) override { 
            cout << currentScope->toString() << endl;
            currentScope = currentScope->getEnclosingScope(); // pop scope
        }

        void enterBlockStat(CymbolParser::BlockStatContext *ctx) override { }
        void exitBlockStat(CymbolParser::BlockStatContext *ctx) override { }

        void enterVarDeclState(CymbolParser::VarDeclStateContext *ctx) override { }
        void exitVarDeclState(CymbolParser::VarDeclStateContext *ctx) override { }

        void enterIfStat(CymbolParser::IfStatContext *ctx) override { }
        void exitIfStat(CymbolParser::IfStatContext *ctx) override { }

        void enterReturnState(CymbolParser::ReturnStateContext *ctx) override { }
        void exitReturnState(CymbolParser::ReturnStateContext *ctx) override { }

        void enterAssignmentState(CymbolParser::AssignmentStateContext *ctx) override { }
        void exitAssignmentState(CymbolParser::AssignmentStateContext *ctx) override { }

        void enterFuncCallStat(CymbolParser::FuncCallStatContext *ctx) override { }
        void exitFuncCallStat(CymbolParser::FuncCallStatContext *ctx) override { }

        void enterCall(CymbolParser::CallContext *ctx) override { }
        void exitCall(CymbolParser::CallContext *ctx) override { }

        void enterNot(CymbolParser::NotContext *ctx) override { }
        void exitNot(CymbolParser::NotContext *ctx) override { }

        void enterMult(CymbolParser::MultContext *ctx) override { }
        void exitMult(CymbolParser::MultContext *ctx) override { }

        void enterAddSub(CymbolParser::AddSubContext *ctx) override { }
        void exitAddSub(CymbolParser::AddSubContext *ctx) override { }

        void enterEqual(CymbolParser::EqualContext *ctx) override { }
        void exitEqual(CymbolParser::EqualContext *ctx) override { }

        void enterVar(CymbolParser::VarContext *ctx) override { }
        void exitVar(CymbolParser::VarContext *ctx) override { }

        void enterParens(CymbolParser::ParensContext *ctx) override { }
        void exitParens(CymbolParser::ParensContext *ctx) override { }

        void enterIndex(CymbolParser::IndexContext *ctx) override { }
        void exitIndex(CymbolParser::IndexContext *ctx) override { }

        void enterNegate(CymbolParser::NegateContext *ctx) override { }
        void exitNegate(CymbolParser::NegateContext *ctx) override { }

        void enterInt(CymbolParser::IntContext *ctx) override { }
        void exitInt(CymbolParser::IntContext *ctx) override { }

        void enterExprList(CymbolParser::ExprListContext *ctx) override { }
        void exitExprList(CymbolParser::ExprListContext *ctx) override { }


        void enterEveryRule(antlr4::ParserRuleContext *ctx) override { }
        void exitEveryRule(antlr4::ParserRuleContext *ctx) override { }
        void visitTerminal(antlr4::tree::TerminalNode *node) override { }
        void visitErrorNode(antlr4::tree::ErrorNode *node) override { }

        void defineVar(CymbolParser::TypeContext* typeCtx, Token* nameToken) {
            int typeTokenType = typeCtx->start->getType();
            Symbol::Type type = getType(typeTokenType);
            VariableSymbol* var = new VariableSymbol(nameToken->getText(), type);
            currentScope->define(var); // Define symbol in current scope
        }  
};


void error(Token* t, string msg) {
    cout << "line " << t->getLine() << ":" << t->getCharPositionInLine() << msg << endl;
}


class RefPhase : public CymbolListener{
    public:
        map<ParserRuleContext*, Scope*>* scopes;
        GlobalScope* globals;
        Scope* currentScope;// resolve symbols starting in this scope

        RefPhase(GlobalScope* globals1, map<ParserRuleContext*, Scope*>* scopes1){
            scopes = scopes1;
            globals =  globals1;
        }

        void enterFile(CymbolParser::FileContext *ctx) override { 
            currentScope = globals;
        }

        void exitFile(CymbolParser::FileContext *ctx) override { 
           
        }

        void enterVarDecl(CymbolParser::VarDeclContext *ctx) override { }
        void exitVarDecl(CymbolParser::VarDeclContext *ctx) override { 
            
        }

        void enterType(CymbolParser::TypeContext *ctx) override { }
        void exitType(CymbolParser::TypeContext *ctx) override { }

        void enterFunctionDecl(CymbolParser::FunctionDeclContext *ctx) override {
            currentScope = (*scopes)[ctx];
        }

        void exitFunctionDecl(CymbolParser::FunctionDeclContext *ctx) override { 
            currentScope = currentScope->getEnclosingScope();
        }

        void enterFormalParameters(CymbolParser::FormalParametersContext *ctx) override { }
        void exitFormalParameters(CymbolParser::FormalParametersContext *ctx) override { }

        void enterFormalParameter(CymbolParser::FormalParameterContext *ctx) override { }
        void exitFormalParameter(CymbolParser::FormalParameterContext *ctx) override { }

        void enterBlock(CymbolParser::BlockContext *ctx) override { 
            currentScope = (*scopes)[ctx];
        }

        void exitBlock(CymbolParser::BlockContext *ctx) override { 
           currentScope = currentScope->getEnclosingScope();
        }

        void enterBlockStat(CymbolParser::BlockStatContext *ctx) override { }
        void exitBlockStat(CymbolParser::BlockStatContext *ctx) override { }

        void enterVarDeclState(CymbolParser::VarDeclStateContext *ctx) override { }
        void exitVarDeclState(CymbolParser::VarDeclStateContext *ctx) override { }

        void enterIfStat(CymbolParser::IfStatContext *ctx) override { }
        void exitIfStat(CymbolParser::IfStatContext *ctx) override { }

        void enterReturnState(CymbolParser::ReturnStateContext *ctx) override { }
        void exitReturnState(CymbolParser::ReturnStateContext *ctx) override { }

        void enterAssignmentState(CymbolParser::AssignmentStateContext *ctx) override { }
        void exitAssignmentState(CymbolParser::AssignmentStateContext *ctx) override { }

        void enterFuncCallStat(CymbolParser::FuncCallStatContext *ctx) override { }
        void exitFuncCallStat(CymbolParser::FuncCallStatContext *ctx) override { }

        void enterCall(CymbolParser::CallContext *ctx) override { }
        void exitCall(CymbolParser::CallContext *ctx) override { 
            // can only handle f(...) not expr(...)
            string funcName = ctx->ID()->getText();
            Symbol* meth = currentScope->resolve(funcName);
            if ( meth==nullptr ) {
                error(ctx->ID()->getSymbol(), " no such function: " + funcName);
            }
            else if (meth->getSymbolName() != "Function") {
                error(ctx->ID()->getSymbol(), " " + funcName + " is not a function");
            }
        }

        void enterNot(CymbolParser::NotContext *ctx) override { }
        void exitNot(CymbolParser::NotContext *ctx) override { }

        void enterMult(CymbolParser::MultContext *ctx) override { }
        void exitMult(CymbolParser::MultContext *ctx) override { }

        void enterAddSub(CymbolParser::AddSubContext *ctx) override { }
        void exitAddSub(CymbolParser::AddSubContext *ctx) override { }

        void enterEqual(CymbolParser::EqualContext *ctx) override { }
        void exitEqual(CymbolParser::EqualContext *ctx) override { }

        void enterVar(CymbolParser::VarContext *ctx) override { }
        void exitVar(CymbolParser::VarContext *ctx) override { 
            string name = ctx->ID()->getSymbol()->getText();
            Symbol* var = currentScope->resolve(name);
            if ( var==nullptr ) {
                error(ctx->ID()->getSymbol(), " no such variable: " + name);
            }
            else if (var->getSymbolName() != "Variable") {
                error(ctx->ID()->getSymbol(), " " + name + " is not a variable");
            }
        }

        void enterParens(CymbolParser::ParensContext *ctx) override { }
        void exitParens(CymbolParser::ParensContext *ctx) override { }

        void enterIndex(CymbolParser::IndexContext *ctx) override { }
        void exitIndex(CymbolParser::IndexContext *ctx) override { }

        void enterNegate(CymbolParser::NegateContext *ctx) override { }
        void exitNegate(CymbolParser::NegateContext *ctx) override { }

        void enterInt(CymbolParser::IntContext *ctx) override { }
        void exitInt(CymbolParser::IntContext *ctx) override { }

        void enterExprList(CymbolParser::ExprListContext *ctx) override { }
        void exitExprList(CymbolParser::ExprListContext *ctx) override { }


        void enterEveryRule(antlr4::ParserRuleContext *ctx) override { }
        void exitEveryRule(antlr4::ParserRuleContext *ctx) override { }
        void visitTerminal(antlr4::tree::TerminalNode *node) override { }
        void visitErrorNode(antlr4::tree::ErrorNode *node) override { }        
};


int main(int argc, const char *args[])
{
    ifstream ins;
 
    ins.open(args[1]);
    ANTLRInputStream input(ins);

    CymbolLexer lexer(&input);
    
    CommonTokenStream tokens(&lexer);
    /*
    cout << "Tokens:" << endl;
    tokens.fill();
    for (Token *token : tokens.getTokens())
        cout << token->toString() << endl;/**/ 

    CymbolParser parser(&tokens);
    tree::ParseTree *tree = parser.file();
    /*
    cout << endl << "Parse tree (Lisp format):" << endl;
    string lisp_tree_str = tree->toStringTree(&parser);
    //cout <<lisp_tree_str  << endl;
    cout <<beautify_lisp_string(lisp_tree_str) << endl;/**/

    ParseTreeWalker* walker = new ParseTreeWalker();

    cout << "*** Scan Definitions ***" << endl;
    DefPhase* def = new DefPhase();
    walker->walk(def, tree);

    //cout << collector->graph->toString() << endl;
    //cout << collector->graph->toDOT() << endl;

    cout << "\n*** Check errors ***" << endl;
    RefPhase* ref = new RefPhase(def->globals, &(def->scopes));
    walker->walk(ref, tree);

    return 0;
}

