#pragma once

#include <iostream>
#include <algorithm>
#include <format>
#include <string>
#include <array>
#include <ranges>
#include <utility>

#define V ||
#define A &&
#define T true
#define F false

#define ATOM(P) propos P = { std::string(#P), T };
#define ATOM_T(P, trueValue) propos P = { std::string(#P), trueValue };

#define RepeatOutput(count, outputStream, content) \
        for (int i = 0; i < count; i++)\
            outputStream << content;\

#define OPERATOR_CODE(formulaFormatString, trueValueRule) \
        \
        auto formula = formulaFormatString;\
        auto trueValue = trueValueRule;\
        if (TruthTableManager::getInstance()->enabled()) {\
            if (TruthTableManager::getInstance()->initTableWidth()) {\
                if (TruthTableManager::getInstance()->hasLabel()) {\
                    *(TruthTableManager::getInstance()->debugOstream()) << std::format("|     {}     |", formula);\
                } else {\
                    *(TruthTableManager::getInstance()->debugOstream()) << "|     ";\
                    int formulaLength = formula.length();\
                    RepeatOutput(formulaLength / 2, *(TruthTableManager::getInstance()->debugOstream()), ' ');\
                    *(TruthTableManager::getInstance()->debugOstream()) << (trueValue ? 'T' : 'F');\
                    if (formulaLength % 2 == 1) {\
                        RepeatOutput(formulaLength / 2, *(TruthTableManager::getInstance()->debugOstream()), ' ');\
                    } else {\
                        RepeatOutput((formulaLength / 2) - 1, *(TruthTableManager::getInstance()->debugOstream()), ' ');\
                    }\
                    *(TruthTableManager::getInstance()->debugOstream()) << "     |";\
                }\
            } else {\
                int unitLength = formula.length() + 12;\
                TruthTableManager::getInstance()->setTableWidth(TruthTableManager::getInstance()->tableWidth() + unitLength);\
            }\
        }\
        return { std::move(formula), trueValue };\

#define TRIVIAL_GETTER_SETTER(memberName, memberType, setterName) \
        \
        memberType memberName() const {\
            return m_##memberName;\
        }\
        void setterName(memberType memberName) {\
            m_##memberName = memberName;\
        }\

// singleton
class TruthTableManager {
private:
    bool m_enabled = false;
    bool m_hasLabel = true;
    bool m_initTableWidth = false;
    unsigned int m_tableWidth = 0;

    std::ostream* m_debugOstream = &(std::cout);
    TruthTableManager() = default;

public:
    static TruthTableManager* getInstance() {
        static TruthTableManager Instance;
        return &Instance;
    }

    TRIVIAL_GETTER_SETTER(enabled, bool, setEnabled)
    TRIVIAL_GETTER_SETTER(hasLabel, bool, setHasLabel)
    TRIVIAL_GETTER_SETTER(tableWidth, unsigned int, setTableWidth)
    TRIVIAL_GETTER_SETTER(initTableWidth, unsigned int, setInitTableWidth)

    auto debugOstream() const {
        return m_debugOstream;
    }
    void setDebugOstream(std::ostream& os) {
        m_debugOstream = &os;
    }
};

class proposTemp {
private:
    std::string m_formula;
    bool m_trueValue;
    proposTemp() = delete;
    proposTemp(std::string&& formula, bool trueValue) noexcept
        : m_formula(std::move(formula)), m_trueValue(trueValue) {}
    operator bool() const {
        return m_trueValue;
    }

    friend class propos;
    friend propos operator>(proposTemp, propos);
};

class propos {
private:
    std::string m_formula;
    bool m_trueValue;

public:
    propos(const std::string& formula, bool trueValue)
        : m_formula(formula), m_trueValue(trueValue) {}
    propos(std::string&& formula, bool trueValue) noexcept
        : m_formula(std::move(formula)), m_trueValue(trueValue) {}
    

    void assign(bool trueValue) {
        m_trueValue = trueValue;
    }
    bool trueValue() const {
        return m_trueValue;
    }
    const std::string& formula() const {
        return m_formula;
    }

    operator bool() const {
        return m_trueValue;
    }
    proposTemp operator--(int) const {
        return { std::format("{}--", m_formula), m_trueValue };
    }
    propos operator<=>(propos rhs) const {
        OPERATOR_CODE (
            std::format("{}<-->{}", m_formula, rhs.m_formula),
            m_trueValue == rhs
        );
    }
    propos operator&&(propos rhs) const {
        OPERATOR_CODE (
            std::format("({} A {})", m_formula, rhs.m_formula),
            m_trueValue && rhs
        );
    }
    propos operator||(propos rhs) const {
        OPERATOR_CODE (
            std::format("({} V {})", m_formula, rhs.m_formula),
            m_trueValue || rhs
        );
    }
    propos operator!() const {
        OPERATOR_CODE (
            std::format("(!{})", m_formula),
            !m_trueValue
        );
    }
};

