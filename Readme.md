# GBGP++
A header-only C++ library to perform grammar-based genetic programming optimization.

## A gentle introduction
### What the hell is grammar-based genetic programming optimization?
Despite the pompous name the idea is dead simple. Take a problem that can be solved via a domain specific language (see the example ), encode the language into a grammar that generates valid solutions, set-up a target function to optimize and let the system approximate a solution by applying the rules of natural selection.

### Care to elaborate? The simplest example
Let's say I give you the following set of points in the form {x, y, f(x,y)}:
```
{{0,0,1},{0,1,2},{0,2,9},{0,3,28},{0,4,65},{0,5,126},{0,6,217},{0,7,344},{0,8,513},{0,9,730},{0,10,1001},{1,0,3},{1,1,4},{1,2,11},{1,3,30},{1,4,67},{1,5,128},{1,6,219},{1,7,346},{1,8,515},{1,9,732},{1,10,1003},{2,0,5},{2,1,6},{2,2,13},{2,3,32},{2,4,69},{2,5,130},{2,6,221},{2,7,348},{2,8,517},{2,9,734},{2,10,1005},{3,0,7},{3,1,8},{3,2,15},{3,3,34},{3,4,71},{3,5,132},{3,6,223},{3,7,350},{3,8,519},{3,9,736},{3,10,1007},{4,0,9},{4,1,10},{4,2,17},{4,3,36},{4,4,73},{4,5,134},{4,6,225},{4,7,352},{4,8,521},{4,9,738},{4,10,1009},{5,0,11},{5,1,12},{5,2,19},{5,3,38},{5,4,75},{5,5,136},{5,6,227},{5,7,354},{5,8,523},{5,9,740},{5,10,1011},{6,0,13},{6,1,14},{6,2,21},{6,3,40},{6,4,77},{6,5,138},{6,6,229},{6,7,356},{6,8,525},{6,9,742},{6,10,1013},{7,0,15},{7,1,16},{7,2,23},{7,3,42},{7,4,79},{7,5,140},{7,6,231},{7,7,358},{7,8,527},{7,9,744},{7,10,1015},{8,0,17},{8,1,18},{8,2,25},{8,3,44},{8,4,81},{8,5,142},{8,6,233},{8,7,360},{8,8,529},{8,9,746},{8,10,1017},{9,0,19},{9,1,20},{9,2,27},{9,3,46},{9,4,83},{9,5,144},{9,6,235},{9,7,362},{9,8,531},{9,9,748},{9,10,1019},{10,0,21},{10,1,22},{10,2,29},{10,3,48},{10,4,85},{10,5,146},{10,6,237},{10,7,364},{10,8,533},{10,9,750},{10,10,1021}}
```
and ask you to tell me which expression f(x,y) generated them. A simple solution would be to just go to WolframAlpha, write those points in the query and let it find the answer for you. But sadly, doing that is lame and you won't be able to brag about it. So alternatively you could use gbgp++!

First let's define a grammar for aritmethic operations. Here we set a special set of rules called "production rules":

```
Rule 1: EXPR -> EXPR Plus TERM
Rule 2: EXPR -> TERM
Rule 3: TERM -> TERM Times FACTOR
Rule 4: TERM -> FACTOR
Rule 5: FACTOR -> LeftParenthesis EXPR RightParenthesis
Rule 6: FACTOR -> var
```

Each production rule is a recipe for generating children terms for a non-terminal term. Non-Terminals are differenciated by the UPPER_CASE name and are terms that have other terms as children. The lowerCase terms ara caled Terminal terms, have no children and contain a value.

The values that terminals such as `var` may have, called the "terminal set" are the following:

```
var: x, y, 1
Plus: +
Times: *
LeftParenthesis: (
RightParenthesis: )
```

Starting from a root Non-Terminal such as EXPR, any arithmetic expression based on the sum +, multiplication +, and grouping () the var terms x, y and 1 can be constructed. For example, the arithmetic expression `x+x+1+(1+y*y)` is represented by the following tree that results from a particular order of applying the production rules.

```
|EXPR
|-EXPR
|--EXPR
|---EXPR
|----TERM
|-----FACTOR
|------var [x]
|---Plus [+]
|---TERM
|----FACTOR
|-----var [x]
|--Plus [+]
|--TERM
|---FACTOR
|----var [1]
|-Plus [+]
|-TERM
|--FACTOR
|---LeftParenthesis [(]
|---EXPR
|----EXPR
|-----TERM
|------FACTOR
|-------var [1]
|----Plus [+]
|----TERM
|-----TERM
|------FACTOR
|-------var [y]
|-----Times [*]
|-----FACTOR
|------var [y]
|---RightParenthesis [)]
```

A grammar is defined by the set of production rules and the terminal set and is the object that defines the search space of the problem to be solved (in this case a space of arithmetic expressions).

Next we have to do is to define a fitness function that will tell us how good or bad is each solution. For this simple example let's say that f'(x,y) is a solution and define the error as

```
ERROR = 1/100 \sum_{x=0}^{10} \sum_{y=0}^{10} |f(x,y) - f'(x,y)|
```

and transform this error into a value in the range [0,1] by doing
```
FITNESS = 1 / (1 + ERROR)
```

With a grammar and a fitness function we can create a population of individuals where each represent a candidate solution and then subject them to the rules of natural selection!

