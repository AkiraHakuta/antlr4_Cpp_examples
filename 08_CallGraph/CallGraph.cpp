#include "antlr4-runtime.h"
#include "CymbolLexer.h"
#include "CymbolParser.h"
#include "CymbolListener.h"

using namespace antlr4;
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


class Graph{
    public:
        vector<string> nodes;
        map<string, vector<string>> edges;

        void edge(string source, string target) {
            edges[source].push_back(target);
        }

        string toString() {
            string buf= "edges: {";
            for (auto itr = edges.begin(); itr != edges.end(); itr++){
                buf += itr->first + "=[" + join(itr->second, ",") + "]";
                if ((++itr)  != edges.end())
                    buf += ",";
                --itr;                
            }
            buf += "}\n";
            buf += "functions: [" + join(nodes, ",") + "]"; 
            return  buf;
        }

        string toDOT(){
            string buf;
            buf =  "digraph G {\n  nodes:\n";
            buf += "    " + join(nodes, ";") + ";\n"; 
            buf += "  edges:\n";
            for (auto itr1 = edges.begin(); itr1 != edges.end(); itr1++){
                for (auto itr2 = itr1->second.begin(); itr2 != itr1->second.end(); itr2++){
                    buf += "    ";
                    buf += itr1->first;
                    buf += " -> ";
                    buf += *itr2;
                    buf += ";\n";                    
                }                    
            }
            buf += "}\n";
            return buf;
        }
};


class FunctionListener : public CymbolListener{
    public:

        Graph* graph = new Graph();
        string currentFunctionName;

        void enterFile(CymbolParser::FileContext *ctx) override { }
        void exitFile(CymbolParser::FileContext *ctx) override { }

        void enterVarDecl(CymbolParser::VarDeclContext *ctx) override { }
        void exitVarDecl(CymbolParser::VarDeclContext *ctx) override { }

        void enterType(CymbolParser::TypeContext *ctx) override { }
        void exitType(CymbolParser::TypeContext *ctx) override { }

        void enterFunctionDecl(CymbolParser::FunctionDeclContext *ctx) override {
            currentFunctionName = ctx->ID()->getText();
            graph->nodes.push_back(currentFunctionName);
        }

        void exitFunctionDecl(CymbolParser::FunctionDeclContext *ctx) override { }

        void enterFormalParameters(CymbolParser::FormalParametersContext *ctx) override { }
        void exitFormalParameters(CymbolParser::FormalParametersContext *ctx) override { }

        void enterFormalParameter(CymbolParser::FormalParameterContext *ctx) override { }
        void exitFormalParameter(CymbolParser::FormalParameterContext *ctx) override { }

        void enterBlock(CymbolParser::BlockContext *ctx) override { }
        void exitBlock(CymbolParser::BlockContext *ctx) override { }

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
            string funcName = ctx->ID()->getText();
            // map current function to the callee
            graph->edge(currentFunctionName, funcName);
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

    FunctionListener *collector = new FunctionListener();
    tree::ParseTreeWalker* walker = new tree::ParseTreeWalker();
    walker->walk(collector, tree);

    cout << collector->graph->toString() << endl;
    cout << endl;
    cout << collector->graph->toDOT() << endl;

    return 0;
}