propos operator>(proposTemp lhs, propos rhs) {
    OPERATOR_CODE (
        std::format("{}>{}", lhs.m_formula, rhs.formula()),
        !((lhs == T) && (rhs == F))
    );
}

std::ostream& operator<<(std::ostream& os, propos p) {
    if (TruthTableManager::getInstance()->enabled()) {
        if (TruthTableManager::getInstance()->initTableWidth()) {
            if (TruthTableManager::getInstance()->hasLabel()) {
                os << std::format("|     {}     |", p.formula());
            } else {
                os << std::format("|     {}     |", (p.trueValue() ? 'T' : 'F'));  
            }
        } else {
            TruthTableManager::getInstance()->setTableWidth(TruthTableManager::getInstance()->tableWidth() + 13);
        }
    } else {
        os << (p.trueValue() ? 'T' : 'F');
    }
    return os;
}

#define _GenerateTruthTableFramework(AtomDecl, AtomTTable, FinalFormula, AtomCount) \
        \
        {\
            TruthTableManager::getInstance()->setEnabled(true);\
            AtomDecl;\
            std::array<bool, AtomCount> trueValueArray = { F };\
            int index = AtomCount - 1;\
            AtomTTable;\
            FinalFormula;\
            TruthTableManager::getInstance()->setInitTableWidth(true);\
            int tableWidth = TruthTableManager::getInstance()->tableWidth();\
            auto debugOstream = TruthTableManager::getInstance()->debugOstream();\
            RepeatOutput(tableWidth, (*debugOstream), '_');\
            (*debugOstream) << '\n';\
            AtomTTable;\
            FinalFormula;\
            (*debugOstream) << '\n';\
            TruthTableManager::getInstance()->setHasLabel(false);\
            while (index != -2) {\
                do {\
                    RepeatOutput(tableWidth, (*debugOstream), '_');\
                    (*debugOstream) << '\n';\
                    AtomTTable;\
                    FinalFormula;\
                    (*debugOstream) << '\n';\
                } while (std::ranges::next_permutation(trueValueArray).found);\
                for (int i = 0; i < AtomCount; i++)\
                    trueValueArray[i] = ((i >= index) ? T : F);\
                index--;\
            }\
            RepeatOutput(tableWidth, (*debugOstream), '_');\
            TruthTableManager::getInstance()->setTableWidth(0);\
            TruthTableManager::getInstance()->setEnabled(false);\
            TruthTableManager::getInstance()->setHasLabel(true);\
            TruthTableManager::getInstance()->setInitTableWidth(false);\
        }\

#define _AtomDecl2P(P, Q) ATOM(P);ATOM(Q);
#define _AtomDecl3P(P, Q, R) _AtomDecl2P(P, Q);ATOM(R);
#define _AtomDecl4P(P, Q, R, S) _AtomDecl3P(P, Q, R);ATOM(S);

#define _AtomTruthTable1P(P, idx) \
        P.assign(trueValueArray[idx]);\
        *(TruthTableManager::getInstance()->debugOstream()) << P;\

#define _AtomTruthTable2P(P, Q) \
        _AtomTruthTable1P(P, 0);\
        _AtomTruthTable1P(Q, 1);\

#define _AtomTruthTable3P(P, Q, R) \
        _AtomTruthTable2P(P, Q);\
        _AtomTruthTable1P(R, 2);\

#define _AtomTruthTable4P(P, Q, R, S) \
        _AtomTruthTable3P(P, Q, R);\
        _AtomTruthTable1P(S, 3);\

#define GenTruthTable1P(P, FORMULA) _GenerateTruthTableFramework(ATOM(P), _AtomTruthTable1P(P, 0), FORMULA, 1)
#define GenTruthTable2P(P, Q, FORMULA) _GenerateTruthTableFramework(_AtomDecl2P(P, Q), _AtomTruthTable2P(P, Q), FORMULA, 2)
#define GenTruthTable3P(P, Q, R, FORMULA) _GenerateTruthTableFramework(_AtomDecl3P(P, Q, R), _AtomTruthTable3P(P, Q, R), FORMULA, 3)
#define GenTruthTable4P(P, Q, R, S, FORMULA) _GenerateTruthTableFramework(_AtomDecl4P(P, Q, R, S), _AtomTruthTable4P(P, Q, R, S), FORMULA, 4)