![GA Rules](https://github.com/CarlosManuelRodr/gbgp/raw/main/img/GeneticAlgorithmsDiagram.png)

By repeating these steps we obtain a solution such as `(x+x)*1*1+(1+(y*y)*y)` that when factored looks like `1+2x+y^3`. Isn't it amazing? If you are not surprised yet wait until you see how easy is to implement it with GBGP++!

### Implementing the example

#### Terms definition
In GBGP++ we start by defining the Terminals and NonTerminals of the grammar. We turn the terminal set

```
var: x, y, 1
Plus: +
Times: *
LeftParenthesis: (
RightParenthesis: )
```

and the non-terminal set `EXPR, TERM, FACTOR` into the declarations:

```
enum Terms
{
    Var, Plus, Times, LeftParenthesis, RightParenthesis, // Terminals
    Expr, Term, Factor // NonTerminals
};

// Term/Nonterm declaration.
const Terminal varTerm(Var, "var", { "x", "y", "1" });
const Terminal plusTerm(Plus, "Plus", { "+" });
const Terminal timesTerm(Times, "Times", { "*" });
const Terminal leftParenthesisTerm(LeftParenthesis, "LeftParenthesis", { "(" });
const Terminal rightParenthesisTerm(RightParenthesis, "RightParenthesis", { ")" });

const NonTerminal exprNonTerm(Expr, "EXPR");
const NonTerminal termNonTerm(Term, "TERM");
const NonTerminal factorNonTerm(Factor, "FACTOR");
```

#### Production rules
Then we can turn a production rule of the grammar, lets say, Rule 1:

```
Rule 1: EXPR -> EXPR Plus TERM
```

into a GBGP++ ProductionRule:

```
const ProductionRule rule1(
        exprNonTerm, // The parent NonTerminal
        {       // The children terms
                ProductionElement(exprNonTerm),
                ProductionElement(plusTerm),
                ProductionElement(termNonTerm)
        },
        {       // Define how this rule will be displayed
                SemanticElement(exprNonTerm),
                SemanticElement(plusTerm),
                SemanticElement(termNonTerm)
        },
        // Semantic action
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(2));
            ctx->result() = std::to_string(n1 + n2);
        }
);
```

This rule says that a `exprNonTerm` may have the sequence `exprNonTerm`, `plusTerm` and `termNonTerm` as children. Notice the apparent duplication of the argument list with one being of the ProductionElement type and another a SemanticElement. The difference between each is subtle: The list of `ProductionElement` are the rules for internally constructing the expression and the list of `SemanticElement` are the rules for visualizing the expression and decorating it.

#### Semantic actions
The last argument is function called **semantic action** (here implemented as a lambda). A semantic action is a routine that will be called upon node evaluation. In this particular case, this rule is defining the sum operation, thus the semantic action performs the sum of EXPR and TERM.

Notice the argument of type `EvaluationContext`. This class contains the information needed to evaluate a node based upon the values of its children, accesible via the method `SemanticValue`.

#### Rules 2-5

```
Rule 2: EXPR -> TERM
Rule 3: TERM -> TERM Times FACTOR
Rule 4: TERM -> FACTOR
Rule 5: FACTOR -> LeftParenthesis EXPR RightParenthesis
```

Based on the previous example, rules 2 to 5 are fairly straightforward to implement.

```
const ProductionRule rule2(
        exprNonTerm,
        {
                ProductionElement(termNonTerm)
        },
        {
                SemanticElement(termNonTerm)
        },
        // Transfer the value of TERM to EXPR.
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule rule3(
        termNonTerm,
        {
                ProductionElement(termNonTerm),
                ProductionElement(timesTerm),
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(termNonTerm),
                SemanticElement(timesTerm),
                SemanticElement(factorNonTerm)
        },
        // Perform multiplication.
        [](EvaluationContext* ctx) {
            int n1 = stoi(ctx->SemanticValue(0));
            int n2 = stoi(ctx->SemanticValue(2));
            ctx->result() = std::to_string(n1 * n2);
        }
);

const ProductionRule rule4(
        termNonTerm,
        {
                ProductionElement(factorNonTerm)
        },
        {
                SemanticElement(factorNonTerm)
        },
        // Transfer the value of FACTOR to TERM.
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(0);
        }
);

const ProductionRule rule5(
        factorNonTerm,
        {
                ProductionElement(leftParenthesisTerm),
                ProductionElement(exprNonTerm),
                ProductionElement(rightParenthesisTerm)
        },
        {
                SemanticElement(leftParenthesisTerm),
                SemanticElement(exprNonTerm),
                SemanticElement(rightParenthesisTerm)
        },
        // Get value of EXPR between parenthesis.
        [](EvaluationContext* ctx) {
            ctx->result() = ctx->SemanticValue(1);
        }
);
```

#### Rule 6 and custom EvaluationContext

At rule 6 we run into a small issue.
```
Rule 6: FACTOR -> var
```

Var can either be {"x", "y", "1"}, and we need a way to assign the proper value as the result. The philosophy of GBGP++ is to not get in the way of the user and let it do its thing. Let's create a custom evaluation context that inherits from `EvaluationContext` and contains the x and y integer values:

```
class ArithmeticContext : public EvaluationContext
{
public:
    int x{}, y{};

    int GetResult() { return stoi(result()); }
};
```

Using this ArithmeticContext we can write rule 6 as;

```
const ProductionRule rule6(
        factorNonTerm,
        {
                ProductionElement(varTerm),
        },
        {
                SemanticElement(varTerm)
        },
        [](EvaluationContext* ctx) {
            auto* arithmeticContext = dynamic_cast<ArithmeticContext*>(ctx); // We get access to the x and y values by casting ctx to its proper type.
            string var = ctx->SemanticValue(0);

            int varValue;
            if (var == "x")
                varValue = arithmeticContext->x;
            else if (var == "y")
                varValue = arithmeticContext->y;
            else
                varValue = 1;

            ctx->result() = to_string(varValue);
        }
);
```