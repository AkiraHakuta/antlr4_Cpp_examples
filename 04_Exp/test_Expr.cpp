#include <cmath>

#include "antlr4-runtime.h"
#include "ExprLexer.h"
#include "ExprParser.h"
#include "ExprVisitor.h"

using namespace std;
using namespace antlr4;


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


class Calc : public ExprVisitor{
    public:
        map<string, double> id_memory;    
        bool output_flag;


        antlrcpp::Any visitProg(ExprParser::ProgContext *ctx) override {
            vector<ExprParser::StatContext *> state_ctx = ctx->stat();
            string result = "";

            for (int i = 0; i < state_ctx.size(); i++){ 
                string stat_str;     
                stat_str = to_string((double)visit(ctx->stat(i)));
                //cout << output_flag << endl;
                if (output_flag)          
                    result += stat_str + '\n';    
            }
            return result;
        }


        antlrcpp::Any visitReturnValue(ExprParser::ReturnValueContext *ctx) override {
            output_flag = true;
            return visit(ctx->expr());
        }


        antlrcpp::Any visitAssignment(ExprParser::AssignmentContext *ctx) override {
            id_memory[ctx->ID()->getText()] = visit(ctx->expr());

            auto begin = id_memory.begin(), end = id_memory.end();
            cout << "id_memory{";
            for (auto iter = begin; iter != end; iter++) {
                cout << "\"" << iter->first << "\":" << iter->second <<",";
            }
            cout << "}\n";

            output_flag = false;
            return 0.0;
        }


        antlrcpp::Any visitIgnore(ExprParser::IgnoreContext *ctx) override {
            output_flag = false;
            return 0.0;
        }


        antlrcpp::Any visitParen(ExprParser::ParenContext *ctx) override {
            return visit(ctx->expr());
        }


        antlrcpp::Any visitPm_expr(ExprParser::Pm_exprContext *ctx) override {
            double result = visit(ctx->expr());
            if (ctx->op->getText()[0] == '-')
                result = (-1)*result;
            return result;
        }


        antlrcpp::Any visitVar(ExprParser::VarContext *ctx) override {
            return id_memory[ctx->ID()->getText()];
        }


        antlrcpp::Any visitMul_div(ExprParser::Mul_divContext *ctx) override {
            double left  = visit(ctx->expr(0));
            double right = visit(ctx->expr(1));
            double result;
            if (ctx->op->getText()[0] == '*')
                result = left * right;
            else
                result = left / right;
            return result;        
        }


        antlrcpp::Any visitInteger(ExprParser::IntegerContext *ctx) override {
            return stod(ctx->INT()->getText());
        }


        antlrcpp::Any visitFloat(ExprParser::FloatContext *ctx) override {
            return stod(ctx->FLOAT()->getText());
        }


        antlrcpp::Any visitExpo(ExprParser::ExpoContext *ctx) override {
            double left  = visit(ctx->expr(0));
            double right = visit(ctx->expr(1));
            return pow(left, right);
        }

        antlrcpp::Any visitAdd_sub(ExprParser::Add_subContext *ctx) override {
            double left  = visit(ctx->expr(0));
            double right = visit(ctx->expr(1));
            double result;
            if (ctx->op->getText()[0] == '+')
                result = left + right;
            else
                result = left - right;
            return result;
        }
};



int main(int argc, const char *args[])
{
    ifstream ifs;

    ifs.open(args[1]);
    ANTLRInputStream input(ifs);

    ExprLexer lexer(&input);    
    CommonTokenStream tokens(&lexer);
    cout << "Tokens:" << endl;
    tokens.fill();
    for (Token *token : tokens.getTokens())
        cout << token->toString() << endl;

    ExprParser parser(&tokens);
    tree::ParseTree *tree = parser.prog();
    cout << endl << "Parse tree (Lisp format):" << endl;
    string lisp_tree_str = tree->toStringTree(&parser);
    //cout <<lisp_tree_str  << endl;
    cout <<beautify_lisp_string(lisp_tree_str) << '\n' << endl;
   
    Calc *calc = new Calc();
    string result = calc->visit(tree);
    cout << "\nresult=\n" << result << endl;

    return 0;
}
