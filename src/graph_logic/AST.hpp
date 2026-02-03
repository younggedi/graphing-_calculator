#include "header_includes.hpp"

using namespace std;

extern unordered_map<char, int> operator_precedence;
unordered_set<char> main_operator = {'+', '-', '*', '/', '^'};
extern unordered_set<string> keywords;

// Definition of the node and different constructors used in the AST
class Node {
    public:
    double value;
    char type;
    string keyword=" ";
    char operator_char;
    Node* next[2]={NULL, NULL};
    double result=0;
    bool complete=false;
    //Contructor for a number or lettered value(x)
    Node(double val){
        value = val;
        result =val;
        type='l';
        complete=true;
    }
    //Constructor for Keywords eg sin, cos, etc...
    Node(string key, Node* next_node){
        keyword = key;
        next[0] = next_node;
        type='t';
    }
    //Constructor for operators eg +,-,etc...
    Node(char op, Node* left, Node* right){
        operator_char = op;
        type = op;
        next[0] = left;
        next[1] = right;
        if(next[0]!=NULL && next[1]!=NULL)complete=true;
    }
    //Constructor for bracket Node
    Node(){
        type='b';
    }

};
//fuction used to view the created AST(Used for testing and Debbuging)
void traverse(Node* x) {
    if (!x)
        return;
    std::cout << x->type <<" = "<<x->result<< endl;
    traverse(x->next[0]);
    traverse(x->next[1]);
}
//Function to build the AST
Node* build_ast(const vector<string>& tokens, double x,int &i) {
    Node* root = nullptr;
    Node* current = nullptr;
    Node* highest_operator= nullptr;
    Node* current_operator= nullptr;
    char opera=' ';
    for(;i<tokens.size();i++){
        opera=' ';
        if(tokens[i].length()==1)opera = tokens[i][0];
        //makes a Node for a number
        if(isdigit(tokens[i][0])){
            Node *new_node= new Node(stod(tokens[i]));
             if(root==nullptr)root = new_node;//if the tree is empty
            current = new_node;
            if(current_operator!=nullptr&&current_operator->complete==false){ // connects the number to the current node if not fully occupied
                current_operator->next[1]=new_node;
                current_operator->complete=true;
                continue;
            }
            else if(highest_operator!=nullptr&&highest_operator->complete==false){ // connects the number to the highest node if not fully occupied
                highest_operator->next[1]=new_node;
                highest_operator->complete=true;
                continue;
            }
            
            new_node=nullptr;
        }
        //makes a Node for a lettered number(x)
        if(tokens[i]=="x"){
            Node *new_node= new Node(x);
            new_node->keyword="x";
            if(root==nullptr)root = new_node;//if the tree is empty
            current = new_node;
             if(current_operator!=nullptr&&current_operator->complete==false){// connects the number to the current node if not fully occupied
                current_operator->next[1]=new_node;
                current_operator->complete=true;
                continue;
            }
            if(highest_operator!=nullptr&&highest_operator->complete==false){// connects the number to the highest node if not fully occupied
                highest_operator->next[1]=new_node;
                highest_operator->complete=true;
                continue;
            }
            new_node=nullptr;
        }

        //makes a node for operators
        auto it=main_operator.find(opera);
        if(it!=main_operator.end()){
            Node *new_nodep=new Node(opera,NULL,NULL);
            Node *correct=root;
            if(root==nullptr){//if tree is empty
                root = new_nodep;
                highest_operator=new_nodep;
                current=new_nodep;
                root=new_nodep;
                current_operator=new_nodep;
                continue;
            }
            if(root->type=='^'&&new_nodep->type=='^'){//treats the operator differently as it is right associative
                Node* sub=root->next[1];
                root->next[1]=new_nodep;
                new_nodep->next[0]=sub;
                current=new_nodep;
                current_operator=new_nodep;
                continue;
            }
            if(operator_precedence[root->type]>=operator_precedence[new_nodep->type]||(root->type=='-'&&root->next[0]==NULL&&new_nodep->type!='^')){
                new_nodep->next[0]=root; 
                highest_operator=new_nodep;
                current=new_nodep;
                root=new_nodep;
                current_operator=new_nodep;
                continue;
            }
            while(operator_precedence[correct->next[1]->type]<=operator_precedence[new_nodep->type]){
                if(correct->next[1]->type=='-'&&correct->next[0]==NULL){//this accounts for unary minus
                break;
            }
                correct=correct->next[1];
            }
            if(correct->type=='^'&& new_nodep->type=='^'){//treats the operator differently as it is right associative
                Node* sub=correct->next[1];
                correct->next[1]=new_nodep;
                new_nodep->next[0]=sub;
                current=new_nodep;
                current_operator=new_nodep;
                continue;
            }
                Node* sub=correct->next[1];
                correct->next[1]=new_nodep;
                new_nodep->next[0]=sub;
                current=new_nodep;
                current_operator=new_nodep;
                continue;
            if(highest_operator==nullptr){
                highest_operator=new_nodep;
            }
            current_operator=new_nodep;

            new_nodep=nullptr;
        }
        //makes a node for keywords
        if(tokens[i]=="log"||tokens[i]=="sin"||tokens[i]=="cos"||tokens[i]=="tan"||tokens[i]=="sqrt"||tokens[i]=="ln"){
            Node* new_nodep=new Node(tokens[i],NULL);
            if(root==nullptr){
                root = new_nodep;
                current=new_nodep;
                current_operator=new_nodep;
                root=new_nodep;
                continue;
            }
            current_operator->next[1]=new_nodep;
            current_operator=new_nodep;
            continue;
        }
        //This creacts a node for brackets
        if(tokens[i]=="("){
            int brac=i+1;
            Node* bracket_Node=new Node();
            bracket_Node->next[0]=build_ast(tokens,x,brac);//The brackets are created by using recursion to create a seperate tree
            i=brac;
            if(root==nullptr){
            root = bracket_Node;
            continue;
            }
            if(current_operator->next[0]==nullptr&&current_operator->type=='t'){
                current_operator->next[0]=bracket_Node;
                continue;
            }
            
            current_operator->next[1]=bracket_Node;
            continue;
        }
        if(tokens[i]==")"){//Base case for the recursive function
            return root;
        }
    }
    return root; 

}

//Function to solve the operator while traversing the tree
void operator_solver(Node* root){
    char op=root->type;
    if(root->next[0]==nullptr){
        root->result=0-root->next[1]->result;
        return;
    }
    double left= root->next[0]->result;
    double right=root->next[1]->result;
    switch (op) {
        case '+': root->result= left + right;break;
        case '-': root->result= left - right;break;
        case '*': root->result= left * right;break;
        case '/': root->result= left / right;break;
        case '^': root->result= pow(left, right);break;
    }
}
//Function to solve the keyword while traversing the tree
void operator_keyword(Node* root){
    string op=root->keyword;
    if(root->next[0]==nullptr){
    root->result=0;
        return;
    }
    double left= root->next[0]->result;
        if(op=="sin"){root->result=sin(left) ;return;}
        if(op=="log"){ root->result=log10(left);return;}
        if(op=="cos"){root->result=cos(left);return;}
        if(op=="tan"){root->result=tan(left);return;}
        if(op=="sqrt"){root->result= sqrt(left);return;}
        if(op=="ln"){root->result= log(left);return;}
        
}