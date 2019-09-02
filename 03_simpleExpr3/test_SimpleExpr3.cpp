#include <cmath>

#include "antlr4-runtime.h"
#include "SimpleExpr3Lexer.h"
#include "SimpleExpr3Parser.h"
#include "SimpleExpr3Listener.h"

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


class Calc : public SimpleExpr3Listener{
public:
        int result;

        void enterStat(SimpleExpr3Parser::StatContext *ctx) override {}

        void exitStat(SimpleExpr3Parser::StatContext *ctx) override {
                result = ctx->expr()->value;
        }


        void enterAdd(SimpleExpr3Parser::AddContext *ctx) override {}

        void exitAdd(SimpleExpr3Parser::AddContext *ctx) override {
                int left  = ctx->expr(0)->value;
                int right = ctx->expr(1)->value;
                ctx->value = right + left;
        }


        void enterExpo(SimpleExpr3Parser::ExpoContext *ctx) override {}

        void exitExpo(SimpleExpr3Parser::ExpoContext *ctx) override {
                int left  = ctx->expr(0)->value;
                int right = ctx->expr(1)->value;
                ctx->value = (int)pow(left, right);
                //cout << "left=" << left << " ,right=" << right << endl;
        }


        void enterMult(SimpleExpr3Parser::MultContext *ctx) override {}

        void exitMult(SimpleExpr3Parser::MultContext *ctx) override {
                int left  = ctx->expr(0)->value;
                int right = ctx->expr(1)->value;
                ctx->value = right * left;
                //cout << "left=" << left << " ,right=" << right << endl;
        }


        void enterInt(SimpleExpr3Parser::IntContext *ctx) override {}

        void exitInt(SimpleExpr3Parser::IntContext *ctx) override {
                ctx->value = stoi(ctx->INT()->getText());
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

 SimpleExpr3Lexer lexer(&input);
 CommonTokenStream tokens(&lexer);
 cout << "Tokens:" << endl;
 tokens.fill();
 for (Token *token : tokens.getTokens())
    cout << token->toString() << endl;
 
 SimpleExpr3Parser parser(&tokens);
 tree::ParseTree *tree = parser.stat();
 cout << endl << "Parse tree (Lisp format):" << endl;
 string lisp_tree_str = tree->toStringTree(&parser);
 //cout <<lisp_tree_str  << endl;
 cout <<beautify_lisp_string(lisp_tree_str) << endl;

 Calc *calc = new Calc();
 tree::ParseTreeWalker* walker = new tree::ParseTreeWalker();
 walker->walk(calc, tree);
 cout << "result = " << calc->result << endl;

 return 0;
}
