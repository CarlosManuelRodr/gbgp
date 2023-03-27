#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <utility>
#include "../include/gbgp.h"
namespace py = pybind11;
using namespace std;
using namespace gbgp;


//*****************************
//*          Aliases          *
//****************************/

// Factory alias for
// <const NonTerminal&, const std::vector<ProductionElement>&, int> with a python list.
ProductionRule productionRuleAlias3ArgsSI(const NonTerminal& from, const py::list& to, int semanticTransferIndex)
{
    std::vector<ProductionElement> toVector;
    toVector.reserve(to.size());

    for (auto& toItem : to)
    {
        if (py::isinstance<Terminal>(toItem))
            toVector.emplace_back(toItem.cast<Terminal>());
        else if (py::isinstance<NonTerminal>(toItem))
            toVector.emplace_back(toItem.cast<NonTerminal>());
        else if (py::isinstance<py::str>(toItem))
            toVector.emplace_back(toItem.cast<py::str>());
    }

    return {from, toVector, semanticTransferIndex};
}

// Factory alias for
// <const NonTerminal&, const std::vector<ProductionElement>&, std::function<void(EvaluationContext&)>>
// with a python list.
ProductionRule productionRuleAlias3ArgsSA(const NonTerminal& from, const py::list& to,
                                          std::function<void(EvaluationContext&)> semanticAction)
{
    std::vector<ProductionElement> toVector;
    toVector.reserve(to.size());

    for (auto& toItem : to)
    {
        if (py::isinstance<Terminal>(toItem))
            toVector.emplace_back(toItem.cast<Terminal>());
        else if (py::isinstance<NonTerminal>(toItem))
            toVector.emplace_back(toItem.cast<NonTerminal>());
        else if (py::isinstance<py::str>(toItem))
            toVector.emplace_back(toItem.cast<py::str>());
    }

    return {from, toVector, std::move(semanticAction)};
}

// Factory alias for
// <const NonTerminal&, const std::vector<ProductionElement>&> with a python list.
ProductionRule productionRuleAlias2Args(const NonTerminal& from, const py::list& to)
{
    return productionRuleAlias3ArgsSI(from, to, 0);
}

//*****************************
//*         Bindings          *
//****************************/

