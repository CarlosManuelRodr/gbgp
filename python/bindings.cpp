#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include "../include/gbgp.h"
namespace py = pybind11;
using namespace std;

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
            .def("IsMutable", &Terminal::IsMutable, "Does this terminal have multiple possible values?")
            .def("GetRandomValue", &Terminal::GetRandomValue, "Get a random value of the set of possible values.")
            .def("ToString", &Terminal::ToString, "Get the string representation.")
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def_readwrite("id", &Terminal::id, "The term type.")
            .def_readwrite("label", &Terminal::label, "The label used visual representation and debugging.")
            .def_readwrite("values", &Terminal::values, "List of possible terminal values that a node of this type can contain.")
            .def("__repr__",
                 [](const Terminal &t) {
                     return "<gbgp.Terminal " + t.ToString() + ">";
                 }
            );

    py::class_<NonTerminal>(m, "NonTerminal")
            .def(py::init<>())
            .def(py::init<int, const std::string&>(), py::arg("pid"), py::arg("plabel"))
            .def("ToString", &NonTerminal::ToString, "Get the string representation.")
            .def_readwrite("id", &NonTerminal::id, "The term type.")
            .def_readwrite("label", &NonTerminal::label, "The label used visual representation and debugging.")
            .def("__repr__",
                 [](const NonTerminal &nt) {
                     return "<gbgp.NonTerminal " + nt.ToString() + ">";
                 }
            );

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
                 [](const EvaluationContext &ctx) {
                     return "<gbgp.EvaluationContext " + ctx.ToString() + ">";
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
            .def("GetType", &ProductionElement::GetType, "Get the production element type.")
            .def("GetTypeStr", &ProductionElement::GetTypeStr, "Get production element type as a string.")
            .def("GetValue", &ProductionElement::GetValue, "Get the label value of the production element.")
            .def("ToString", &ProductionElement::ToString, "Get a string representation.")
            .def("__repr__",
                 [](const ProductionElement &pe) {
                     return "<gbgp.ProductionElement " + pe.ToString() + ">";
                 }
            );

    py::class_<ProductionRule>(m, "ProductionRule")
            .def(py::init<>(), "Empty ProductionRule constructor.")
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&>(), "Production rule with default semantic action.", py::arg("pfrom"), py::arg("pto"))
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&, std::function<void(EvaluationContext&)>>(), "Production rule with custom semantic action.", py::arg("pfrom"), py::arg("pto"), py::arg("pSemanticAction"))
            .def(py::init<const NonTerminal&, const std::vector<ProductionElement>&, int>(), "Production rule with default transfer semantic action over element at index semanticTransferIndex.", py::arg("pfrom"), py::arg("pto"), py::arg("semanticTransferIndex"))
            .def("NumberOfProductionElements", &ProductionRule::NumberOfProductionElements, "Returns the number of production elements.")
            .def("GetFrom", &ProductionRule::GetFrom, "Getter for the from list.")
            .def("GetTo", &ProductionRule::GetTo, "Getter for the to list.")
            .def("ToString", &ProductionRule::ToString, "Get a string representation.")
            .def("__repr__",
                 [](const ProductionRule &pr) {
                     return "<gbgp.ProductionRule '" + pr.ToString() + "'>";
                 }
            );

    py::enum_<TreeNodeType>(m, "TreeNodeType")
            .value("None", TreeNodeType::None)
            .value("NonTerminal", TreeNodeType::NonTerminal)
            .value("Terminal", TreeNodeType::Terminal);

    py::class_<TreeNode>(m, "TreeNode")
            .def(py::init<>(), "Constructor of an empty node.")
            .def(py::init<const TreeNode&>(), "Copy constructor that copies all linked nodes.", py::arg("other"))
            .def(py::init<const Terminal&>(), "Terminal node constructor.", py::arg("t"))
            .def(py::init<const Terminal&, const std::string&>(), "Terminal node with value constructor.", py::arg("t"), py::arg("value"))
            .def(py::init<const NonTerminal&>(), "NonTerminal node constructor.", py::arg("nt"))
            .def(py::init<const NonTerminal&, const std::vector<TreeNode>&>(), "NonTerminal node constructor with explicit children instances.", py::arg("nt"), py::arg("children"))
            .def(py::init<const ProductionRule&, const NonTerminal&>(), "NonTerminal node constructor with generator production rule.", py::arg("productionRule"), py::arg("nt"))
            .def(py::init<const ProductionRule&, const NonTerminal&, const std::vector<TreeNode>&>(), "NonTerminal node constructor that takes TreeNode instances as children and uses them as a blueprint to construct its own children nodes.",py::arg("productionRule"), py::arg("nt"), py::arg("children"))
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("SameID", &TreeNode::SameID, "Check if both nodes have the same term ID.", py::arg("other"))
            .def("AddChildNode", &TreeNode::AddChildNode, "Add node as a child.", py::arg("node"), py::arg("nodeParent") = nullptr)
            .def("AddChildTerm", py::overload_cast<const NonTerminal&, const ProductionRule&>(&TreeNode::AddChildTerm), py::return_value_policy::reference, "Add child NonTerminal node to the target.", py::arg("nonTerm"), py::arg("generatorPR"))
            .def("AddChildTerm", py::overload_cast<const Terminal&>(&TreeNode::AddChildTerm), py::return_value_policy::reference, "Add child Terminal node to the target. If the Terminal has only one possible value, sets it as the termValue.", py::arg("term"))
            .def("AddChildTerm", py::overload_cast<const Terminal&, const std::string&>(&TreeNode::AddChildTerm), py::return_value_policy::reference, "Add child Terminal node to the target.", py::arg("term"), py::arg("ptermValue"))
            .def("ClearSynthesis", &TreeNode::ClearSynthesis, "Reset the synthesis of this node.")
            .def("IsSynthesized", &TreeNode::IsSynthesized, "Check if this node has been synthesized.")
            .def("ClearEvaluation", &TreeNode::ClearEvaluation, "Reset the evaluation of this node.")
            .def("IsEvaluated", &TreeNode::IsEvaluated, "Check if this node has been evaluated.")
            .def("HasChildren", &TreeNode::HasChildren, "Check if this node has children.")
            .def("GetValue", &TreeNode::GetValue, "Returns the value of the node.")
            .def("GetLabel", &TreeNode::GetLabel, "Returns a formatted label of the node.")
            .def("ToString", &TreeNode::ToString, "Get node representation as string.")
            .def("__repr__",
                 [](const TreeNode &node) {
                     return "<gbgp.TreeNode '" + node.ToString() + "'>";
                 }
            );

    py::class_<SyntaxTree>(m, "SyntaxTree")
            .def(py::init<>(), "Creates an empty SyntaxTree.")
            .def(py::init<const TreeNode&>(), "Builds a tree from a root node.", py::arg("root"))
            .def(py::init<const SyntaxTree&>(), "Copy constructor.", py::arg("other"))
            .def_static("CopySubtree", &SyntaxTree::CopySubtree, "Get subtree starting from subTreeStartNode.", py::arg("subTreeStartNode"))
            .def("Root", &SyntaxTree::Root, "Returns a reference to the root.", py::return_value_policy::reference)
            .def("SetRoot", &SyntaxTree::SetRoot, "Set the root node.", py::arg("rootNode"))
            .def("DeleteSubtree", &SyntaxTree::DeleteSubtree, "Removes the subtree starting from rootOfSubtree.", py::arg("rootOfSubtree"))
            .def("InsertSubtree", py::overload_cast<TreeNode*, TreeNode*>(&SyntaxTree::InsertSubtree), "Insert a copy of the subtree into the position at insertNode.", py::arg("insertNode"), py::arg("subtreeStartNode"))
            .def("InsertSubtree", py::overload_cast<TreeNode*, const SyntaxTree&>(&SyntaxTree::InsertSubtree), "Insert a copy of the subtree into the position at insertNode.", py::arg("insertNode"), py::arg("subtree"))
            .def("IsEmpty", &SyntaxTree::IsEmpty, "Check if the tree is empty.")
            .def("SetRootRule", &SyntaxTree::SetRootRule, "Set the production rule of the root node.", py::arg("startRule"))
            .def("ToString", &SyntaxTree::ToString, "Get string representation.")
            .def("GetTreeTraversal", &SyntaxTree::GetTreeTraversal, "Traverses the tree in a depth first post-order.")
            .def("SynthesizeExpression", &SyntaxTree::SynthesizeExpression, "Synthesizes the tree into an expression using the production rules of the grammar.")
            .def("Evaluate", &SyntaxTree::Evaluate, "Evaluates the tree using the semantic actions of the grammar.", py::arg("ctx"))
            .def("ExternalEvaluate", &SyntaxTree::ExternalEvaluate<string>, "Evaluates the tree using an external evaluator.", py::arg("evaluator"))
            .def("__repr__",
                 [](const SyntaxTree &tree) {
                     return "<gbgp.SyntaxTree '" + tree.ToString() + "'>";
                 }
            );

    py::class_<PruneRule>(m, "PruneRule")
            .def(py::init<const SyntaxTree&, const SyntaxTree&>(), "Constructor by SyntaxTree.")
            .def("CanBeApplied", &PruneRule::CanBeApplied, "Does the target tree can be simplified further with this rule?", py::arg("target"))
            .def("Apply", &PruneRule::Apply, "Create a new SyntaxTree where the prune rule has been applied.")
            .def("ToString", &PruneRule::ToString, "Get string representation.")
            .def("__repr__",
                 [](const PruneRule &pruneRule) {
                     return "<gbgp.PruneRule '" + pruneRule.ToString() + "'>";
                 }
            );

    py::class_<Grammar>(m, "Grammar")
            .def(py::init<>(), "Empty constructor.")
            .def(py::init<std::initializer_list<ProductionRule>>(), "")
            .def(py::init<std::initializer_list<ProductionRule>, std::initializer_list<PruneRule>>(), "")
            .def(py::init<const Grammar&>(), "Copy constructor.")
            .def(py::init<const std::vector<ProductionRule>&>(), "")

            .def("GetRootRule", &Grammar::GetRootRule, "Get the root rule of the grammar.")
            .def("Size", &Grammar::Size, "Get the number of production rules of this grammar.")
            .def("CreateRandomTree", py::overload_cast<SyntaxTree&>(&Grammar::CreateRandomTree, py::const_), "Create random tree safely by creating random trees until there is a success.", py::arg("syntaxTree"))
            .def("CreateRandomTree", py::overload_cast<SyntaxTree&, int>(&Grammar::CreateRandomTree, py::const_), "Create random tree safely by creating random trees until there is a success.", py::arg("syntaxTree"), py::arg("maxDepth"))
            .def("PruneTree", &Grammar::PruneTree, " Applies the grammar prune rules repeatedly until no further simplification can be performed.", py::arg("syntaxTree"))
            .def("ToString", &Grammar::ToString, "Get string representation.")
            .def("__repr__",
                 [](const Grammar &grammar) {
                     return "<gbgp.Grammar '" + grammar.ToString() + "'>";
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
            ;
}