## antlr4 Cpp examples  
What is ANTLR4  
[ANTLR4](http://www.antlr.org) is a parser generator.

### Creating antlr4 C++ library on mingw-w64  
<ol>
<li>
	
Install [mingw-w64](https://mingw-w64.org/doku.php)  
Set path C:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin   

</li>
<li>
	
Install [cmake](https://cmake.org)  
cmake-3.15.2-win64-x64.msi  
Set path C:\CMake\bin  

</li>
<li>
Download [antlr4-4.7.2.zip](https://github.com/antlr/antlr4/releases) 
</li> 
<li>  

Making antlr4 C++ libraries `libantlr4-runtime.a`  
Open `Dos Command Prompt`  
$ unzip antlr4-4.7.2.zip  
$ cd .../antlr4-4.7.2/runtime/Cpp  
$ mkdir build  
$ cd build  
$ cmake -G "MinGW Makefiles" ..  
$ mingw32-make.exe  
$ mingw32-make.exe install  
If the following error message shows  
	
```
Install the project...   
-- Install configuration: "Release"  
CMake Error at cmake_install.cmake:36 (file):  
file cannot create directory: C:/Program Files  
``` 

You change `cmake_install.cmake` as follows  
old : set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/LIBANTLR4")  
new : set(CMAKE_INSTALL_PREFIX "C:/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/usr/local") 

Put the folder `cmake` in  `..mingw64/usr/local`  
</li>
<li>

Download [java](https://java.com/ja/download/help/download_options.xml)   
Download antlr-4.7.2-complete.jar ([Complete ANTLR 4.7.2 Java binaries jar](http://www.antlr.org/download.html))  
Make C:\Javalib and 
save antlr-4.7.2-complete.jar in C:/Javalib. 
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
#include <iostream>
#include <fstream>
#include <cmath>

#include "antlr4-runtime.h"
#include "SimpleExpr1Lexer.h"
#include "SimpleExpr1Parser.h"
#include "SimpleExpr1Visitor.h"

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


class Calc : public SimpleExpr1Visitor{
public:
 
    antlrcpp::Any visitStat(SimpleExpr1Parser::StatContext *ctx) override {
        return visit(ctx->expr());
    }


    antlrcpp::Any visitAdd(SimpleExpr1Parser::AddContext *ctx) override {
        int left  = visit(ctx->expr(0));
        int right = visit(ctx->expr(1));
        return left + right;
    }


    antlrcpp::Any visitExpo(SimpleExpr1Parser::ExpoContext *ctx) override {
        int left  = visit(ctx->expr(0));
        int right = visit(ctx->expr(1));
        return (int) pow(left, right);
    }


    antlrcpp::Any visitMult(SimpleExpr1Parser::MultContext *ctx) override {
        int right = visit(ctx->expr(0));
        int left  = visit(ctx->expr(1));
        return left * right; 
    }


    antlrcpp::Any visitInt(SimpleExpr1Parser::IntContext *ctx) override {
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
    int result = calc->visit(tree);
    cout << "\nresult=" << result << endl;

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
$ mkdir build  
$ cd build  
$ cmake -G "MinGW Makefiles" ..  
$ mingw32-make.exe  
$ test_SimpleExpr1.exe ../test.txt   

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