PYBIND11_MODULE(gbgp, m) {
    m.doc() = "gbgp python bindings"; // optional module docstring

    py::class_<Terminal>(m, "Terminal")
            .def(py::init<>())
            .def(py::init<const std::string&>(), py::arg("value"))
            .def(py::init<int, const std::string&>(), py::arg("pid"), py::arg("plabel"))
            .def(py::init<int, const std::string&, const std::vector<std::string>&>(), py::arg("pid"), py::arg("plabel"), py::arg("pvalues"))
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("IsMutable", &Terminal::IsMutable, "Does this terminal have multiple possible values?")
            .def("GetRandomValue", &Terminal::GetRandomValue, "Get a random value of the set of possible values.")
            .def("ToString", &Terminal::ToString, "Get the string representation.")
            .def_readwrite("id", &Terminal::id, "The term type.")
            .def_readwrite("label", &Terminal::label, "The label used visual representation and debugging.")
            .def_readwrite("values", &Terminal::values, "List of possible terminal values that a node of this type can contain.")
            .def("__repr__",
                 [](const Terminal& t) {
                     return t.ToString();
                 }
            )
            .def(py::pickle(
                    [](const Terminal& t) { // __getstate__
                        /* Return a tuple that fully encodes the state of the object */
                        return py::make_tuple(t.id, t.label, t.values);
                    },
                    [](const py::tuple& t) { // __setstate__
                        if (t.size() != 3)
                            throw std::runtime_error("Invalid state.");

                        /* Create a new C++ instance */
                        Terminal p(t[0].cast<int>(), t[1].cast<std::string>(), t[2].cast<std::vector<std::string>>());
                        return p;
                    }
            ));

    py::class_<NonTerminal>(m, "NonTerminal")
            .def(py::init<>())
            .def(py::init<int, const std::string&>(), py::arg("pid"), py::arg("plabel"))
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("ToString", &NonTerminal::ToString, "Get the string representation.")
            .def_readwrite("id", &NonTerminal::id, "The term type.")
            .def_readwrite("label", &NonTerminal::label, "The label used visual representation and debugging.")
            .def("__repr__",
                 [](const NonTerminal& nt) {
                     return nt.ToString();
                 }
            )
            .def(py::pickle(
                    [](const NonTerminal& nt) { // __getstate__
                        /* Return a tuple that fully encodes the state of the object */
                        return py::make_tuple(nt.id, nt.label);
                    },
                    [](const py::tuple& t) { // __setstate__
                        if (t.size() != 2)
                            throw std::runtime_error("Invalid state.");

                        /* Create a new C++ instance */
                        NonTerminal p(t[0].cast<int>(), t[1].cast<std::string>());
                        return p;
                    }
            ));

    py::class_<EvaluationContext>(m, "EvaluationContext", py::dynamic_attr())
            .def(py::init<>())
            .def("GetResult", &EvaluationContext::GetResult, "Get the string result.")
            .def("SetResult", &EvaluationContext::SetResult, "Set a string result.", py::arg("s"))
            .def("SemanticValue", &EvaluationContext::SemanticValue, "Get semantic value of the associated ProductionRule at the specified index.", py::arg("index"))
            .def("GetSemanticValues", &EvaluationContext::GetSemanticValues, "Get the list of semantic values.")
            .def("PushSemanticValue", &EvaluationContext::PushSemanticValue, "Push the evaluation result of a ProductionRule for later use inside a semantic action.", py::arg("value"))
            .def("NumberOfSemanticValues", &EvaluationContext::NumberOfSemanticValues, "Get the total number of semantic values.")
            .def("TransferSemanticValueToResult", &EvaluationContext::TransferSemanticValueToResult, "Transfer the semantic value at the specified index to the result.", py::arg("index") = 0)
            .def("ToString", &EvaluationContext::ToString, "Get a string representation.")
            .def("__repr__",
                 [](const EvaluationContext& ctx) {
                     return ctx.ToString();
                 }
            );

    py::enum_<ProductionElementType>(m, "ProductionElementType")
            .value("Unassigned", ProductionElementType::Unassigned)
            .value("NonTerminal", ProductionElementType::NonTerminal)
            .value("Terminal", ProductionElementType::Terminal);

    py::class_<ProductionElement>(m, "ProductionElement")
            .def(py::init<const std::string&>(), "Terminal constructor with unique value.", py::arg("value"))
            .def(py::init<const Terminal&>(), "Terminal element constructor.", py::arg("pterm"))
            .def(py::init<const NonTerminal&>(), "NonTerminal element constructor.", py::arg("pnonterm"))
            .def(py::init<int, const std::string&, const std::vector<std::string>&>(), "Terminal constructor with various possible values.", py::arg("id"), py::arg("label"), py::arg("values"))
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("GetType", &ProductionElement::GetType, "Get the production element type.")
            .def("GetTypeStr", &ProductionElement::GetTypeStr, "Get production element type as a string.")
            .def("GetValue", &ProductionElement::GetValue, "Get the label value of the production element.")
            .def("ToString", &ProductionElement::ToString, "Get a string representation.")
            .def("__repr__",
                 [](const ProductionElement& pe) {
                     return pe.ToString();
                 }
            )
            .def(py::pickle(
                    [](const ProductionElement& pe) { // __getstate__
                        /* Return a tuple that fully encodes the state of the object */
                        return py::make_tuple(pe.type, pe.term, pe.nonterm);
                    },
                    [](const py::tuple& t) { // __setstate__
                        if (t.size() != 3)
                            throw std::runtime_error("Invalid state.");

                        /* Create a new C++ instance */
                        ProductionElement pe(t[0].cast<ProductionElementType>(), t[1].cast<Terminal>(), t[2].cast<NonTerminal>());
                        return pe;
                    }
            ));

    py::class_<ProductionRule>(m, "ProductionRule")
            .def(py::init<>(), "Empty ProductionRule constructor.")
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&>(), "Production rule with default semantic action.", py::arg("pfrom"), py::arg("pto"))
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&, std::function<void(EvaluationContext&)>>(), "Production rule with custom semantic action.", py::arg("pfrom"), py::arg("pto"), py::arg("pSemanticAction"))
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&, int>(), "Production rule with default transfer semantic action over element at index semanticTransferIndex.", py::arg("pfrom"), py::arg("pto"), py::arg("semanticTransferIndex"))
            .def(py::init(&productionRuleAlias2Args), "Python array constructor alias with two arguments.")
            .def(py::init(&productionRuleAlias3ArgsSI), "Python array constructor alias with three arguments and a semantic transfer index.")
            .def(py::init(&productionRuleAlias3ArgsSA), "Python array constructor alias with three arguments and a semantic action.")
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("NumberOfProductionElements", &ProductionRule::NumberOfProductionElements, "Returns the number of production elements.")
            .def("GetFrom", &ProductionRule::GetFrom, "Getter for the from list.")
            .def("GetTo", &ProductionRule::GetTo, "Getter for the to list.")
            .def("ToString", &ProductionRule::ToString, "Get a string representation.")
            .def("__repr__",
                 [](const ProductionRule& pr)
                 {
                     return pr.ToString();
                 }
            )
            .def(py::pickle(
                    [](const ProductionRule& pr)
                    { // __getstate__
                        /* Return a tuple that fully encodes the state of the object */
                        return py::make_tuple(pr.from, pr.to);
                    },
                    [](const py::tuple& t)
                    { // __setstate__
                        if (t.size() != 2)
                            throw std::runtime_error("Invalid state!");

                        /* Create a new C++ instance */
                        ProductionRule pr(t[0].cast<NonTerminal>(), t[1].cast<std::vector<ProductionElement>>(), nullptr);
                        return pr;
                    }
            ));

    py::enum_<NodeType>(m, "NodeType")
            .value("None", NodeType::None)
            .value("NonTerminal", NodeType::NonTerminal)
            .value("Terminal", NodeType::Terminal);

    py::class_<Node>(m, "Node")
            .def(py::init<>(), "Constructor of an empty node.")
            .def(py::init<const Terminal&>(), "Terminal tree node constructor.", py::arg("t"))
            .def(py::init<const Terminal&, const std::string&>(), "Terminal tree node with value constructor.", py::arg("t"), py::arg("value"))
            .def(py::init<const NonTerminal&>(), "NonTerminal tree node constructor.", py::arg("nt"))
            .def(py::init<const ProductionRule&, const NonTerminal&>(), "NonTerminal tree node constructor with generator production rule.", py::arg("productionRule"), py::arg("nt"))
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("GetValue", &Node::GetValue, "Returns the value of the node.")
            .def("GetLabel", &Node::GetLabel, "Returns a formatted label of the node.")
            .def("ToString", &Node::ToString, "Get node representation as string.")
            .def("__repr__",
                 [](const Node& node)
                 {
                     return node.ToString();
                 }
            )
            .def(py::pickle(
                    [](const Node& node)
                    { // __getstate__
                        /* Return a tuple that fully encodes the state of the object */
                        return py::make_tuple(node.type, node.nonTermInstance, node.termInstance, node.generatorPR, node.termValue);
                    },
                    [](const py::tuple& t)
                    { // __setstate__
                        if (t.size() != 5)
                            throw std::runtime_error("Invalid state!");

                        /* Create a new C++ instance */
                        auto type = t[0].cast<NodeType>();
                        auto nonTermInstance = t[1].cast<NonTerminal>();
                        auto termInstance = t[2].cast<Terminal>();
                        auto generatorPR = t[3].cast<ProductionRule>();
                        auto termValue = t[4].cast<std::string>();
                        Node node(type, nonTermInstance, termInstance, generatorPR, termValue);
                        return node;
                    }
            ));

    py::class_<TreeNode, Node>(m, "TreeNode")
            .def(py::init<>(), "Constructor of an empty tree node.")
            .def(py::init<const TreeNode&>(), "Copy constructor that copies all linked nodes.", py::arg("other"))
            .def(py::init<const Terminal&>(), "Terminal tree node constructor.", py::arg("t"))
            .def(py::init<const Terminal&, const std::string&>(), "Terminal tree node with value constructor.", py::arg("t"), py::arg("value"))
            .def(py::init<const NonTerminal&>(), "NonTerminal tree node constructor.", py::arg("nt"))
            .def(py::init<const NonTerminal&, const std::vector<TreeNode>&>(), "NonTerminal tree node constructor with explicit children instances.", py::arg("nt"), py::arg("children"))
            .def(py::init<const ProductionRule&, const NonTerminal&>(), "NonTerminal tree node constructor with generator production rule.", py::arg("productionRule"), py::arg("nt"))
            .def(py::init<const ProductionRule&, const NonTerminal&, const std::vector<TreeNode>&>(), "NonTerminal tree node constructor that takes TreeNode instances as children and uses them as a blueprint to construct its own children nodes.",py::arg("productionRule"), py::arg("nt"), py::arg("children"))
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("SameID", &TreeNode::SameID, "Check if both nodes have the same term ID.", py::arg("other"))
            .def("AddChildNode", &TreeNode::AddChildNode, "Add node as a child.", py::arg("node"))
            .def("AddChildTerm", py::overload_cast<const NonTerminal&, const ProductionRule&>(&TreeNode::AddChildTerm), py::return_value_policy::reference, "Add child NonTerminal node to the target.", py::arg("nonTerm"), py::arg("generatorPR"))
            .def("AddChildTerm", py::overload_cast<const Terminal&>(&TreeNode::AddChildTerm), py::return_value_policy::reference, "Add child Terminal node to the target. If the Terminal has only one possible value, sets it as the termValue.", py::arg("term"))
            .def("AddChildTerm", py::overload_cast<const Terminal&, const std::string&>(&TreeNode::AddChildTerm), py::return_value_policy::reference, "Add child Terminal node to the target.", py::arg("term"), py::arg("ptermValue"))
            .def("ClearSynthesis", &TreeNode::ClearSynthesis, "Reset the synthesis of this node.")
            .def("IsSynthesized", &TreeNode::IsSynthesized, "Check if this node has been synthesized.")
            .def("ClearEvaluation", &TreeNode::ClearEvaluation, "Reset the evaluation of this node.")
            .def("IsEvaluated", &TreeNode::IsEvaluated, "Check if this node has been evaluated.")
            .def("HasChildren", &TreeNode::HasChildren, "Check if this node has children.")
            .def("ToString", &TreeNode::ToString, "Get tree node representation as string.")
            .def("__repr__",
                 [](const TreeNode& treeNode)
                 {
                     return treeNode.ToString();
                 }
            );

    py::class_<Graph>(m, "Graph")
            .def(py::init<>(), "Creates an empty Graph.")
            .def(py::init<const std::vector<Node>&, const std::vector<std::pair<int, int>>&>(), "Graph constructor.")
            .def("GetNodes", &Graph::GetNodes, "Getter for the nodes.")
            .def("GetNodeIndexes", &Graph::GetNodeIndexes, "Get the indexes of the nodes.")
            .def("GetEdges", &Graph::GetEdges, "Getter for the edges.")
            .def("GetLabels", &Graph::GetLabels, "Get the labels as a map.")
            .def("GetTreeNodes", &Graph::GetTreeNodes, "Rebuild the TreeNode pointer chain from this graph.")
            .def("ToString", &Graph::ToString, "Get string representation.")
            .def("__repr__",
                 [](const Graph& graph)
                 {
                     return graph.ToString();
                 }
            )
            ;

    py::class_<SyntaxTree>(m, "SyntaxTree")
            .def(py::init<>(), "Creates an empty SyntaxTree.")
            .def(py::init<const TreeNode&>(), "Builds a tree from a root node.", py::arg("root"))
            .def(py::init<const SyntaxTree&>(), "Copy constructor.", py::arg("other"))
            .def(py::init<const Graph&>(), "Graph constructor.", py::arg("graph"))
            .def_static("CopySubtree", &SyntaxTree::CopySubtree, "Get subtree starting from subTreeStartNode.", py::arg("subTreeStartNode"))
            .def("Root", &SyntaxTree::Root, "Returns a reference to the root.", py::return_value_policy::reference)
            .def("SetRoot", &SyntaxTree::SetRoot, "Set the root node.", py::arg("rootNode"))
            .def("GetRoot", &SyntaxTree::GetRoot, "Get the root node instance.")
            .def("DeleteSubtree", &SyntaxTree::DeleteSubtree, "Removes the subtree starting from rootOfSubtree.", py::arg("rootOfSubtree"))
            .def("InsertSubtree", py::overload_cast<TreeNode*, TreeNode*>(&SyntaxTree::InsertSubtree), "Insert a copy of the subtree into the position at insertNode.", py::arg("insertNode"), py::arg("subtreeStartNode"))
            .def("InsertSubtree", py::overload_cast<TreeNode*, const SyntaxTree&>(&SyntaxTree::InsertSubtree), "Insert a copy of the subtree into the position at insertNode.", py::arg("insertNode"), py::arg("subtree"))
            .def("IsEmpty", &SyntaxTree::IsEmpty, "Check if the tree is empty.")
            .def("SetRootRule", &SyntaxTree::SetRootRule, "Set the production rule of the root node.", py::arg("startRule"))
            .def("ToString", &SyntaxTree::ToString, "Get string representation.")
            .def("ToGraph", &SyntaxTree::ToGraph, "Export the tree into a graph.")
            .def("FromGraph", &SyntaxTree::FromGraph, "Build syntax tree from a graph.")
            .def("GetPostOrderTreeTraversal", &SyntaxTree::GetPostOrderTreeTraversal, "Traverses the tree in a depth first post-order.")
            .def("SynthesizeExpression", &SyntaxTree::SynthesizeExpression, "Synthesizes the tree into an expression using the production rules of the grammar.")
            .def("Evaluate", &SyntaxTree::Evaluate, "Evaluates the tree using the semantic actions of the grammar.", py::arg("ctx"))
            .def("ExternalEvaluate", &SyntaxTree::ExternalEvaluate<string>, "Evaluates the tree using an external evaluator.", py::arg("evaluator"))
            .def("__repr__",
                 [](const SyntaxTree& tree)
                 {
                     return tree.ToString();
                 }
            )
            .def(py::pickle(
                    [](const SyntaxTree& tree)
                    { // __getstate__
                        /* Return a tuple that fully encodes the state of the object */
                        Graph treeAsGraph = tree.ToGraph();
                        return py::make_tuple(treeAsGraph);
                    },
                    [](const py::tuple& t)
                    { // __setstate__
                        if (t.size() != 1)
                            throw std::runtime_error("Invalid state!");

                        /* Create a new C++ instance */
                        auto deserializedGraph = t[0].cast<Graph>();
                        SyntaxTree tree(deserializedGraph);
                        return tree;
                    }
            ));

    py::class_<PruneRule>(m, "PruneRule")
            .def(py::init<const SyntaxTree&, const SyntaxTree&>(), "Constructor by SyntaxTree.")
            .def("CanBeApplied", &PruneRule::CanBeApplied, "Does the target tree can be simplified further with this rule?", py::arg("target"))
            .def("Apply", &PruneRule::Apply, "Create a new SyntaxTree where the prune rule has been applied.")
            .def("ToString", &PruneRule::ToString, "Get string representation.")
            .def("__repr__",
                 [](const PruneRule& pruneRule) {
                     return pruneRule.ToString();
                 }
            );

    py::class_<Grammar>(m, "Grammar")
            .def(py::init<>(), "Empty constructor.")
            .def(py::init<std::initializer_list<ProductionRule>>(), "Only production rules initializer list constructor.")
            .def(py::init<std::initializer_list<ProductionRule>, std::initializer_list<PruneRule>>(), "Initializer list constructor.")
            .def(py::init<const Grammar&>(), "Copy constructor.")
            .def(py::init<const std::vector<ProductionRule>&>(), "Only production rules constructor.")
            .def(py::init<const std::vector<ProductionRule>&, const std::vector<PruneRule>&>(), "Parameter by parameter constructor.")

            .def("GetRootRule", &Grammar::GetRootRule, "Get the root rule of the grammar.")
            .def("Size", &Grammar::Size, "Get the number of production rules of this grammar.")
            .def("CreateRandomTree", py::overload_cast<SyntaxTree&>(&Grammar::CreateRandomTree, py::const_), "Create random tree safely by creating random trees until there is a success.", py::arg("syntaxTree"))
            .def("CreateRandomTree", py::overload_cast<SyntaxTree&, int>(&Grammar::CreateRandomTree, py::const_), "Create random tree safely by creating random trees until there is a success.", py::arg("syntaxTree"), py::arg("maxDepth"))
            .def("PruneTree", &Grammar::PruneTree, " Applies the grammar prune rules repeatedly until no further simplification can be performed.", py::arg("syntaxTree"))
            .def("ToString", &Grammar::ToString, "Get string representation.")
            .def("RestoreSemanticAction", py::overload_cast<ProductionRule&>(&Grammar::RestoreSemanticAction, py::const_), "Restore the appropriate semantic action of the production rule.", py::arg("target"))
            .def("RestoreSemanticAction", py::overload_cast<Node&>(&Grammar::RestoreSemanticAction, py::const_), "Restore the appropriate semantic action of the production rule.", py::arg("target"))
            .def("RestoreSemanticAction", py::overload_cast<Graph&>(&Grammar::RestoreSemanticAction, py::const_), "Restore the appropriate semantic action of the production rule.", py::arg("target"))
            .def("__repr__",
                 [](const Grammar& grammar) {
                     return grammar.ToString();
                 }
            );

    py::class_<Individual>(m, "Individual")
            .def(py::init<>(), "Empty constructor.")
            .def(py::init<const std::function<double(SyntaxTree&)>&>(), "Blank individual constructor.", py::arg("fitnessFunction"))
            .def(py::init<const std::function<double(SyntaxTree&)>&, const SyntaxTree&>(), "Constructor for already built syntax tree.", py::arg("fitnessFunction"), py::arg("syntaxTree"))
            .def(py::init<const Individual&>(), "Copy constructor.", py::arg("other"))

            .def("GetFitnessFunction", &Individual::GetFitnessFunction, "Fitness function getter.")
            .def("GetTree", &Individual::GetTree, "Returns a reference to the syntax tree.")
            .def("GetExpression", &Individual::GetExpression, "Synthesizes the tree expression.")
            .def("IsEvaluated", &Individual::IsEvaluated, "Is this individual evaluated?")
            .def("GetFitness", &Individual::GetFitness, "Return the fitness value.")
            .def("Evaluate", &Individual::Evaluate, "Evaluates the fitness function and assign the fitness value.")
            .def("Prune", &Individual::Prune, "Prunes the tree.", py::arg("grammar"))
            .def("CreateRandom", &Individual::CreateRandom, "Generates a random individual using the production rules and prune rules of the grammar.", py::arg("grammar"))
            .def("__repr__",
                 [](const Individual& individual) {
                     return individual.ToString();
                 }
            )
            ;

    py::class_<Population>(m, "Population")
            .def(py::init<const Grammar&, const std::function<double(SyntaxTree&)>&>(), "Population constructor.", py::arg("grammar"), py::arg("fitnessFunction"))

            .def("Initialize", &Population::Initialize, "Initializes a population of randomly generated individuals.", py::arg("populationSize"))
            .def("AddIndividual", &Population::AddIndividual, "Add an individual to the population.", py::arg("individual"))
            .def("AddIndividuals", &Population::AddIndividuals, "Add a collection of individuals to the population.", py::arg("newIndividuals"))
            .def("GetIndividual", &Population::GetIndividual, "Get the individual at the n-th index.", py::arg("n"))
            .def("GetFittestByRank", &Population::GetFittestByRank, "Get the fittest individual by rank.", py::arg("rank"))
            .def("GetNthFittestByRank", &Population::GetNthFittestByRank, "Get the fittest individuals up to rank maxRank.", py::arg("maxRank"))
            .def("ReducePopulation", &Population::ReducePopulation, "Reduce the population to the selected indexes.", py::arg("keepIndexes"))
            .def("RemoveWorst", &Population::RemoveWorst, "Removes the n-th worst individuals by fitness of the population.", py::arg("maxRank"))
            .def("Prune", &Population::Prune, "Prune all the individuals of the population.")
            .def("Evaluate", py::overload_cast<>(&Population::Evaluate), "Evaluate all the individuals of the population.")
            .def("GetFitness", &Population::GetFitness, "Get the fitness values of all the individuals in the population.")
            .def("GetIndividuals", &Population::GetIndividuals, "Get a reference to the individuals of the population.")
            .def("Size", &Population::Size, "Get the size of the population.")
            .def("GetGeneratingGrammar", &Population::GetGeneratingGrammar, "Grammar getter.")
            .def("GetFitnessFunction", &Population::GetFitnessFunction, "Fitness function getter.")
            .def("__repr__",
                 [](const Population& population) {
                     return population.ToString();
                 }
            )
            ;

    py::class_<GeneticOperators>(m, "GeneticOperators")
            .def_static("IndividualsCrossover", &GeneticOperators::IndividualsCrossover, "Generates a new offspring individual that combines genome features from both parents.", py::arg("parent1"), py::arg("parent2"))
            .def_static("Selection", &GeneticOperators::Selection, "The selection operator. Reduces the population to its fittest individuals.", py::arg("population"), py::arg("size"))
            .def_static("Crossover", py::overload_cast<Population&>(&GeneticOperators::Crossover), "The crossover operator. Creates a new generation by reproduction of the individuals.", py::arg("population"))
            .def_static("Crossover", py::overload_cast<Population&, unsigned>(&GeneticOperators::Crossover), "The crossover operator. Creates a new generation by reproduction of the individuals.", py::arg("population"), py::arg("offspringSize"))
            .def_static("MutateIndividual", py::overload_cast<Individual&, const Grammar&>(&GeneticOperators::MutateIndividual), "Mutation operator that acts over an individual with default mutation probability of 50%.", py::arg("individual"), py::arg("grammar"))
            .def_static("MutateIndividual", py::overload_cast<Individual&, const Grammar&, double>(&GeneticOperators::MutateIndividual), "Mutation operator that acts over an individual.", py::arg("individual"), py::arg("grammar"), py::arg("nonTermMutationProb"))
            .def_static("Mutation", py::overload_cast<Population&, double>(&GeneticOperators::Mutation), "Mutation operator that acts over a population.", py::arg("population"), py::arg("mutationProbability"))
            .def_static("Mutation", py::overload_cast<Population&, double, double>(&GeneticOperators::Mutation), "Mutation operator that acts over a population.", py::arg("population"), py::arg("mutationProbability"), py::arg("nonTermMutationProbability"))
            ;

    py::class_<Environment>(m, "Environment")
            .def(py::init<const Grammar&, const std::function<double(SyntaxTree&)>&, int, int, int, int, double>(), "Environment constructor.", py::arg("grammar"), py::arg("fitnessFunction"), py::arg("populationSize"), py::arg("survivorsPerGeneration"), py::arg("eliteIndividuals"), py::arg("immigrationIndividuals"), py::arg("mutationProbability"))
            .def("GetPopulation", &Environment::GetPopulation, "Population getter.")
            .def("Optimize", py::overload_cast<>(&Environment::Optimize), "Optimizes a population via genetic optimization.")
            .def("Optimize", py::overload_cast<unsigned>(&Environment::Optimize), "Optimizes a population via genetic optimization.", py::arg("generations"))
            .def("__repr__",
                 [](const Environment& environment) {
                     return environment.ToString();
                 }
            )
            ;
}