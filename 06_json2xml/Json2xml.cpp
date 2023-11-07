// Example from The Definitive ANTLR 4 Reference by Terence Parr 
// JSON2XML.java -> json2xml.cpp

#include <iostream>
#include <fstream>
#include <string>

#include "antlr4-runtime.h"
#include "JSONLexer.h"
#include "JSONParser.h"
#include "JSONListener.h"

using namespace antlr4;
using namespace tree;
using namespace std;


string indent_sp(int size)
{   
    string result = "";
    for(int i = 0; i < size; i++)
        result += " ";
    return result;
}


string beautify_lisp_string(string in_string)
{
    int indent_size = 3;
    int curr_indent = 0;
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


class JSON2XML : public JSONListener{
    public:
        int depth = -1;

        string depth_sp(int depth1){   
            int indent_size = 3;
            string result = "";
            for(int i = 0; i < indent_size*depth1; i++)
                result += " ";
            return result;
        }

        JSON2XML(ParseTreeProperty<string>* xml)
            :xml(xml){}

        ParseTreeProperty<string>* xml;
        //string getXML(ParseTree *ctx) { return "("+ to_string(depth) + ")"+ xml->get(ctx); }
        string getXML(ParseTree *ctx) { return xml->get(ctx); }
        void setXML(ParseTree *ctx, string s) { xml->put(ctx, s); }

        void enterJson(JSONParser::JsonContext * /*ctx*/) override { }
        void exitJson(JSONParser::JsonContext *ctx) override {
            setXML(ctx, getXML(ctx->element()));
        }

        void enterElement(JSONParser::ElementContext* /*ctx*/) override { }
        void exitElement(JSONParser::ElementContext *ctx) override { 
            setXML(ctx, getXML(ctx->value()));
        }

        void enterObjectValue(JSONParser::ObjectValueContext *ctx) override { 
            depth++;
        }
        void exitObjectValue(JSONParser::ObjectValueContext *ctx) override {
            setXML(ctx, getXML(ctx->object()));
            depth--;
        }

        void enterArrayValue(JSONParser::ArrayValueContext * ctx) override {
            depth++;
        }
        void exitArrayValue(JSONParser::ArrayValueContext *ctx) override {            
            setXML(ctx, getXML(ctx->array()));
            depth--;
        }

        void enterString(JSONParser::StringContext * /*ctx*/) override { }
        void exitString(JSONParser::StringContext *ctx) override { 
            string str1 = ctx->getText();
            str1 = str1.substr(1, str1.size()-2);
            setXML(ctx, str1);
        }

        void enterAtom(JSONParser::AtomContext * /*ctx*/) override { }
        void exitAtom(JSONParser::AtomContext *ctx) override { 
            setXML(ctx, ctx->getText());
        }

        void enterEmptyObject(JSONParser::EmptyObjectContext * /*ctx*/) override { }
        void exitEmptyObject(JSONParser::EmptyObjectContext *ctx) override {
            setXML(ctx, "");
        }

        void enterAnObject(JSONParser::AnObjectContext* /*ctx*/) override { }
        void exitAnObject(JSONParser::AnObjectContext *ctx) override {
            string buf = "\n";
            for (int i = 0; i < ctx->member().size(); i++){                
                JSONParser::MemberContext* mctx =  ctx->member(i);
                buf += getXML(mctx);                
            }
            setXML(ctx, buf + depth_sp(depth-1));            
         }

        void enterEmptyArray(JSONParser::EmptyArrayContext * /*ctx*/) override { }
        void exitEmptyArray(JSONParser::EmptyArrayContext *ctx) override {
            setXML(ctx, "");
        }

        void enterArrayOfValues(JSONParser::ArrayOfValuesContext* /*ctx*/) override { }
        void exitArrayOfValues(JSONParser::ArrayOfValuesContext *ctx) override {
            string buf = "\n";
             for (int i = 0; i < ctx->element().size(); i++){
                JSONParser::ElementContext* ectx =  ctx->element(i);
                buf += depth_sp(depth) + "<element>" + getXML(ectx) + "</element>\n";
            }
            setXML(ctx, buf + depth_sp(depth-1));
        }

        void enterMember(JSONParser::MemberContext * /*ctx*/) override { }
        void exitMember(JSONParser::MemberContext *ctx) override {
            string tag = ctx->STRING()->getText();
            tag = tag.substr(1, tag.size()-2);
            JSONParser::ElementContext* ectx = ctx->element();
            string x = depth_sp(depth) + "<"+tag+">" + getXML(ectx) + "</"+tag+">\n";
            setXML(ctx, x);
        }

        void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
        void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
        void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
        void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }
};



int main(int argc, const char *args[])
{
    ParseTreeProperty<string>* xml = new ParseTreeProperty<string>();    

    ifstream ifs;
    ifs.open(args[1]);
    ANTLRInputStream input(ifs);

    JSONLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    cout << "Tokens:" << endl;
    tokens.fill();
    for (Token *token : tokens.getTokens())
        cout << token->toString() << endl;

    JSONParser parser(&tokens);
    tree::ParseTree *tree = parser.json();
    cout << endl << "Parse tree (Lisp format):" << endl;
    string lisp_tree_str = tree->toStringTree(&parser);
    //cout <<lisp_tree_str  << endl;
    cout <<beautify_lisp_string(lisp_tree_str) << endl;    
    
    JSON2XML *json2xml = new JSON2XML(xml);
    
    tree::ParseTreeWalker* walker = new tree::ParseTreeWalker();
    walker->walk(json2xml, tree);
    cout << "\nxml code:" << json2xml->getXML(tree) << endl;

    return 0;
}
