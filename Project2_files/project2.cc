/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include "lexer.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

LexicalAnalyzer lexer;

struct Rule {
    string LHS;
    vector<string> RHS;
};

vector<Rule> rules;

// task 0
void parse_Grammar();
void parse_Rule_list();
void parse_Rule();
void parse_Right_hand_side(const string &lhs);
void parse_Id_list(vector<string> &rhs);
void expect(TokenType expected);

void expect(TokenType expected)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected) {
        cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << endl;
        exit(1);
    }
}

void parse_Id_list(vector<string> &rhs)
{
    Token next = lexer.peek(1);
    if (next.token_type == ID) {
        Token t = lexer.GetToken();
        rhs.push_back(t.lexeme);
        parse_Id_list(rhs);
    }
}

void parse_Right_hand_side(string &lhs)
{
    Rule r;
    r.LHS = lhs;
    parse_Id_list(r.RHS);
    rules.push_back(r);

    Token next = lexer.peek(1);
    if (next.token_type == OR) {
        lexer.GetToken();
        parse_Right_hand_side(lhs);
    }
}

void parse_Rule()
{
    Token lhsT = lexer.GetToken();
    if (lhsT.token_type != ID) {
        cout << "SYNTAX ERROR !!!!!!!!!!!!!!" << endl;
        exit(1);
    }
    string lhs = lhsT.lexeme;

    expect(ARROW);
    parse_Right_hand_side(lhs);
    expect(STAR);
}

void parse_Rule_list()
{
    parse_Rule();

    Token next = lexer.peek(1);
    if (next.token_type == ID) {
        parse_Rule_list();
    }
}

void parse_Grammar()
{
    parse_Rule_list();
    expect(HASH);
}

// read grammar
void ReadGrammar()
{
    parse_Grammar();
    expect(END_OF_FILE);
}

/*
 * Task 1:
 * Printing the terminals, then nonterminals of grammar in appearing order
 * output is one line, and all names are space delineated
 */

bool contains(unordered_set<string> s1, string s)
{
    for (string str : s1) {
        if (s.compare(str) == 0) {
            return true;
        }
    }
    return false;
}

bool contains(vector<string> v, string s)
{
    for (string str : v) {
        if (s.compare(str) == 0) {
            return true;
        }
    }
    return false;
}

bool contains(set<string> set1, set<string> set2)
{
    int c = 0;
    for (string s2 : set2) {
        for (string s1 : set1) {
            if (s1.compare(s2) == 0) {
                c++;
            }
        }
    }
    return c == set2.size();
}

void Task1()
{
    unordered_set<string> non_terminal;
    unordered_set<string> terminal;
    vector<string> temp;

    for (int i = 0; i < rules.size(); i++)
        non_terminal.insert(rules[i].LHS);

    for (int i = 0; i < rules.size(); i++) {
        if (!contains(temp, rules[i].LHS))
            temp.push_back(rules[i].LHS);

        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (!contains(non_terminal, rules[i].RHS[j]) &&
                !contains(terminal, rules[i].RHS[j])) {
                cout << rules[i].RHS[j] << " ";
                terminal.insert(rules[i].RHS[j]);
            }
            else if (!contains(temp, rules[i].RHS[j]) &&
                     !contains(terminal, rules[i].RHS[j]))
                temp.push_back(rules[i].RHS[j]);
        }
    }

    for (auto i = temp.cbegin(); i != temp.cend(); i++) {
        cout << *i << " ";
    }
    cout << endl;
}

/*
 * Task 2:
 * Print out nullable set of the grammar in specified format.
 */
void Task2()
{
    unordered_set<string> nullable;
    vector<string> answer;

    for (int i = 0; i < rules.size(); i++) {
        if (rules[i].RHS.size() < 1 && !contains(nullable, rules[i].LHS)) {
            nullable.insert(
                rules[i]
                    .LHS); // if RHS is empty and is not is nullable set add it!
        }
    }

    bool changed = true;

    while (changed) {
        changed = false;
        for (int i = 0; i < rules.size(); i++) {
            bool temp = true;
            for (int j = 0; j < rules[i].RHS.size(); j++) {
                if (!contains(nullable, rules[i].RHS[j])) {
                    temp = false;
                }
            }
            if (temp && !contains(nullable, rules[i].LHS)) {
                nullable.insert(rules[i].LHS);
                changed = true;
                temp = false;
            }
        }
    }

    for (int i = 0; i < rules.size(); i++) {
        if (contains(nullable, rules[i].LHS) &&
            !contains(answer, rules[i].LHS)) {
            answer.push_back(rules[i].LHS);
        }
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (contains(nullable, rules[i].RHS[j]) &&
                !contains(answer, rules[i].RHS[j])) {
                answer.push_back(rules[i].RHS[j]);
            }
        }
    }

    cout << "Nullable = { ";
    for (int i = 0; i < answer.size(); i++) {
        if (i != answer.size() - 1)
            cout << answer.at(i) << " , ";
        else
            cout << answer.at(i);
    }
    cout << "}" << endl;
}

