#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

enum class TokenType {
    NUMBER,
    PLUS, MINUS, MUL, DIV, MOD,
    ASSIGN,
    ID, PRINT,
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    LESS, GREATER,
    IF, ELSE, FOR,
    SEMI,
    END
};

struct Token {
    TokenType type;
    string value;
};

class Lexer {
public:
    string src;
    int pos;

    Lexer(string s) {
        src = s;
        pos = 0;
    }

    Token getNextToken() {
        while (pos < src.size()) {
            char c = src[pos];
            if (isspace(c)) { 
                pos++; 
                continue; 
            }

            if (isalpha(c)) {
                string id = "";
                while (pos < src.size() && isalnum(src[pos])) {
                    id += src[pos++];
                }
                if (id == "if") return {TokenType::IF, id};
                if (id == "else") return {TokenType::ELSE, id};
                if (id == "for") return {TokenType::FOR, id};
                if (id == "print") return {TokenType::PRINT, id};
                return {TokenType::ID, id};
            }

            if (isdigit(c) || c == '.') {
                string num = "";
                bool hasDot = false;
                while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.')) {
                    if (src[pos] == '.') {
                        if (hasDot) break;
                        hasDot = true;
                    }
                    num += src[pos++];
                }
                return {TokenType::NUMBER, num};
            }

            pos++;
            if (c == '+') return {TokenType::PLUS, "+"};
            if (c == '-') return {TokenType::MINUS, "-"};
            if (c == '') return {TokenType::MUL, ""};
            if (c == '/') return {TokenType::DIV, "/"};
            if (c == '%') return {TokenType::MOD, "%"};
            if (c == '(') return {TokenType::LPAREN, "("};
            if (c == ')') return {TokenType::RPAREN, ")"};
            if (c == '{') return {TokenType::LBRACE, "{"};
            if (c == '}') return {TokenType::RBRACE, "}"};
            if (c == '=') return {TokenType::ASSIGN, "="};
            if (c == ';') return {TokenType::SEMI, ";"};
            if (c == '<') return {TokenType::LESS, "<"};
            if (c == '>') return {TokenType::GREATER, ">"};

            cout << "Unknown char: " << c << endl;
        }
        return {TokenType::END, ""};
    }
};

enum class NodeType { NUM, VAR, BINOP, ASSIGN, IFSTMT, FORSTMT, BLOCK, PRINT };

class Node {
public:
    NodeType type;
    Node(NodeType t) { type = t; }
    virtual ~Node() {}
};

class NumNode : public Node {
public:
    double val;
    NumNode(double v) : Node(NodeType::NUM), val(v) {}
};

class VarNode : public Node {
public:
    string name;
    VarNode(string n) : Node(NodeType::VAR), name(n) {}
};

class BinOpNode : public Node {
public:
    Node *left;
    Token op;
    Node *right;
    BinOpNode(Node *l, Token o, Node *r) : Node(NodeType::BINOP), left(l), op(o), right(r) {}
    ~BinOpNode() { delete left; delete right; }
};

class AssignNode : public Node {
public:
    string name;
    Node *expr;
    AssignNode(string n, Node *e) : Node(NodeType::ASSIGN), name(n), expr(e) {}
    ~AssignNode() { delete expr; }
};

class IfNode : public Node {
public:
    Node *cond;
    Node *thenStmt;
    Node *elseStmt;
    IfNode(Node *c, Node *t, Node *e) : Node(NodeType::IFSTMT), cond(c), thenStmt(t), elseStmt(e) {}
    ~IfNode() { delete cond; delete thenStmt; if (elseStmt) delete elseStmt; }
};

class ForNode : public Node {
public:
    Node *init;
    Node *cond;
    Node *update;
    Node *body;
    ForNode(Node *i, Node *c, Node *u, Node *b)
        : Node(NodeType::FORSTMT), init(i), cond(c), update(u), body(b) {}
    ~ForNode() { delete init; delete cond; delete update; delete body; }
};

class BlockNode : public Node {
public:
    Node* stmts[100];
    int count;
    BlockNode() : Node(NodeType::BLOCK), count(0) {}
    void add(Node* n) { stmts[count++] = n; }
    ~BlockNode() { for (int i = 0; i < count; i++) delete stmts[i]; }
};

class PrintNode : public Node {
public:
    Node* expr;
    PrintNode(Node* e) : Node(NodeType::PRINT), expr(e) {}
    ~PrintNode() { delete expr; }
};

class Parser {
public:
    Lexer &lex;
    Token curr;

    Parser(Lexer &l) : lex(l) { curr = lex.getNextToken(); }

    void eat(TokenType t) {
        if (curr.type == t) curr = lex.getNextToken();
        else throw runtime_error("Syntax error");
    }

    Node *factor() {
        if (curr.type == TokenType::NUMBER) {
            double v = stod(curr.value);
            eat(TokenType::NUMBER);
            return new NumNode(v);
        } else if (curr.type == TokenType::ID) {
            string n = curr.value;
            eat(TokenType::ID);
            return new VarNode(n);
        } else if (curr.type == TokenType::LPAREN) {
            eat(TokenType::LPAREN);
            Node *node = expr();
            eat(TokenType::RPAREN);
            return node;
        }
        throw runtime_error("Invalid factor");
    }

