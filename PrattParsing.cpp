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

    Token() : type("null"), value("null"), tree(nullptr) {}

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

    Op(): op(Token()), left(Token()), right(Token()), weightL(0), weightR(0) {}

    Op(Token op) : op(op), left(Token()), right(Token()), weightL(0), weightR(0) {
        if (op.value == "+" || op.value == "-"){
            weightL = 1;
            weightR = 1.1;
        }
        else if (op.value == "^"){
            weightL = 3;
            weightR = 3.1;
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

    if (op.left.type != "tree"){
        Node* leftNode = new Node(op.left.value);
        if (op.left.type == "null"){
            rewireNode(root, nullptr, 0);
        }
        else{
            rewireNode(root, leftNode, 0);
        }
    }
    else{
        rewireNode(root, op.left.tree.root, 0);
    }

    if (op.right.type != "tree"){
        Node* rightNode = new Node(op.right.value);
        if (op.right.type == "null"){
            rewireNode(root, nullptr, 1);
        }
        else{
            rewireNode(root, rightNode, 1);
        }
    }
    else{
        rewireNode(root, op.right.tree.root, 1);
    }
    

    BinaryTree tree(root);

    Token tok("tree", op.op.value);
    tok.tree = tree;

    return tok;
}

Token PrattParsing(vector<Token> tokensExp, vector<Op> opsExp){
    while (tokensExp.size() > 1){
        for (size_t i = 0; i < tokensExp.size(); i++){ // прошлись по токенам и прицепили
            if (tokensExp[i].type == "null"){
                continue;
            }
            if (i == 0 && opsExp[0].op.type != "null"){
                opsExp[0].left = tokensExp[0];
                tokensExp[0] = Token();
            }
            else if (i == 0 && opsExp[0].op.type == "null"){
                continue;
            }
            else if (i == tokensExp.size() - 1 && opsExp[i-1].op.type != "null"){
                opsExp[i-1].right = tokensExp[i];
                tokensExp[i] = Token();
            }
            else if (opsExp[i-1].op.type == "null"){
                continue;
            }
            else if (opsExp[i-1].op.type != "null" && opsExp[i].op.type == "null"){
                opsExp[i-1].right = tokensExp[i];
                tokensExp[i] = Token();
            }
            else if (opsExp[i-1].op.type == "null" && opsExp[i].op.type != "null"){
                opsExp[i].left = tokensExp[i];
                tokensExp[i] = Token();
            }
            else if (opsExp[i-1].weightR > opsExp[i].weightL){
                opsExp[i-1].right = tokensExp[i];
                tokensExp[i] = Token();
            }
            else{
                opsExp[i].left = tokensExp[i];
                tokensExp[i] = Token();
            }
        }

        for (int i = 0; i < opsExp.size(); i++){ // перевёл оперы в деревья и удалил умножения на 0 и оставшиеся лишние
            if (opsExp[i].op.value == "*" && (opsExp[i].left.value == "0" || opsExp[i].right.value == "0") && (opsExp[i].left.type != "null" && opsExp[i].right.type != "null")){
                opsExp[i] = Op();
                continue;
            }
            if (opsExp[i].left.type != "null" && opsExp[i].right.type != "null"){
                Token completedOp = convertToTreeToken(opsExp[i]);
                opsExp[i] = Op();
                tokensExp[i] = completedOp;
                continue;
            }
            if (tokensExp[i].type == "null" && tokensExp[i+1].type == "null" && opsExp[i].left.type != "null" && opsExp[i].right.type != "null"){
                opsExp[i] = Op();
                continue;
            }
        }

        for (int i = opsExp.size() - 1; i >= 0; i--){ // удалили лишние оперы и токены по дигонали вверх вправо
            if (opsExp[i].op.type == "null" && tokensExp[i+1].type == "null"){
                opsExp.erase(opsExp.begin() + i);
                tokensExp.erase(tokensExp.begin() + i + 1);
            }
        }

        bool fl = false; // проверка на то, если после удаления нулей остались изолированные оперы
        for (size_t i = 0; i < tokensExp.size(); i++){
            if (tokensExp[i].type != "null"){
                fl = true;
            }
        }
        if (!fl && opsExp.size() > 1){
            for (size_t i = opsExp.size() - 1; i >= 1; i--){
                if (opsExp[i].op.type != "null"){
                    opsExp[i] = Op();
                }
            }
        }

        if (opsExp.size() == 1 && tokensExp.size() == 2){
            if (tokensExp[0].type == "null" && tokensExp[1].type == "null"){
                Token completedOp = convertToTreeToken(opsExp[0]);
                opsExp[0] = Op();
                tokensExp[0] = completedOp;
            }
        }

        // printTokensVector(tokensExp);
        // printOpVector(opsExp);
    }

    return tokensExp[0];
}

bool isOperator(const string& s){
    return s == "+" || s == "-" || s == "*" || s == "/" || s == "^";
}

void printExpression(Node* node){
    if (!node) return;

    if (!isOperator(node->value)){
        cout << node->value;
        return;
    }

    if (node->left && !node->right){
        printExpression(node->left);
        return;
    }

    if (!node->left && node->right){
        printExpression(node->right);
        return;
    }

    if (isOperator(node->left->value)){
        cout << "( ";
        printExpression(node->left);
        cout << " )";
    } 
    else{
        printExpression(node->left);
    }

    cout << " " << node->value << " ";

    if (isOperator(node->right->value)){
        cout << "( ";
        printExpression(node->right);
        cout << " )";
    } 
    else{
        printExpression(node->right);
    }
}

int main(){
    string exp;

    while (true){
        cout << "Enter expression:" << endl;
        getline(cin, exp);

        if (exp == "exit"){
            break;
        }

        vector<Token> tokens = parse(exp);
        // printTokensVector(tokens);
        auto [tokensExp, opsExp] = vectorOpOrdering(tokens);
        vector<Op> weighted = weightLifting(opsExp);
        Token fin = PrattParsing(tokensExp, weighted);
        cout << "Binary expression tree:" << endl;
        fin.tree.printTree(fin.tree.root);
        cout << "Finalized expression:" << endl;
        printExpression(fin.tree.root);
        cout << endl; 

        fin.tree.clearTree(fin.tree.root);
        cout << "=================================" << endl;
    }
}

