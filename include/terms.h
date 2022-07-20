#pragma once
#include <string>
#include <vector>
#include "vector_ops.h"

//*****************************
//*         Terminal          *
//****************************/

/// Terminal term. A terminal is a leaf node in the expression _tree that contains a value.
struct Terminal
{
    /// The term type.
    int id;

    /// Used for debugging and visual representation of rules and trees.
    std::string label;

    /// List of possible terminal values that a node of this type can contain.
    std::vector<std::string> values;

    Terminal()
    {
        id = -1;
        label = '\0';
    }
    Terminal(int pid, const std::string& plabel)
    {
        id = pid;
        label = plabel;
        values.push_back(plabel);
    }
    Terminal(int pid, const std::string& plabel, const std::vector<std::string>& pvalues)
    {
        id = pid;
        label = plabel;
        values = pvalues;
    }

    bool operator==(const Terminal& other) const
    {
        return id == other.id;
    }
    bool operator!=(const Terminal& other) const
    {
        return id != other.id;
    }

    [[nodiscard]]
    std::string GetValue() const
    {
        if (values.size() == 1)
            return values.front();
        else
        {
            // Select a terminal value randomly.
            std::vector<size_t> applicableTerminals = range<size_t>(values.size());
            const size_t r = *random_choice(applicableTerminals.begin(), applicableTerminals.end());
            return values[r];
        }
    }
};

//*****************************
//*        NonTerminal        *
//****************************/

/// Non-Terminal term. A non-terminal is a part of the expression _tree that contains other children nodes.
struct NonTerminal
{
    /// The term type.
    int id;

    /// Used for debugging and visual representation of rules and trees.
    std::string label;

    NonTerminal()
    {
        id = -1;
        label = '\0';
    }
    NonTerminal(int pid, const std::string& plabel)
    {
        id = pid;
        label = plabel;
    }

    bool operator==(const NonTerminal& other) const
    {
        return id == other.id;
    }
    bool operator!=(const NonTerminal& other) const
    {
        return id != other.id;
    }
};