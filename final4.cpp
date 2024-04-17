#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib> 
#include <unordered_map> 
#include <vector>
#include <string>
using namespace std;


enum TokenType {
    ID,
    REF,
    LPAREN,
    RPAREN,
    COMMA,
    NUMBER,
    END
};

struct Token {
    TokenType type;
    string value;
};

class Parser{
    private:
    string input;
    size_t position;
    Token currentToken;
    int flagNonVoid=0;


    vector<string>adj[50];
    int lvl=0;

    void move(){
        position++;
    }
    char nextChar(){
        return input[position];
    }
    bool complete(){
        return (position>=input.size());
    }

    Token getNextToken(){
        if (complete()) {
            return {END,""};
        }
        char currentChar=nextChar();

        while(isspace(currentChar)){ 
            move();
            currentChar=nextChar();
        }
        if (isalpha(currentChar)||currentChar=='_'||currentChar=='$'||currentChar=='*'){
            string identifier;
            bool isfunc_ptr=false;
            if(currentChar=='*'){
                isfunc_ptr=true;
                identifier+=currentChar;
                move();
                currentChar=nextChar();
            }
            while (isalnum(currentChar)||currentChar=='_'||currentChar=='$'){
                identifier+=currentChar;
                move();
                currentChar=nextChar();
            }
            if(identifier.length()==1 && isfunc_ptr==true){
                cout<<"Syntax error: Unexpected token -"<<endl;
                exit(1);
            }
            return {ID,identifier};
        }
        else if(isdigit(currentChar)||currentChar=='-'){
            string number;
            bool isNegative=false;
            if(currentChar=='-'){
                number+=currentChar;
                move();
                currentChar = nextChar();
                isNegative = true;
            }
            while (isdigit(currentChar)) {
                number+=currentChar;
                move();
                currentChar=nextChar();
            }
            if(isNegative&&number.size()==1){
                cout<<"Syntax error: Unexpected token -"<<endl;
                exit(1);
            }
            return {NUMBER,number};
        }
        else if(currentChar=='&'){
            move();
            return {REF,"&"};
        } 
        else if(currentChar=='('){
            move();
            return {LPAREN,"("};
        } 
        else if(currentChar==')'){
            move();
            return {RPAREN, ")"};
        } 
        else if(currentChar==','){
            move();
            return {COMMA,","};
        } 
        else {
            cout<<"Invalid character encountered:"<<currentChar<<endl;
            exit(1);
        }
    }

    void match(TokenType expected){
        if(currentToken.type==expected){
            currentToken=getNextToken();
        }
        else{
            cout<<"Syntax error: Unexpected token "<<currentToken.value<<endl;
            exit(1);
        }
    }

    void param(){
        if(currentToken.type==ID||currentToken.type==NUMBER){
            adj[lvl+1].push_back(currentToken.value);
            match(currentToken.type);
        }
        else if(currentToken.type==REF){
            match(REF);
            adj[lvl+1].push_back("&"+currentToken.value);
            match(ID);
        }
        else{
            cout<<"Syntax error: Unexpected token "<<currentToken.value<<endl;
            exit(1);
        }
    }

    void Parameters(){
        lvl++;
        if(currentToken.type==ID||currentToken.type==NUMBER||currentToken.type==REF){
            param();
            adj[lvl].push_back("    param");
            if(currentToken.type==COMMA){
                match(COMMA);
                if(currentToken.type==RPAREN){
                    //, ke baad ) nhi aana chahiye
                    cout<<"Syntax error: Unexpected token "<<currentToken.value<<endl;
                    exit(1);
                }
                adj[lvl].push_back(" , ");
                adj[lvl].push_back(" B ");
                Parameters();
            }
        }
        else if(currentToken.type==RPAREN){
            //void call ke liye left or for end
            adj[2].push_back("epsilon");
            flagNonVoid=1;
        }
        else{
            cout<<"Syntax error: Unexpected token "<<currentToken.value<<endl;
            exit(1);
        }
    }

    void FUNC_CALL(){
        adj[lvl].push_back("        FUNC_CALL");
        lvl++;

        adj[1].push_back(currentToken.value);
        match(ID);

        adj[1].push_back(currentToken.value);
        match(LPAREN);

        adj[1].push_back("Parameters");

        lvl++;
        Parameters();
        if(flagNonVoid==0){
            adj[2].push_back("         B");
        }


        adj[1].push_back(currentToken.value);
        match(RPAREN);
    }
    public:

    Parser(string input){
        this->input=input;
        this->position=0;
        this->currentToken=getNextToken();
    }
    void parse() {
        FUNC_CALL();
        if(currentToken.type!=END){
            cout<<" Syntax error: Unexpected token "<<currentToken.value<<endl;
            exit(1);
        }
        adj[lvl+2].push_back("end");
        cout<<endl<<"Parsing successful"<<endl<<endl;
        print();
    }
    void print(){
        int flag=0;
        for(int i=0;i<50;i++){
            for(int j=0;j<adj[i].size();j++){
                if(adj[i][j]=="end"){
                    flag=1;
                    break;
                }
                cout<<adj[i][j]<<" ";
            }
            if(flag==1)break;
            cout<<endl<<endl;
        }
        cout<<endl<<endl;
    }
};

int main(){
    string input1="funcName(vari_able, *func_ptr , &_var, $var, -96,91,sarim,harsh,&m)";
    string input2="$amosa()";
    string input3="wrongFunc(7ver,%,)";
    Parser functionCall(input1);
    functionCall.parse();
    return 0;
}