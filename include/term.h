#pragma once
#include <string>
#include <vector>
#include "vector_ops.h"

namespace gbgp
{
    //*****************************
    //*         Terminal          *
    //****************************/

    /// Terminal term. A terminal is a leaf node in the expression tree that contains a value.
    struct Terminal
    {
        /// The term type.
        int id;

        /// The label used visual representation and debugging of rules and trees.
        std::string label;

        /// List of possible terminal values that a node of this type can contain.
        std::vector<std::string> values;

        Terminal()
        {
            id = -1;
            label = "";
            values.clear();
        }
        explicit Terminal(const std::string& value)
        {
            id = -1;
            label = value;
            values.push_back(value);
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
            return id != -1 ? id == other.id : label == other.label;
        }
        bool operator!=(const Terminal& other) const
        {
            return id != -1 ? id != other.id : label != other.label;
        }

        /// Does this terminal have multiple possible values?
        [[nodiscard]]
        bool IsMutable() const
        {
            return values.size() > 1;
        }

        /// Get a random value of the set of possible values.
        [[nodiscard]]
        std::string GetRandomValue() const
        {
            if (values.size() == 1)
                return values.front();
            else
                return *random_choice(values.begin(), values.end());
        }

        /// Get string representation.
        [[nodiscard]]
        std::string ToString() const
        {
            return "Terminal(id='" + std::to_string(id) + "', label='" + label + "', values='"
                   + vector_to_string(values) + ")";
        }

        /// Serialization hook.
        template<class Archive> void serialize(Archive& ar)
        {
            ar(id, label, values);
        }
    };

    //*****************************
    //*        NonTerminal        *
    //****************************/

    /// Non-Terminal term. A non-terminal is a part of the expression tree that contains other children nodes.
    struct NonTerminal
    {
        /// The term type.
        int id;

        /// Used for debugging and visual representation of rules and trees.
        std::string label;

        /// Empty constructor.
        NonTerminal()
        {
            id = -1;
            label = "";
        }

        /// Constructor by id and label.
        /// \param pid The id.
        /// \param plabel The label.
        NonTerminal(int pid, const std::string& plabel)
        {
            id = pid;
            label = plabel;
        }

        bool operator==(const NonTerminal& other) const
        {
            return id != -1 ? id == other.id : label == other.label;
        }
        bool operator!=(const NonTerminal& other) const
        {
            return id != -1 ? id != other.id : label != other.label;
        }

        /// Get string representation.
        [[nodiscard]]
        std::string ToString() const
        {
            return "NonTerminal(id='" + std::to_string(id) + "', label='" + label + "')";
        }

        /// Serialization hook.
        template<class Archive> void serialize(Archive& ar)
        {
            ar(id, label);
        }
    };
}