// Task 3: FIRST sets
void Task3()
{
    unordered_set<string> nullable;
    unordered_set<string> non_terminal;
    unordered_set<string> terminal;
    unordered_map<string, set<string>> FIRST;
    vector<string> answer;
    bool changed = true;
    // ========================================================================
    // calc terminal/nonterminal
    for (int i = 0; i < rules.size(); i++) {
        if (rules[i].RHS.size() < 1 && !contains(nullable, rules[i].LHS)) {
            nullable.insert(
                rules[i]
                    .LHS); // if RHS is empty and is not is nullable set add it!
        }
    }

    for (int i = 0; i < rules.size(); i++)
        non_terminal.insert(rules[i].LHS);

    for (int i = 0; i < rules.size(); i++) {
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (!contains(non_terminal, rules[i].RHS[j]) &&
                !contains(terminal, rules[i].RHS[j]))
                terminal.insert(rules[i].RHS[j]);
        }
    }
    //==================================================================== calc
    // null set
    while (changed) { // calculate nullable set
        changed = false;
        for (int i = 0; i < rules.size(); i++) {
            bool temp = true;
            for (int j = 0; j < rules[i].RHS.size(); j++) {
                if (!contains(nullable, rules[i].RHS[j])) {
                    temp = false;
                }
            }
            if (temp && !contains(nullable, rules[i].LHS)) {
                nullable.insert(rules[i].LHS);
                changed = true;
                temp = false;
            }
        }
    }
    // =======================================================================
    // calc follow set
    for (int i = 0; i < rules.size();
         i++) { // Initialize the first sets for terminals
        FIRST[rules[i].LHS] = {};
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (contains(terminal, rules[i].RHS[j]))
                FIRST[rules[i].RHS[j]] = {rules[i].RHS[j]};
        }
    }

    changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < rules.size(); i++) {
            for (int j = 0; j < rules[i].RHS.size(); j++) {
                if (j == 0) {
                    if (!contains(FIRST[rules[i].LHS],
                                  FIRST[rules[i].RHS[j]])) {
                        FIRST[rules[i].LHS].insert(
                            FIRST[rules[i].RHS[j]].cbegin(),
                            FIRST[rules[i].RHS[j]].cend());
                        changed = true;
                    }
                    int temp = 0;
                    while (contains(nullable, rules[i].RHS[j + temp]) &&
                           j + temp + 1 < rules[i].RHS.size()) {
                        temp++;
                        if (contains(terminal, rules[i].RHS[j + temp])) {
                            if (!contains(FIRST[rules[i].LHS],
                                          FIRST[rules[i].RHS[j + temp]])) {
                                FIRST[rules[i].LHS].insert(
                                    FIRST[rules[i].RHS[j + temp]].cbegin(),
                                    FIRST[rules[i].RHS[j + temp]].cend());
                                changed = true;
                            }
                            break;
                        }
                        else {
                            if (!contains(FIRST[rules[i].LHS],
                                          FIRST[rules[i].RHS[j + temp]])) {
                                FIRST[rules[i].LHS].insert(
                                    FIRST[rules[i].RHS[j + temp]].cbegin(),
                                    FIRST[rules[i].RHS[j + temp]].cend());
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < rules.size(); i++) { // order of non_terminals
        if (contains(non_terminal, rules[i].LHS) &&
            !contains(answer, rules[i].LHS)) {
            answer.push_back(rules[i].LHS);
        }
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (contains(non_terminal, rules[i].RHS[j]) &&
                !contains(answer, rules[i].RHS[j])) {
                answer.push_back(rules[i].RHS[j]);
            }
        }
    }

    vector<string> universe; // order of grammer aka T and NT
    for (int i = 0; i < rules.size(); i++) {
        if (!contains(universe, rules[i].LHS))
            universe.push_back(rules[i].LHS);
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (!contains(universe, rules[i].RHS[j]))
                universe.push_back(rules[i].RHS[j]);
        }
    }

    for (int i = 0; i < answer.size(); i++) {
        cout << "FIRST(" << answer[i] << ") = { ";
        bool first_elem = true;
        for (int u = 0; u < universe.size(); u++) {
            if (FIRST[answer[i]].count(universe[u])) {
                if (!first_elem)
                    cout << ", ";
                cout << universe[u];
                first_elem = false;
            }
        }
        cout << " }" << endl;
    }
}

// Task 4: FOLLOW sets
void Task4()
{
    unordered_set<string> nullable;
    unordered_set<string> non_terminal;
    unordered_set<string> terminal;
    unordered_map<string, set<string>> FIRST;
    unordered_map<string, set<string>> FOLLOW;
    vector<string> answer;
    bool changed = true;
    // ========================================================================
    // calc terminal/nonterminal
    for (int i = 0; i < rules.size(); i++) {
        if (rules[i].RHS.size() < 1 && !contains(nullable, rules[i].LHS)) {
            nullable.insert(
                rules[i]
                    .LHS); // if RHS is empty and is not is nullable set add it!
        }
    }

    for (int i = 0; i < rules.size(); i++)
        non_terminal.insert(rules[i].LHS);

    for (int i = 0; i < rules.size(); i++) {
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (!contains(non_terminal, rules[i].RHS[j]) &&
                !contains(terminal, rules[i].RHS[j]))
                terminal.insert(rules[i].RHS[j]);
        }
    }
    //==================================================================== calc
    // null set
    while (changed) { // calculate nullable set
        changed = false;
        for (int i = 0; i < rules.size(); i++) {
            bool temp = true;
            for (int j = 0; j < rules[i].RHS.size(); j++) {
                if (!contains(nullable, rules[i].RHS[j])) {
                    temp = false;
                }
            }
            if (temp && !contains(nullable, rules[i].LHS)) {
                nullable.insert(rules[i].LHS);
                changed = true;
                temp = false;
            }
        }
    }
    // =======================================================================
    // calc first set
    for (int i = 0; i < rules.size();
         i++) { // Initialize the first sets for terminals
        FIRST[rules[i].LHS] = {};
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (contains(terminal, rules[i].RHS[j]))
                FIRST[rules[i].RHS[j]] = {rules[i].RHS[j]};
        }
    }

    changed = true; // LOOP untill you have no changes to first sets
    while (changed) {
        changed = false;
        for (int i = 0; i < rules.size(); i++) {
            for (int j = 0; j < rules[i].RHS.size(); j++) {
                if (j == 0) {
                    if (!contains(FIRST[rules[i].LHS],
                                  FIRST[rules[i].RHS[j]])) {
                        FIRST[rules[i].LHS].insert(
                            FIRST[rules[i].RHS[j]].cbegin(),
                            FIRST[rules[i].RHS[j]].cend());
                        changed = true;
                    }
                    int temp = 0;
                    while (contains(nullable, rules[i].RHS[j + temp]) &&
                           j + temp + 1 < rules[i].RHS.size()) {
                        temp++;
                        if (contains(terminal, rules[i].RHS[j + temp])) {
                            if (!contains(FIRST[rules[i].LHS],
                                          FIRST[rules[i].RHS[j + temp]])) {
                                FIRST[rules[i].LHS].insert(
                                    FIRST[rules[i].RHS[j + temp]].cbegin(),
                                    FIRST[rules[i].RHS[j + temp]].cend());
                                changed = true;
                            }
                            break;
                        }
                        else {
                            if (!contains(FIRST[rules[i].LHS],
                                          FIRST[rules[i].RHS[j + temp]])) {
                                FIRST[rules[i].LHS].insert(
                                    FIRST[rules[i].RHS[j + temp]].cbegin(),
                                    FIRST[rules[i].RHS[j + temp]].cend());
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
    //========================================= calculate FOLLOW sets

    unordered_set<string> temp;
    bool is_nullable_set;

    for (int i = 0; i < rules.size();
         i++) { // Initialize the non_terminals FOLLOW sets to {} and FOLLOW(S)
                // = $
        if (i == 0) {
            FOLLOW[rules[i].LHS] = {"$"};
            temp.insert(rules[i].LHS);
        }
        else if (!contains(temp, rules[i].LHS)) {
            FOLLOW[rules[i].LHS] = {};
            temp.insert(rules[i].LHS);
        }
    }
    // RULE 4 If there precedes a NT and another T/NT then put FIRST(T/NT) in
    // FOLLOW(NT)

    // RULE 5 If there precedes a NT and a nullable NT then put FIRST(T/NT) in
    // FOLLOW(NT) for any proceding NT/T since Nullable allows for you to skip
    // that NT

    for (int i = 0; i < rules.size(); i++) { // for each grammer rule as well
        for (int j = 0; j < rules[i].RHS.size(); j++) { // for each RHS
            if (contains(non_terminal,
                         rules[i].RHS[j])) { // IF your on a non_termnial check
                                             // if any other things follow it
                for (int k = j + 1; k < rules[i].RHS.size();
                     k++) { // check if preceds any Nullable NT
                    if (contains(
                            nullable,
                            rules[i].RHS[k])) { // If what preceeds is NULLABLE
                                                // then do FIRST[nullable] then
                                                // continue down
                        FOLLOW[rules[i].RHS[j]].insert(
                            FIRST[rules[i].RHS[k]].cbegin(),
                            FIRST[rules[i].RHS[k]].cend());
                    }
                    else {
                        FOLLOW[rules[i].RHS[j]].insert(
                            FIRST[rules[i].RHS[k]].cbegin(),
                            FIRST[rules[i].RHS[k]].cend());
                        break;
                    }
                }
            }
        }
    }

    // RULE 2 If The last thing in the grammer is a NT then add
    // FOLLOW(NT)  to FOLLOW (grammer)

    // RULE 3 If There are NULLABLE NT that preceeds a NT to the end, then add
    // FOLLOW(NT) to FOLLOW(grammer) for each NT that can Be thelast NT in the
    // grammer

    changed = true; // Did the set change?
    while (changed) {
        changed = false;
        for (int i = 0; i < rules.size(); i++) {
            for (int j = 0; j < rules[i].RHS.size(); j++) {
                if (contains(non_terminal, rules[i].RHS[j]) &&
                    j == rules[i].RHS.size() - 1 &&
                    !contains(FOLLOW[rules[i].RHS[j]], FOLLOW[rules[i].LHS])) {
                    FOLLOW[rules[i].RHS[j]].insert(
                        FOLLOW[rules[i].LHS].cbegin(),
                        FOLLOW[rules[i].LHS].cend());
                    changed = true;
                }
                else if (contains(non_terminal, rules[i].RHS[j])) {
                    is_nullable_set = true;
                    for (int k = j + 1; k < rules[i].RHS.size(); k++) {
                        if (!contains(nullable, rules[i].RHS[k])) {
                            is_nullable_set = false;
                        }
                    }
                    if (is_nullable_set && !contains(FOLLOW[rules[i].RHS[j]],
                                                     FOLLOW[rules[i].LHS])) {
                        FOLLOW[rules[i].RHS[j]].insert(
                            FOLLOW[rules[i].LHS].cbegin(),
                            FOLLOW[rules[i].LHS].cend());
                        changed = true;
                    }
                }
            }
        }
    }

    // ==================================PRINT IT ALLLLL

    for (int i = 0; i < rules.size(); i++) { //
        if (contains(non_terminal, rules[i].LHS) &&
            !contains(answer, rules[i].LHS)) {
            answer.push_back(rules[i].LHS);
        }
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (contains(non_terminal, rules[i].RHS[j]) &&
                !contains(answer, rules[i].RHS[j])) {
                answer.push_back(rules[i].RHS[j]);
            }
        }
    }

    vector<string> universe; // order of grammer aka T and NT
    universe.push_back("$");
    for (int i = 0; i < rules.size(); i++) {
        if (!contains(universe, rules[i].LHS))
            universe.push_back(rules[i].LHS);
        for (int j = 0; j < rules[i].RHS.size(); j++) {
            if (!contains(universe, rules[i].RHS[j]))
                universe.push_back(rules[i].RHS[j]);
        }
    }

    for (int i = 0; i < answer.size(); i++) {
        cout << "FOLLOW(" << answer[i] << ") = { ";
        bool first_elem = true;
        for (int u = 0; u < universe.size(); u++) {
            if (FOLLOW[answer[i]].count(universe[u])) {
                if (!first_elem)
                    cout << ", ";
                cout << universe[u];
                first_elem = false;
            }
        }
        cout << " }" << endl;
    }
}

void Task5()
{
}


void Task6()
{
}

int main(int argc, char *argv[])
{
    int task;

    if (argc < 2) {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    ReadGrammar(); // Reads the input grammar from standard input
                   // and represent it internally in data structures
                   // ad described in project 2 presentation file

    switch (task) {
    case 1:
        Task1();
        break;

    case 2:
        Task2();
        break;

    case 3:
        Task3();
        break;

    case 4:
        Task4();
        break;

    case 5:
        Task5();
        break;

    case 6:
        Task6();
        break;

    default:
        cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}
