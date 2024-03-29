## antlr4 Cpp examples  
What is ANTLR4  
[ANTLR4](http://www.antlr.org) (ANother Tool for Language Recognition) is a powerful parser generator  


### Creating antlr4 C++ library on mingw-w64  
<ol>
<li>
	
Install [mingw-w64](https://www.mingw-w64.org/downloads/)  

x86_64-13.1.0-release-win32-seh-msvcrt-rt_v11-rev1.7z  

</li>
<li>
	
Install [cmake](https://cmake.org/download/)  

Windows x64 Installer:  cmake-3.27.4-windows-x86_64.msi  

</li>
<li>
  
Download [antlr4-4.13.1.zip](https://github.com/antlr/antlr4/tags)  


</li> 
<li>  

Making antlr4 C++ libraries  `libantlr4-runtime-static.a`, `libantlr4-runtime.dll`
  
Open `Dos Command Prompt`  
make directory C:/antlr4/antlr4-4.13.1   
```
> unzip antlr4-4.13.1.zip 
> cd .../antlr4-4.13.1/runtime/Cpp  
> mkdir build  
> cd build  
> cmake -G "MinGW Makefiles" .. -D CMAKE_INSTALL_PREFIX=C:/antlr4/antlr4-4.13.1
> mingw32-make.exe install 
```  

Put the folder `cmake` in  `C:/antlr4/antlr4-4.13.1 `
</li>
<li>

Download [java](https://www.oracle.com/jp/java/technologies/downloads/#jdk21-windows)   
Download antlr-4.13.1-complete.jar ([ANTLR 4.13.1 Java runtime binaries jar](http://www.antlr.org/download.html))  
Make C:\Javalib and 
save antlr-4.13.1-complete.jar in C:/Javalib. 
</li>
</ol>


### First example (01_simpleExpr1)   

SimpleExpr1.g4
```antlr
grammar SimpleExpr1;

stat : expr ;

expr : <assoc=right> expr EXPO expr # Expo
     | expr MULT expr               # Mult
     | expr ADD  expr               # Add
     | INT                          # Int
     ;

EXPO : '^' ;
MULT : '*' ;
ADD  : '+' ;
INT : [0-9]+ ;
WS : [ \t\n\r]+ -> skip ;
```  


test_SimpleExpr1.cpp  
```c++   
#include <cmath>

#include "antlr4-runtime.h"
#include "SimpleExpr1Lexer.h"
#include "SimpleExpr1Parser.h"
#include "SimpleExpr1Visitor.h"


using namespace std;
using namespace antlr4;


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


class Calc : public SimpleExpr1Visitor{
public:
 
    any visitStat(SimpleExpr1Parser::StatContext *ctx) override {
        return visit(ctx->expr());
    }


    any visitAdd(SimpleExpr1Parser::AddContext *ctx) override {
        any left  = visit(ctx->expr(0));
        any right = visit(ctx->expr(1));
        return  any_cast<int>(left) +  any_cast<int>(right);
    }


    any visitExpo(SimpleExpr1Parser::ExpoContext *ctx) override {
        any left  = visit(ctx->expr(0));
        any right = visit(ctx->expr(1));
        return (int) pow( any_cast<int>(left),  any_cast<int>(right));
    }


    any visitMult(SimpleExpr1Parser::MultContext *ctx) override {
        any left  = visit(ctx->expr(0));
        any right = visit(ctx->expr(1));
        return  any_cast<int>(left) *  any_cast<int>(right); 
    }


    any visitInt(SimpleExpr1Parser::IntContext *ctx) override {
        return stoi(ctx->INT()->getText());
    }

};



int main(int argc, const char *args[])
{
    ifstream ifs;

    ifs.open(args[1]);
    ANTLRInputStream input(ifs);

    SimpleExpr1Lexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    cout << "Tokens:" << endl;
    tokens.fill();
    for (Token *token : tokens.getTokens())
    cout << token->toString() << endl;

    SimpleExpr1Parser parser(&tokens);
    tree::ParseTree *tree = parser.stat();
    cout << endl << "Parse tree (Lisp format):" << endl;
    string lisp_tree_str = tree->toStringTree(&parser);
    //cout <<lisp_tree_str  << endl;
    cout <<beautify_lisp_string(lisp_tree_str) << endl;

    Calc *calc = new Calc();
    any result = calc->visit(tree);
    cout << "\nresult = " << any_cast<int>(result) << endl;

    return 0;
}

```

test.txt  
```
100+3*4+2^3^2
```  

To compile,   
open  `Dos Command Prompt`  
from antlr4_Cpp_examples/01_simpleExpr1  
```
> mkdir build  
> cd build  
> cmake -G "MinGW Makefiles" ..  
> mingw32-make.exe  
> test_SimpleExpr1.exe ../test.txt   
```

output  
```
Tokens:
[@0,0:2='100',<4>,1:0]
[@1,3:3='+',<3>,1:3]
[@2,4:4='3',<4>,1:4]
[@3,5:5='*',<2>,1:5]
[@4,6:6='4',<4>,1:6]
[@5,7:7='+',<3>,1:7]
[@6,8:8='2',<4>,1:8]
[@7,9:9='^',<1>,1:9]
[@8,10:10='3',<4>,1:10]
[@9,11:11='^',<1>,1:11]
[@10,12:12='2',<4>,1:12]
[@11,13:12='<EOF>',<-1>,1:13]

Parse tree (Lisp format):
(stat 
   (expr 
      (expr 
         (expr 100) + 
         (expr 
            (expr 3) * 
            (expr 4))) + 
      (expr 
         (expr 2) ^ 
         (expr 
            (expr 3) ^ 
            (expr 2)))))

result = 624
```

Other examples are the same