    Node *term() {
        Node *node = factor();
        while (curr.type == TokenType::MUL || curr.type == TokenType::DIV || curr.type == TokenType::MOD) {
            Token op = curr;
            eat(curr.type);
            node = new BinOpNode(node, op, factor());
        }
        return node;
    }

    Node *expr() {
        Node *node = term();
        while (curr.type == TokenType::PLUS || curr.type == TokenType::MINUS ||
               curr.type == TokenType::LESS || curr.type == TokenType::GREATER) {
            Token op = curr;
            eat(curr.type);
            node = new BinOpNode(node, op, term());
        }
        return node;
    }

    Node *statement() {
        if (curr.type == TokenType::PRINT) {
            eat(TokenType::PRINT);
            Node *exprNode = expr();
            return new PrintNode(exprNode);
        }
        else if (curr.type == TokenType::ID) {
            string name = curr.value;
            eat(TokenType::ID);
            if (curr.type == TokenType::ASSIGN) {
                eat(TokenType::ASSIGN);
                Node *rhs = expr();
                return new AssignNode(name, rhs);
            }
            else {
                return new VarNode(name);
            }
        }
        else if (curr.type == TokenType::IF) {
            eat(TokenType::IF);
            eat(TokenType::LPAREN);
            Node *cond = expr();
            eat(TokenType::RPAREN);
            Node *thenStmt = statement();
            Node *elseStmt = nullptr;
            if (curr.type == TokenType::ELSE) {
                eat(TokenType::ELSE);
                elseStmt = statement();
            }
            return new IfNode(cond, thenStmt, elseStmt);
        }
        else if (curr.type == TokenType::FOR) {
            eat(TokenType::FOR);
            eat(TokenType::LPAREN);
            Node *init = statement();
            eat(TokenType::SEMI);
            Node *cond = expr();
            eat(TokenType::SEMI);
            Node *update = statement();
            eat(TokenType::RPAREN);
            Node *body = statement();
            return new ForNode(init, cond, update, body);
        }
        else if (curr.type == TokenType::LBRACE) {
            eat(TokenType::LBRACE);
            BlockNode *block = new BlockNode();
            while (curr.type != TokenType::RBRACE && curr.type != TokenType::END) {
                Node *stmt = statement();
                block->add(stmt);
                if (curr.type == TokenType::SEMI)
                    eat(TokenType::SEMI);
            }
            eat(TokenType::RBRACE);
            return block;
        }
        return expr();
    }

    Node *parse() { return statement(); }
};


class Value {
public:
    double v;
    Value(double x = 0) { v = x; }

    Value operator+(Value o) { return Value(v + o.v); }
    Value operator-(Value o) { return Value(v - o.v); }
    Value operator*(Value o) { return Value(v * o.v); }
    Value operator/(Value o) { if (o.v == 0) throw runtime_error("Divide by zero"); return Value(v / o.v); }
    Value mod(Value o) { return Value((int)v % (int)o.v); }
};

struct Variable { string name; double value; };

class VarTable {
public:
    Variable vars[100];
    int count = 0;

    bool exists(string n) {
        for (int i = 0; i < count; i++)
            if (vars[i].name == n)
                return true;
        return false;
    }

    double get(string n) {
        for (int i = 0; i < count; i++)
            if (vars[i].name == n)
                return vars[i].value;
        throw runtime_error("Undefined variable " + n);
    }

    void set(string n, double val) {
        for (int i = 0; i < count; i++) {
            if (vars[i].name == n) {
                vars[i].value = val;
                return;
            }
        }
        vars[count++] = {n, val};
    }
};

class Interpreter
{
    map<string, Value> variables;

public:
    Value visit(BaseNode *node)
    {
        if (node->getType() == NodeType::number)
        {
            NumNode *n = static_cast<NumNode *>(node);
            return Value(n->getValue());
        }
        else if (node->getType() == NodeType::variable)
        {
            VarNode *v = static_cast<VarNode *>(node);
            if (variables.find(v->getName()) == variables.end())
                throw runtime_error("Undefined variable: " + v->getName());
            return variables[v->getName()];
        }
        else if (node->getType() == NodeType::assign)
        {
            AssignNode *a = static_cast<AssignNode *>(node);
            Value val = visit(a->getExpr());
            variables[a->getVarName()] = val;
            return val;
        }
        else if (node->getType() == NodeType::binop)
        {
            BinOpNode *b = static_cast<BinOpNode *>(node);
            Value left = visit(b->getLeft());
            Value right = visit(b->getRight());
            switch (b->getOp().type)
            {
            case TokenType::plus:
                return left + right;
            case TokenType::minus:
                return left - right;
            case TokenType::mul:
                return left * right;
            case TokenType::div:
                return left / right;
            case TokenType::mod:
                return left.mod(right);
            default:
                return Value(0);
            }
        }
        return Value(0);
    }
};

int main() {
    cout << "Simple Calculator with if/else, for, blocks, and print\n";
    Interpreter interp;

    string line, code = "";
    cout << ">>> ";
    while (getline(cin, line)) {
        if (line.empty()) break;
        code += line + "\n";
        if (line.find('}') != string::npos || line.find(';') != string::npos) {
            try {
                Lexer lex(code);
                Parser parser(lex);
                Node *tree = parser.parse();
                interp.visit(tree);
                delete tree;
            } catch (exception &e) {
                cout << "Error: " << e.what() << endl;
            }
            code = "";
            cout << ">>> ";
        }
    }
    return 0;
}
