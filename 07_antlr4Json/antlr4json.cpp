#include "antlr4-runtime.h"
#include "JSONLexer.h"
#include "JSONParser.h"
#include "JSONListener.h"

#include "antlr4json.h"

using namespace antlr4;
using namespace tree;
using namespace std;

#include <iomanip>

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


class MyJSONListener : public JSONListener{
    public:
        unsigned int curr_addr = 0;
        map<ParseTree*, unsigned int> ctx2json_addr;
        map<unsigned int, JSON_NODE>* json;
        
        MyJSONListener(map<unsigned int, JSON_NODE>& json1){
            json = &json1;
        }


        void setJsonNode(ParseTree *ctx, string name, unsigned int child_addr, string keyOrAtom = ""){
            JSON_NODE *jnode = new JSON_NODE();
            jnode->name = name;
            jnode->my_addr = ++curr_addr;
            jnode->child_addr = child_addr;
            if (name == "Member")
                jnode->key = keyOrAtom;
            else
                jnode->atom = keyOrAtom;
            (*json)[curr_addr] = *jnode;
            ctx2json_addr[ctx] = curr_addr;
            if (curr_addr == UINT_MAX){
                cerr << "curr_addr is " << UINT_MAX << endl;
                exit(1);
            }
        }

        void enterJson(JSONParser::JsonContext * /*ctx*/) override { }
        void exitJson(JSONParser::JsonContext *ctx) override {
            setJsonNode(ctx, "Json", ctx2json_addr[ctx->element()]);
        }

        void enterElement(JSONParser::ElementContext* /*ctx*/) override { }
        void exitElement(JSONParser::ElementContext *ctx) override { 
            setJsonNode(ctx, "Element", ctx2json_addr[ctx->value()]);
        }

        void enterObjectValue(JSONParser::ObjectValueContext* /*ctx*/) override { }
        void exitObjectValue(JSONParser::ObjectValueContext *ctx) override {
            setJsonNode(ctx, "ObjectValue", ctx2json_addr[ctx->object()]);
        }

        void enterArrayValue(JSONParser::ArrayValueContext* /*ctx*/) override { }
        void exitArrayValue(JSONParser::ArrayValueContext *ctx) override {
            setJsonNode(ctx, "ArrayValue", ctx2json_addr[ctx->array()]);
        }

        void enterString(JSONParser::StringContext * /*ctx*/) override { }
        void exitString(JSONParser::StringContext *ctx) override {
            string buf = ctx->getText();
            buf = buf.substr(1, buf.size()-2);
            setJsonNode(ctx, "String", 0, buf);
        }

        void enterDouble(JSONParser::DoubleContext * /*ctx*/) override { }
        void exitDouble(JSONParser::DoubleContext *ctx) override {
            setJsonNode(ctx, "Double", 0, ctx->getText());
        }

        void enterIntExp(JSONParser::IntExpContext * /*ctx*/) override { }
        void exitIntExp(JSONParser::IntExpContext *ctx) override { 
            setJsonNode(ctx, "IntExp", 0, ctx->getText());
        }

        void enterInteger(JSONParser::IntegerContext * /*ctx*/) override { }
        void exitInteger(JSONParser::IntegerContext *ctx) override { 
            setJsonNode(ctx, "Integer", 0, ctx->getText());
        }

        void enterBool(JSONParser::BoolContext * /*ctx*/) override { }
        void exitBool(JSONParser::BoolContext *ctx) override {
            setJsonNode(ctx, "Bool", 0, ctx->getText());
        }

        void enterNull(JSONParser::NullContext * /*ctx*/) override { }
        void exitNull(JSONParser::NullContext *ctx) override {
            setJsonNode(ctx, "Null", 0, ctx->getText());
        }

        void enterEmptyObject(JSONParser::EmptyObjectContext * /*ctx*/) override { }
        void exitEmptyObject(JSONParser::EmptyObjectContext *ctx) override {
            setJsonNode(ctx, "EmptyObject", 0);
        }

        void enterAnObject(JSONParser::AnObjectContext* /*ctx*/) override { }
        void exitAnObject(JSONParser::AnObjectContext *ctx) override {
            for (int i = 0; i < ctx->member().size(); i++){
                if (i+1 < ctx->member().size())
                    (*json)[ctx2json_addr[ctx->member(i)]].sibling_addr = ctx2json_addr[ctx->member(i+1)];
            }
            setJsonNode(ctx, "AnObject", ctx2json_addr[ctx->member(0)]);
        }

        void enterEmptyArray(JSONParser::EmptyArrayContext * /*ctx*/) override { }
        void exitEmptyArray(JSONParser::EmptyArrayContext *ctx) override {
            setJsonNode(ctx, "EmptyObject", 0);
        }

        void enterArrayOfValues(JSONParser::ArrayOfValuesContext* /*ctx*/) override { }
        void exitArrayOfValues(JSONParser::ArrayOfValuesContext *ctx) override {
             for (int i = 0; i < ctx->element().size(); i++){
                if (i+1 < ctx->element().size())
                    (*json)[ctx2json_addr[ctx->element(i)]].sibling_addr = ctx2json_addr[ctx->element(i+1)];
            }
            setJsonNode(ctx, "ArrayOfValues", ctx2json_addr[ctx->element(0)]);            
        }

