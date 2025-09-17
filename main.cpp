#include <iostream>
#include <string>
using namespace std;

enum class TokenType {
    NUMBER, PLUS, MINUS, MUL, DIV, END_OF_FILE
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

    string getSource() const { return src; }

    Token getNextToken() {
        while (pos < (int)src.size()) {
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
            if (current == '*') { pos++; return {TokenType::MUL, "*"}; }
            if (current == '/') { pos++; return {TokenType::DIV, "/"}; }

            pos++;
        }
        return {TokenType::END_OF_FILE, ""};
void showSource(const Lexer &l)
{
    cout << "Lexer source: " << l.getSource() << endl;
}

class AST {
public:
    virtual ~AST() = default;
};

class NumberNode : public AST {
public:
    int value;
    NumberNode(int v = 0) : value(v) {}
};

class BinOpNode : public AST {
public:
    AST* left;
    Token op;
    AST* right;
    BinOpNode(AST* l, Token o, AST* r) : left(l), op(o), right(r) {}
};

class Parser {
private:
    Lexer &lexer;
    Token current;

    void eat(TokenType type) {
        if (current.type == type) {
            current = lexer.getNextToken();
        } else {
            cerr << "Unexpected token: " << current.value << endl;
        }
    }

    AST* factor() {
        if (current.type == TokenType::NUMBER) {
            int val = stoi(current.value);
            eat(TokenType::NUMBER);
            return new NumberNode(val);
        }
        return nullptr;
    }

    AST* term() {
        AST* node = factor();
        while (current.type == TokenType::MUL || current.type == TokenType::DIV) {
            Token op = current;
            if (op.type == TokenType::MUL) eat(TokenType::MUL);
            else eat(TokenType::DIV);
            node = new BinOpNode(node, op, factor());
        }
        return node;
    }

    AST* expr() {
        AST* node = term();
        while (current.type == TokenType::PLUS || current.type == TokenType::MINUS) {
            Token op = current;
            if (op.type == TokenType::PLUS) eat(TokenType::PLUS);
            else eat(TokenType::MINUS);
            node = new BinOpNode(node, op, term());
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

class Interpreter {
public:
    Value visit(AST* node) {
        if (auto num = dynamic_cast<NumberNode*>(node)) return visit(num);
        if (auto bin = dynamic_cast<BinOpNode*>(node)) return visit(bin);
        return Value(0);
    }
    Value visit(NumberNode* node) {
        return Value(node->value);
    }

    Value visit(BinOpNode* node) {
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
};

int main()
{
    string s;
    cout << "Enter an expression: ";
    getline(cin, s);

    Lexer lexer(s);
    showSource(lexer);

    Parser parser(lexer);
    AST *tree = parser.parse();

    Interpreter interp;
    int result = interp.visit(tree);
    cout << "Result = " << result << endl;

    delete tree;
    return 0;
}
