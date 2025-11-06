```mermaid
%%{init: {'theme': 'default', 'logLevel': 'fatal', 'gitGraph': {'rotateCommitLabel': false}}}%%
usecase-diagram
actor User as "User (Programmer / End User)"

rectangle "Interpreter System" {
    (Enter Expression / Statement) as UC1
    (Lexical Analysis / Tokenization) as UC2
    (Parsing / Build AST) as UC3
    (Interpretation / Evaluate AST) as UC4
    (Variable Management) as UC5
    (Display Output) as UC6
    (Error Handling) as UC7
}

%% Lexer Use Cases
(Recognize Numbers) as L2
(Recognize Identifiers) as L3
(Recognize Operators) as L4
(Recognize Parentheses) as L5
(Skip Whitespace) as L1

%% Parser Use Cases
(Parse Factors) as P1
(Parse Terms) as P2
(Parse Expressions) as P3
(Parse Assignment Statements) as P4
(Build AST Nodes) as P5

%% Interpreter Use Cases
(Visit Number Nodes) as I1
(Visit Variable Nodes) as I2
(Visit Binary Operation Nodes) as I3
(Visit Assignment Nodes) as I4
(Store Variable Values) as I5
(Check Division/Modulo by Zero) as I6

%% Relationships
User --> UC1
UC1 --> UC2
UC2 --> UC3
UC3 --> UC4
UC4 --> UC6
UC4 --> UC7
UC4 --> UC5
User --> UC6
UC1 --> UC7

%% Internal Connections
UC2 --> L1
UC2 --> L2
UC2 --> L3
UC2 --> L4
UC2 --> L5

UC3 --> P1
UC3 --> P2
UC3 --> P3
UC3 --> P4
UC3 --> P5

UC4 --> I1
UC4 --> I2
UC4 --> I3
UC4 --> I4
UC4 --> I5
UC4 --> I6

```
