#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "AST.hpp"
// initializing variable to store operator, keywords and their precedence
extern unordered_set<char> main_operator;
unordered_set<string> keywords;
unordered_map<string, double> special_constants;
unordered_map<char, int> operator_precedence = {
    {'+', 1},
    {'-', 1},
    {'*', 3},
    {'/', 3},
    {'^', 5},
    {'t', 9},
    {'l', 7},
    {'b', 8}
};
unordered_set<char> operators = {'+', '-', '*', '/', '^', '(', ')','^'};
vector<string>parsed_equation;
void initialize_keywords() {
    keywords.insert("log");
    keywords.insert("sin");
    keywords.insert("cos");  
    keywords.insert("tan");
    keywords.insert("sqrt");
}
void initialize_constants() {
    special_constants["pi"] = 3.14159265358979323846;
    special_constants["e"] = 2.71828182845904523536;
}
//The function parser is used to split plain characters into smaller segments that will be used by another function
//to make the required Abstract Syntax Tree(AST)
vector<string> parser(const string& equation,int& bracket_counter){
    string token="";
    for(int i=0;i<equation.length();i++){
        auto it=operators.find(equation[i]);
        if(it!=operators.end()){
            parsed_equation.push_back(string(1,equation[i]));
            if(equation[i]=='(')bracket_counter++;
        }
        if(equation[i]=='l' && equation.substr(i,2)=="ln"){
            parsed_equation.push_back("ln");
            i+=1;
            continue;
        }
        if(equation[i]=='l' && equation.substr(i,3)=="log"){
            parsed_equation.push_back("log");
            i+=2;
            continue;
        }
        if(equation[i]=='s' && equation.substr(i,3)=="sin"){
            parsed_equation.push_back("sin");
            i+=2;
            continue;
        }
        if(equation[i]=='c' && equation.substr(i,3)=="cos"){
            parsed_equation.push_back("cos");
            i+=2;
            continue;
        }
        if(equation[i]=='t' && equation.substr(i,3)=="tan"){
            parsed_equation.push_back("tan");
            i+=2;
            continue;
        }
        if(equation[i]=='s' && equation.substr(i,4)=="sqrt"){
            parsed_equation.push_back("sqrt");
            i+=3;
            continue;
        }
        if(isdigit(equation[i]) || equation[i]=='.'){
            token+=equation[i];
            while(i+1<equation.length() && (isdigit(equation[i+1]) || equation[i+1]=='.')){
                token+=equation[i+1];
                i++;
            }
            parsed_equation.push_back(token);
            if(isalpha(equation[i+1])){
                parsed_equation.push_back("*");
            }
            token="";
        }
        auto it2=special_constants.find(string(1, equation[i]));
        if(it2!=special_constants.end()){
            parsed_equation.push_back(string(1, equation[i]));
        }
        if(isalpha(equation[i])){
            token+=equation[i];
            while(i+1<equation.length() && isalpha(equation[i+1])){
                token+=equation[i+1];
                i++;
            }
            parsed_equation.push_back(token);
            token="";
        }
    }
    return parsed_equation;
}
//function to traverse the tree and solve operators according to the their branch node
// and to replace the value of lettered node if needed
void try_solve(Node* x,double& current_x) {
    if (!x)
        return;
    try_solve(x->next[0],current_x);
    try_solve(x->next[1],current_x);
    if(operator_precedence[x->type]==7&&x->keyword=="x"){
            x->result=current_x;
            x->value=current_x;
        }
    auto it=main_operator.find(x->type);
        if(it!=main_operator.end())operator_solver(x);
}
//function to push the result of the solution in the bracket up to the bracket so as to be used by the next operator
void resolve_brackets(Node* x) {
    if (!x)
        return;
    resolve_brackets(x->next[0]);
    resolve_brackets(x->next[1]);
        if(operator_precedence[x->type]==8){
            double bbc=(x->next[0])->result;
            x->result=bbc;
        }

}
//function to traverse the tree and solve keywords according to the their branch node
void resolve_keywords(Node* x) {
    if (!x)
        return;
    resolve_keywords(x->next[0]);
    resolve_keywords(x->next[1]);

        if(operator_precedence[x->type]==9){
            operator_keyword(x);
        }

}
// function to free the dynamically allocated nodes
void free(Node* x) {
    if (!x)
        return;
    free(x->next[0]);
    free(x->next[1]);

        delete x;
}

   
#endif