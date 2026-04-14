#include <iostream>
#include <vector>
#include <string>
#include "BinaryTree.cpp"
using namespace std;

class Token{
public:
    string type;
    string value;
    BinaryTree tree;

    Token() : type(""), value(""), tree(nullptr) {}

    Token(string t, string v) : type(t), value(v), tree(nullptr) {}

    void printToken(){
        cout << this->type << ", " << this->value << endl;
    }
};

vector<Token> parse(string exp){
    vector<Token> tokens;
    string res = "";
    string type;
    
    for (size_t i = 0; i < exp.length(); i++){
        char c = exp[i];
        // cout << res << endl;

        if (isspace(c)){
            continue;
        }

        if (isdigit(c) || c=='.'){
            if (type != "num" && !type.empty()){
                Token tok(type, res);
                tokens.push_back(tok);
                res = "";
            }
            res += c;
            type = "num";
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^'){
            if (!type.empty()){
                Token tok(type, res);
                tokens.push_back(tok);
                res = "";
            }
            res += c;
            // cout << res << endl;
            type = "oper";
        }
        else if (c == '(' || c == ')'){
            if (!type.empty()){
                Token tok(type, res);
                tokens.push_back(tok);
                res = "";
            }
            res += c;
            type = "oper";
        }
        else{
            continue;
        }
    }

    Token tok(type, res);
    tokens.push_back(tok);

    // cout << res << endl;

    return tokens;
}

void printTokensVector(vector<Token> tokens){
    for (Token tok : tokens){
        tok.printToken();
    }
}


class Op{
public:
    Token left;
    Token op;
    Token right;
    float weightL;
    float weightR;

    Op(Token op) : op(op), weightL(0), weightR(0) {
        if (op.value == "+" || op.value == "-"){
            weightL = 1;
            weightR = 1.1;
        }
        else{
            weightL = 2;
            weightR = 2.1;
        }
    }

    void printOp(){
        cout << op.value << ", " << weightL << ", " << weightR << endl;
        // op.printToken();
    }
};

void printOpVector(vector<Op> ops){
    for (Op op : ops){
        op.printOp();
    }
}


pair<vector<Token>, vector<Op>> vectorOpOrdering(vector<Token> tokens){
    vector<Op> opsExp;
    vector<Token> tokensExp;

    for (Token tok : tokens){
        if (tok.type == "oper"){
            Op op(tok);
            opsExp.push_back(op);
        }
        else{
            tokensExp.push_back(tok);
        }
    }

    return {tokensExp, opsExp};
}

vector<Op> weightLifting(vector<Op> ops){
    vector<Op> weighted;
    int mult = 0;
    for (size_t i = 0; i < ops.size(); i++){
        if (ops[i].op.value == "("){
            mult += 10;
        }
        else if (ops[i].op.value == ")"){
            mult -= 10;
        }
        else{
            ops[i].weightL += mult;
            ops[i].weightR += mult;
            weighted.push_back(ops[i]);
        }
    }

    return weighted;
}

Token convertToTreeToken(Op op){
    Node* root = new Node(op.op.value);

    Node* leftNode = new Node(op.left.value);
    rewireNode(root, leftNode, 0);

    Node* rightNode = new Node(op.right.value);
    rewireNode(root, rightNode, 1);

    BinaryTree tree(root);

    Token tok("tree", op.op.value);
    tok.tree = tree;

    return tok;
}

// Op PrattParsing(vector<Token> tokensExp, vector<Op> opsExp){
//     Op op(Token tok());
//     return op;
// }



int main(){
    string exp = "1 + 2 / ((3 - 4)/1)";

    // 1 + (2 / ((3 - 4) / 1))

    vector<Token> tokens = parse(exp);
    printTokensVector(tokens);

    cout << "END TOKENS" << endl;

    auto [tokensExp, opsExp] = vectorOpOrdering(tokens);
    printTokensVector(tokensExp);
    // opsExp[0].printOp();

    vector<Op> weighted;
    weighted = weightLifting(opsExp);
    printOpVector(weighted);

    cout << "END WEIGHT" << endl;

    Token tok("oper", "+");
    Token le("num", "1");
    Token ri("num", "2");

    Op oper(tok);
    oper.left = le;
    oper.right = ri;
    Token treeToken = convertToTreeToken(oper);
    treeToken.printToken();
    cout << treeToken.tree.root->left->value << endl;
    cout << treeToken.tree.root->right->value << endl;
    // есть нода с типом tree и имеет в tree объект типа BinaryTree, в котором есть ссылки на левую и правую ноды с стрингами числа или операции

}

