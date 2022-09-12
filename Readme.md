# GBGP++
A header-only C++ library to perform grammar-based genetic programming optimization.

## A gentle introduction
### What the hell is grammar-based genetic programming optimization?
Despite the pompous name the idea is dead simple. Take a problem that can be solved via a domain specific language (see the example ), encode the language into a grammar that generates valid solutions, set-up a target function to optimize and let the system approximate a solution by applying the rules of natural selection.

### Care to elaborate? Simplest example
Let's say I give you the following set of points in the form {x, y, f(x,y)}:
```
{{0,0,1},{0,1,2},{0,2,9},{0,3,28},{0,4,65},{0,5,126},{0,6,217},{0,7,344},{0,8,513},{0,9,730},{0,10,1001},{1,0,3},{1,1,4},{1,2,11},{1,3,30},{1,4,67},{1,5,128},{1,6,219},{1,7,346},{1,8,515},{1,9,732},{1,10,1003},{2,0,5},{2,1,6},{2,2,13},{2,3,32},{2,4,69},{2,5,130},{2,6,221},{2,7,348},{2,8,517},{2,9,734},{2,10,1005},{3,0,7},{3,1,8},{3,2,15},{3,3,34},{3,4,71},{3,5,132},{3,6,223},{3,7,350},{3,8,519},{3,9,736},{3,10,1007},{4,0,9},{4,1,10},{4,2,17},{4,3,36},{4,4,73},{4,5,134},{4,6,225},{4,7,352},{4,8,521},{4,9,738},{4,10,1009},{5,0,11},{5,1,12},{5,2,19},{5,3,38},{5,4,75},{5,5,136},{5,6,227},{5,7,354},{5,8,523},{5,9,740},{5,10,1011},{6,0,13},{6,1,14},{6,2,21},{6,3,40},{6,4,77},{6,5,138},{6,6,229},{6,7,356},{6,8,525},{6,9,742},{6,10,1013},{7,0,15},{7,1,16},{7,2,23},{7,3,42},{7,4,79},{7,5,140},{7,6,231},{7,7,358},{7,8,527},{7,9,744},{7,10,1015},{8,0,17},{8,1,18},{8,2,25},{8,3,44},{8,4,81},{8,5,142},{8,6,233},{8,7,360},{8,8,529},{8,9,746},{8,10,1017},{9,0,19},{9,1,20},{9,2,27},{9,3,46},{9,4,83},{9,5,144},{9,6,235},{9,7,362},{9,8,531},{9,9,748},{9,10,1019},{10,0,21},{10,1,22},{10,2,29},{10,3,48},{10,4,85},{10,5,146},{10,6,237},{10,7,364},{10,8,533},{10,9,750},{10,10,1021}}
```
and ask you to tell me which expression f(x,y) generated them. A simple solution would be to just go to WolframAlpha, write those points in the query and let it find the answer for you. But sadly, doing that is lame and you won't be able to brag about it. So alternatively you could gbgp++!

First let's define a grammar for aritmethic operations. Here we set a special set of rules called "production rules":

```
Rule 1: EXPR -> EXPR + TERM
Rule 2: EXPR -> TERM
Rule 3: TERM -> TERM * FACTOR
Rule 4: TERM -> FACTOR
Rule 5: FACTOR -> ( EXPR )
Rule 6: FACTOR -> var
```

Each production rule is a recipe for generating children terms for a non-terminal term. Non-Terminals are differenciated by the UPPER_CASE name and are terms that have other terms as children. The lowerCase terms ara caled Terminal terms, have no children and contain a value.

The values that terminals such as `var` may have, called the "terminal set" are the following:

```
var: x, y, 1
```

Starting from a root Non-Terminal such as EXPR, any arithmetic expression based on the sum +, multiplication +, and grouping () the var terms x, y and 1 can be constructed. For example, the arithmetic expression `x+x+1+(1+y*y)` is represented by the following tree that results from a particular way of applying the production rules.

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