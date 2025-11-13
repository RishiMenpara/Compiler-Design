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

class Parser
{
private:
    Lexer &lexer;
    Token token;

    void eat(TokenType expected)
    {
        if (token.type == expected)
            token = lexer.getNextToken();
        else
            throw runtime_error("Unexpected token");
    }

    BaseNode *factor()
    {
        if (token.type == TokenType::number)
        {
            double val = stod(token.value);
            eat(TokenType::number);
            return new NumNode(val);
        }
        else if (token.type == TokenType::identifier)
        {
            string name = token.value;
            eat(TokenType::identifier);
            return new VarNode(name);
        }
        else if (token.type == TokenType::lparen)
        {
            eat(TokenType::lparen);
            BaseNode *node = expr();
            eat(TokenType::rparen);
            return node;
        }
        throw runtime_error("Invalid factor");
    }

    BaseNode *term()
    {
        BaseNode *node = factor();
        while (token.type == TokenType::mul || token.type == TokenType::div || token.type == TokenType::mod)
        {
            Token oper = token;
            if (oper.type == TokenType::mul)
                eat(TokenType::mul);
            else if (oper.type == TokenType::div)
                eat(TokenType::div);
            else
                eat(TokenType::mod);
            node = new BinOpNode(node, oper, factor());
        }
        return node;
    }

    BaseNode *expr()
    {
        BaseNode *node = term();
        while (token.type == TokenType::plus || token.type == TokenType::minus)
        {
            Token oper = token;
            if (oper.type == TokenType::plus)
                eat(TokenType::plus);
            else
                eat(TokenType::minus);
            node = new BinOpNode(node, oper, term());
        }
        return node;
    }

public:
    Parser(Lexer &lex) : lexer(lex) { token = lexer.getNextToken(); }

    BaseNode *statement()
    {
        if (token.type == TokenType::identifier)
        {
            string varName = token.value;
            eat(TokenType::identifier);
            if (token.type == TokenType::assign)
            {
                eat(TokenType::assign);
                BaseNode *rhs = expr();
                return new AssignNode(varName, rhs);
            }
            throw runtime_error("Expected '=' after variable");
        }
        return expr();
    }

    BaseNode *parse() { return statement(); }
};
class Value
{
public:
    double val;
    Value(double v = 0) : val(v) {}

    Value operator+(const Value &other) const { return Value(val + other.val); }
    Value operator-(const Value &other) const { return Value(val - other.val); }
    Value operator*(const Value &other) const { return Value(val * other.val); }
    Value operator/(const Value &other) const
    {
        if (other.val == 0){
            throw runtime_error("Division by zero");
        }
        return Value(val / other.val);
    }

    Value mod(const Value &other) const
    {
        if (other.val == 0){
            throw runtime_error("Modulo by zero");
        }
        if (val == (int)val && other.val == (int)other.val){
            return Value((int)val % (int)other.val);
        }
        else{
            throw runtime_error("Modulus only supported for integers");
        }
    }

    void display() const
    {
        if (val == (int)val){
            cout << (int)val;
        }
        else{
            cout << val;
        }
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

int main()
{
    Interpreter interp;
    while (true)
    {
        cout << ">>> ";
        string s;
        if (!getline(cin, s) || s.empty())
            break;

        Lexer lexer(s);
        Parser parser(lexer);
        BaseNode *tree = parser.parse();

        try
        {
            Value result = interp.visit(tree);
            cout << "Result = ";
            result.display();
            cout << "\n";
        }
        catch (exception &e)
        {
            cout << "Error: " << e.what() << "\n";
        }

        delete tree;
    }

    return 0;
}