        void enterMember(JSONParser::MemberContext * /*ctx*/) override { }
        void exitMember(JSONParser::MemberContext *ctx) override {
            string key = ctx->STRING()->getText();
            key = key.substr(1, key.size()-2);
            setJsonNode(ctx, "Member", ctx2json_addr[ctx->element()], key);
        }

        void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
        void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
        void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
        void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }
};


map<unsigned int, JSON_NODE> antlr4json(string filename){
    ifstream ifs;
    ifs.open(filename);
    if (ifs.fail()) {
        cerr << "Failed to open file." << endl;
        exit(1);
    }

    ANTLRInputStream input(ifs);

    JSONLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    /*
    cout << "Tokens:" << endl;
    tokens.fill();
    for (Token *token : tokens.getTokens())
        cout << token->toString() << endl;/**/
    JSONParser parser(&tokens);
    tree::ParseTree *tree = parser.json();
    /*
    cout << endl << "Parse tree (Lisp format):" << endl;
    string lisp_tree_str = tree->toStringTree(&parser);
    //cout <<lisp_tree_str  << endl;
    cout <<beautify_lisp_string(lisp_tree_str) << endl;/**/  
    map<unsigned int, JSON_NODE> json;
    MyJSONListener *myjson = new MyJSONListener(json);
    
    tree::ParseTreeWalker* walker = new tree::ParseTreeWalker();
    walker->walk(myjson, tree);

    return json;
}


#define TO_STRING(VariableName) # VariableName

void dump_antlr4json(map<unsigned int, JSON_NODE>& ajson){
    string name = TO_STRING(ajson);
    unsigned int json_addr = ajson.size(); // json address
    cout << "json\'s address is " << json_addr << endl;
    for (int i = 1; i < json_addr + 1; i++)
        cout  << name <<  "[" << setw(4) << i << "]:" << " name = " << setw(14) << ajson[i].name 
         << ", child = "<< setw(4) << ajson[i].child_addr << ", sibling = " << setw(4) << ajson[i].sibling_addr
         << ", key = " << ajson[i].key << ", atom = " << ajson[i].atom << endl;
}


JSON_NODE* jasonNodeWalk(map<unsigned int, JSON_NODE>& ajson, JSON_NODE& currJn, string route){
    JSON_NODE *curr_jn = &currJn;
    for (int i = 0; i < route.size(); i++){
        char cs = route[i];
        if (cs == 'c'){
            if (curr_jn->child_addr == 0){
                //cerr << "route \"" << route << "\" is wrong"<< endl;
                return nullptr;
            }
            curr_jn = &(ajson[curr_jn->child_addr]);            
        }
        else if (cs == 's'){
            if (curr_jn->sibling_addr == 0){
                //cerr << "route \"" << route << "\" is wrong"<< endl;
                return nullptr;
            }
            curr_jn = &(ajson[curr_jn->sibling_addr]);            
        }
    }
    return curr_jn;
}


void test_jasonNodeWalk(map<unsigned int,JSON_NODE>& ajson, JSON_NODE& currJn, string route){
    JSON_NODE* jn = jasonNodeWalk(ajson, currJn, route);
    if (jn == nullptr)
        cout << "ajson[" << currJn.my_addr << "] --" << route << "-> is wrong" << endl;
    else
        cout << "ajson[" << currJn.my_addr << "] --" << route << "-> "
        <<  "ajson[" << jn->my_addr << "]: " <<"name = " << jn->name
        << ", key = " << jn->key << ", atom = "<< jn->atom << endl;
}


unsigned int getJasonNodeKeyAddr(map<unsigned int,JSON_NODE>& ajson, string target_key){
    for(auto itr = ajson.begin(); itr != ajson.end(); itr++){
        if (ajson[itr->first].key == target_key )
            return ajson[itr->first].my_addr;
    }
    return 0;
}


int main(int argc, const char *args[])
{
    map<unsigned int,JSON_NODE> ajson = antlr4json(args[1]);
    dump_antlr4json(ajson);

    cout << "\njasonNodeWalk test" << endl;
    unsigned int json_addr = ajson.size();
    test_jasonNodeWalk(ajson, ajson[json_addr], "cccc");
    test_jasonNodeWalk(ajson, ajson[json_addr], "cccccc");
    test_jasonNodeWalk(ajson, ajson[json_addr], "ccccccc"); // route "ccccccc" is wrong
    test_jasonNodeWalk(ajson, ajson[json_addr], "ccccsssccccc");
    test_jasonNodeWalk(ajson, ajson[json_addr], "ccccsssccccsc");

    cout << "\ngetJasonNodeKeyAddr test" << endl;
    string target_key[] = {"aliases", "dir", "real"};     
    for (string tkey : target_key){
        unsigned int addr = getJasonNodeKeyAddr(ajson, tkey);
        if (addr != 0)
            cout << "ajson[" << addr << "].key = " << tkey << endl;
        else
            cout << "there is not \"" << tkey << "\" in ajson.key" <<endl;
    }    
    return 0;
}
