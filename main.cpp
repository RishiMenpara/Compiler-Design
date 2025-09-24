#include <iostream>
#include <string>
using namespace std;

enum class TokenType {
    number, plus, minus, mul, div, lparen, rparen, eof
};

struct Token {
    TokenType type;
    string text;
};

class Lexer {
private:
    string input;
    int index;
    
public:
    Lexer(const string& source) : input(source), index(0) {}

    Token getNext() {
        while (index < input.size()) {
            char ch = input[index];
            
            if (isspace(ch)) { 
                index++; 
                continue; 
            }

            if (isdigit(ch)) {
                string num;
                while (index < (int)input.size() && isdigit(input[index])) {
                    num.push_back(input[index++]);
                }
                return {TokenType::number, num};
            }

            if (ch == '+') { index++; return {TokenType::plus, "+"}; }
            if (ch == '-') { index++; return {TokenType::minus, "-"}; }
            if (ch == '*') { index++; return {TokenType::mul, "*"}; }
            if (ch == '/') { index++; return {TokenType::div, "/"}; }
            if (ch == '(') { index++; return {TokenType::lparen, "("}; }
            if (ch == ')') { index++; return {TokenType::rparen, ")"}; }

            cerr << "Bad character: " << ch << endl;
            index++;
        }
        return {TokenType::eof, ""};
    }
};

enum class NodeType { number, binop };

struct TreeNode {
    NodeType nodetype;
    int data;
    TreeNode* left;
    Token op;
    TreeNode* right;

    TreeNode(int val) : nodetype(NodeType::number), data(val), left(nullptr), right(nullptr) {}
    TreeNode(TreeNode* l, Token oper, TreeNode* r) : nodetype(NodeType::binop), data(0), left(l), op(oper), right(r) {}
    
    ~TreeNode() {
        delete left;
        delete right;
    }
};

class Parser {
private:
    Lexer &lex;
    Token token;

    void eat(TokenType expected) {
        if (token.type == expected) {
            token = lex.getNext();
        }
    }

    TreeNode* factor() {
        if (token.type == TokenType::number) {
            int val = stoi(token.text);
            eat(TokenType::number);
            return new TreeNode(val);
        }
        else if (token.type == TokenType::lparen) {
            eat(TokenType::lparen);
            TreeNode* result = expr();
            eat(TokenType::rparen);
            return result;
        }
        return nullptr;
    }

    TreeNode* term() {
        TreeNode* result = factor();
        
        while (token.type == TokenType::mul || token.type == TokenType::div) {
            Token oper = token;
            if (oper.type == TokenType::mul) 
                eat(TokenType::mul);
            else 
                eat(TokenType::div);
            result = new TreeNode(result, oper, factor());
        }
        return result;
    }

    TreeNode* expr() {
        TreeNode* result = term();
        
        while (token.type == TokenType::plus || token.type == TokenType::minus) {
            Token oper = token;
            if (oper.type == TokenType::plus) 
                eat(TokenType::plus);
            else 
                eat(TokenType::minus);
            result = new TreeNode(result, oper, term());
        }
        return result;
    }

public:
    Parser(Lexer &lexer) : lex(lexer) {
        token = lex.getNext();
    }

    TreeNode* parse() {
        return expr();
    }
};

class Value {
public:
    int num;
    
    Value(int n = 0) : num(n) {}

    Value operator+(const Value& other) const { 
        return Value(num + other.num); 
    }
    Value operator-(const Value& other) const { 
        return Value(num - other.num); 
    }
    Value operator*(const Value& other) const { 
        return Value(num * other.num); 
    }
    Value operator/(const Value& other) const {
        if (other.num == 0) 
            throw runtime_error("Division by zero");
        return Value(num / other.num);
    }
};

class Interpreter {
public:
    Value visit(TreeNode* tree) {
        if (tree->nodetype == NodeType::number) {
            return Value(tree->data);
        }
        else if (tree->nodetype == NodeType::binop) {
            Value left = visit(tree->left);
            Value right = visit(tree->right);
            
            switch (tree->op.type) {
                case TokenType::plus:  return left + right;
                case TokenType::minus: return left - right;
                case TokenType::mul:   return left * right;
                case TokenType::div:   return left / right;
                default: return Value(0);
            }
        }
        return Value(0);
    }
};

int main() {
    string expr;
    cout << "Enter expression: ";
    getline(cin, expr);

    Lexer lexer(expr);
    Parser parser(lexer);
    TreeNode* tree = parser.parse();

    Interpreter calc;
    try {
        Value answer = calc.visit(tree);
        cout << "Result: " << answer.num << endl;
    } catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    delete tree;
    return 0;
}
