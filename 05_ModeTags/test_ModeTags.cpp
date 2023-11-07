#include "antlr4-runtime.h"
#include "ModeTagsLexer.h"
#include "ModeTagsParser.h"

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


int main(int argc, const char *args[])
{
    ifstream ifs;

    ifs.open(args[1]);
    ANTLRInputStream input(ifs);

    ModeTagsLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    cout << "Tokens:" << endl;
    tokens.fill();
    for (Token *token : tokens.getTokens())
        cout << token->toString() << endl;

    ModeTagsParser parser(&tokens);
    tree::ParseTree *tree = parser.file();
    cout << endl << "Parse tree (Lisp format):" << endl;
    string lisp_tree_str = tree->toStringTree(&parser);
    //cout <<lisp_tree_str  << endl;
    cout <<beautify_lisp_string(lisp_tree_str) << endl;
    return 0;
}
