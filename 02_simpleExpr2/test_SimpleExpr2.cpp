#include <cmath>

#include "antlr4-runtime.h"
#include "SimpleExpr2Lexer.h"
#include "SimpleExpr2Parser.h"
#include "SimpleExpr2Listener.h"

using namespace antlr4;
using namespace std;


string indent_sp(int size)
{   
    string result = "";
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
            if(curr_indent > indent_size)
                curr_indent -= indent_size;
        }
        else
            out_string += in_string[i];
     
return out_string;
}


class Calc : public SimpleExpr2Listener{
    public:
        int result;
        map<SimpleExpr2Parser::ExprContext*, int> memory;


        void enterStat(SimpleExpr2Parser::StatContext *ctx) override {}

        void exitStat(SimpleExpr2Parser::StatContext *ctx) override {
                result = memory[ctx->expr()];
        }


        void enterAdd(SimpleExpr2Parser::AddContext *ctx) override {}

        void exitAdd(SimpleExpr2Parser::AddContext *ctx) override {
            int left = memory[ctx->expr(0)];
            int right  = memory[ctx->expr(1)];
            memory[ctx] = right + left;
        }


        void enterExpo(SimpleExpr2Parser::ExpoContext *ctx) override {}

        void exitExpo(SimpleExpr2Parser::ExpoContext *ctx) override {
            int left = memory[ctx->expr(0)];
            int right  = memory[ctx->expr(1)];
            memory[ctx] = (int)pow(left, right);
        }


        void enterMult(SimpleExpr2Parser::MultContext *ctx) override {}

        void exitMult(SimpleExpr2Parser::MultContext *ctx) override {
            int left = memory[ctx->expr(0)];
            int right  = memory[ctx->expr(1)];
            memory[ctx] = right * left;
        }


        void enterInt(SimpleExpr2Parser::IntContext *ctx) override {}

        void exitInt(SimpleExpr2Parser::IntContext *ctx) override {
            memory[ctx]=stoi(ctx->INT()->getText());
        }

        void visitTerminal(tree::TerminalNode *node) override {}

        void visitErrorNode(tree::ErrorNode *node) override {}

        void enterEveryRule(ParserRuleContext *ctx) override {}

        void exitEveryRule(ParserRuleContext *ctx) override {}
};



int main(int argc, const char *args[])
{
    ifstream ins;

    ins.open(args[1]);
    ANTLRInputStream input(ins);

    SimpleExpr2Lexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    cout << "Tokens:" << endl;
    tokens.fill();
    for (Token *token : tokens.getTokens())
        cout << token->toString() << endl;

    SimpleExpr2Parser parser(&tokens);
    tree::ParseTree *tree = parser.stat();
    cout << endl << "Parse tree (Lisp format):" << endl;
    string lisp_tree_str = tree->toStringTree(&parser);
    //cout <<lisp_tree_str  << endl;
    cout <<beautify_lisp_string(lisp_tree_str) << endl;

    Calc *calc = new Calc();
    tree::ParseTreeWalker* walker = new tree::ParseTreeWalker();
    walker->walk(calc, tree);
    cout << "\nresult = " << calc->result << endl;

    return 0;
}
