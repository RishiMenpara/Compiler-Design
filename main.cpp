#include <iostream>
#include <string>
using namespace std;

enum class TokenType {
    NUMBER, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END_OF_FILE
};

struct Token {
    TokenType type;
    string value;
};

class Lexer {
private:
    string src;
    int pos;
public:
    Lexer(const string& source) : src(source), pos(0) {}

    Token getNextToken() {
        while (pos < src.size()) {
            char current = src[pos];
            if (isspace(current)) { pos++; continue; }

            if (isdigit(current)) {
                string num;
                while (pos < (int)src.size() && isdigit(src[pos])) {
                    num.push_back(src[pos++]);
                }
                return {TokenType::NUMBER, num};
            }

            if (current == '+') { pos++; return {TokenType::PLUS, "+"}; }
            if (current == '-') { pos++; return {TokenType::MINUS, "-"}; }
            if (current == '') { pos++; return {TokenType::MUL, ""}; }
            if (current == '/') { pos++; return {TokenType::DIV, "/"}; }
            if (current == '(') { pos++; return {TokenType::LPAREN, "("}; }
            if (current == ')') { pos++; return {TokenType::RPAREN, ")"}; }

            cerr << "Unexpected character: " << current << endl;
            pos++;
        }
        return {TokenType::END_OF_FILE, ""};
    }
};

enum class NodeType { NUMBER, BINOP };

struct AST {
    NodeType nodetype;
    int value;
    AST* left;
    Token op;
    AST* right;

    AST(int v) : nodetype(NodeType::NUMBER), value(v), left(nullptr), right(nullptr) {}
    AST(AST* l, Token o, AST* r) : nodetype(NodeType::BINOP), value(0), left(l), op(o), right(r) {}
    ~AST() {
        delete left;
        delete right;
    }
};

class Parser {
private:
    Lexer &lexer;
    Token current;

    void eat(TokenType type) {
        if (current.type == type) {
            current = lexer.getNextToken();
        }
    }

    AST* factor() {
        if (current.type == TokenType::NUMBER) {
            int val = stoi(current.value);
            eat(TokenType::NUMBER);
            return new AST(val);
        }
        else if (current.type == TokenType::LPAREN) {
            eat(TokenType::LPAREN);
            AST* node = expr();
            eat(TokenType::RPAREN);
            return node;
        }
        return nullptr;
    }

    AST* term() {
        AST* node = factor();
        while (current.type == TokenType::MUL || current.type == TokenType::DIV) {
            Token op = current;
            if (op.type == TokenType::MUL) eat(TokenType::MUL);
            else eat(TokenType::DIV);
            node = new AST(node, op, factor());
        }
        return node;
    }

    AST* expr() {
        AST* node = term();
        while (current.type == TokenType::PLUS || current.type == TokenType::MINUS) {
            Token op = current;
            if (op.type == TokenType::PLUS) eat(TokenType::PLUS);
            else eat(TokenType::MINUS);
            node = new AST(node, op, term());
        }
        return node;
    }

public:
    Parser(Lexer &lex) : lexer(lex) {
        current = lexer.getNextToken();
    }

    AST* parse() {
        return expr();
    }
};

class Value {
public:
    int val;
    Value(int v = 0) : val(v) {}

    Value operator+(const Value& other) const { return Value(val + other.val); }
    Value operator-(const Value& other) const { return Value(val - other.val); }
    Value operator*(const Value& other) const { return Value(val * other.val); }
    Value operator/(const Value& other) const {
        if (other.val == 0) throw runtime_error("Division by zero");
        return Value(val / other.val);
    }
};

class Interpreter {
public:
    Value visit(AST* node) {
        if (node->nodetype == NodeType::NUMBER) {
            return Value(node->value);
        }
        else if (node->nodetype == NodeType::BINOP) {
            Value left = visit(node->left);
            Value right = visit(node->right);
            switch (node->op.type) {
                case TokenType::PLUS:  return left + right;
                case TokenType::MINUS: return left - right;
                case TokenType::MUL:   return left * right;
                case TokenType::DIV:   return left / right;
                default: return Value(0);
            }
        }
        return Value(0);
    }
};

int main() {
    string s;
    cout << "Enter an expression: ";
    getline(cin, s);

    Lexer lexer(s);
    Parser parser(lexer);
    AST* tree = parser.parse();

    Interpreter interp;
    try {
        Value result = interp.visit(tree);
        cout << "Result = " << result.val << endl;
    } catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    delete tree;
    return 0;
}
