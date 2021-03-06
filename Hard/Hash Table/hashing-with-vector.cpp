//  Name: Vo Huy
//  Assignment number: 5
//  Assignment: Spell Checking with the Hash Table Data Structure
//  File name: huy.cpp
//  Date last modified: March 25, 2018
//  Honor statement: I have neither given nor received any unauthorized help on this assignment. 

using namespace std;
#include <fstream>      // std::ifstream
#include <iostream>     // std::cout, std::fixed
#include <vector>       // std::vector
#include <string>       // std::string
#include <queue>        // std::priority_queue
#include <cstddef>      // std::size_t

class HashTable {
private:
    vector<vector<string>> nodesTable;      // hash nodes array
    unsigned tableSize;         // hash table size
    unsigned numElement;        // number of elements
public:
    /***    CONSTRUCTOR    ***/
    HashTable (unsigned tableSize, string fileName) : nodesTable(tableSize) {
        this->tableSize = tableSize;
        numElement = 0;         // Intial number of elements = 0

        ifstream file(fileName);// reading the text   
        string word;            // each word in the text file
        if (!file) {            // Check if the text file could be opened
            cout  << "Unable to open file " + fileName;
            exit(1);            // Unable to open the file, exit the function
        }
        while (file >> word) {
            bool inserting = insert(word);      // fill the hash table with words
            (void)inserting;    // silence the unused-variable warning
        }
    }

    // hash function maps a string to an index within the table
    // Using Fowler-Noll-Vo Hash (FNV)
    // based on:https://www.programmingalgorithms.com/algorithm/fnv-hash?lang=C%2B%2B
    unsigned hash(string word) const {
        const unsigned fnv_prime = 0x811C9DC5;
        unsigned hash = 0;      // hashing index
        for (unsigned i = 0; i < word.length(); i++)
        {
            hash *= fnv_prime;
            hash ^= (word[i]);
        }
        return hash % tableSize;
    }

    // insert method inserts a string into the hash table
    // return true if it successfully inserts the word into the table
    // otherwise, it should return false if the word already is present in the table
    bool insert(string word) {
        // the index of the word in the hash table
        unsigned index = hash(word); 
        // the chain at that index
        vector<string>& chain = nodesTable[index];
        // check if the word is already present
        if (!chain.empty()) {       // the chain is not empty
            for (auto &it : chain) {
                if (it == word)
                    return false;   // the word is already present
            }
        }
        chain.push_back(word);      // add a new node
        // successfully inserted the word
        numElement++;               // increment the size
        return true;
    }

    // remove method removes a string from the hash table 
    // returns true if it removes the word
    // otherwise, it returns false if the word to remove is not in the hash table
    bool remove(string word) {
        // the index of the word in the hash table
        unsigned index = hash(word); 
        // the chain at that index
        vector<string>& chain = nodesTable[index];
        // check if the word is present
        if (!chain.empty()) {       // the chain is not empty
            for (auto it = chain.begin(); it != chain.end(); it++) {
                if (*it == word) {
                    it = chain.erase(it);
                    numElement--;
                    return true;    // successfully found and removed the word
                }
            }
        }
        return false;               // the word is not present
    }

    // returns true if the hash table contains a given word
    // otherwise, the method returns false if the word is not present
    bool contain(string word) const { 
        // the index of the word in the hash table
        unsigned index = hash(word); 
        // the chain at that index
        vector<string> chain = nodesTable[index];
        // check if the word is present
        if (!chain.empty()) {       // the chain is not empty
            for (auto &it : chain) {
                if (it == word)
                    return true;    // the word is present
            }
        }
        return false;               // the word is not present
    }

    // returns the total number of strings in the hash table
    unsigned size() const {
        return numElement;
    }
};

// Ordering a to z for priority queue
class CompareAlphabets {
public:
    bool operator()(string word1, string word2) {
        return word1 > word2; 
    }
};

void checkOmission(const string userInput, const HashTable& hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& suggestedWords) {  
    // Try adding a single letter at all possible places in the word
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 0; i <= userInput.length(); i++) {
        for (char j = 'a'; j <= 'z'; j++) {
            string testingWord = userInput;     // make a copy of userInput to test 
            if (hashTable.contain(testingWord.insert(i, string(1, j)))) {
                suggestedWords.push(testingWord);
            }
        }
    }
}

void checkExtra(const string userInput, const HashTable& hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& suggestedWords) {
    // Try removing a single letter from the word
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 0; i < userInput.length(); i++) {
        string testingWord = userInput;         // make a copy of userInput to test
        if (hashTable.contain(testingWord.erase(i, 1))) {
            suggestedWords.push(testingWord);
        }
    }
}

void checkTypo(const string userInput, const HashTable& hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& suggestedWords) {
    // Try replacing an existing letter in the word with some other letter
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 0; i < userInput.length(); i++) {
        for (char j = 'a'; j <= 'z'; j++) {
            string testingWord = userInput;     // make a copy of userInput to test 
            if (hashTable.contain(testingWord.replace(i, 1, string(1, j)))) { 
                suggestedWords.push(testingWord);
            }
        }
    }
}

void checkTransposition(const string userInput, const HashTable& hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& suggestedWords) {
    // Try transposing adjacent letters
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 0; i < userInput.length() - 1; i++) {
        string testingWord = userInput;         // make a copy of userInput to test
        swap(testingWord[i], testingWord[i + 1]);       // swapping 2 adjacent elements
        if (hashTable.contain(testingWord)) {
            suggestedWords.push(testingWord);
        }
    }
}

void checkMissingSpace(const string userInput, const HashTable& hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& suggestedWords) {
    // Try adding a single space at all possible places in the word
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 1; i < userInput.length(); i++) {
        string testingWord1 = userInput.substr(0, i);   // make a copy of the first word
        string testingWord2 = userInput.substr(i);      // make a copy of the second word
        if (hashTable.contain(testingWord1) && hashTable.contain(testingWord2)) {
            // both words match words in the hash table, add the word pair
            suggestedWords.push(testingWord1 + ' ' + testingWord2);
        }
    }
}

// Attempt to suggest the user input
void suggest(const string userInput, const HashTable& hashTable) {
    // Make an empty priority queue
    priority_queue<string, std::vector<string>, CompareAlphabets> suggestedWords;
    // check if the user omitted a letter
    checkOmission(userInput, hashTable, suggestedWords);
    // check if the user added an extraneous letter
    checkExtra(userInput, hashTable, suggestedWords);
    // check if the user mistyped a letter
    checkTypo(userInput, hashTable, suggestedWords);
    // check if the user tranposed 2 adjacent letter
    checkTransposition(userInput, hashTable, suggestedWords);
    // check if the user forgot to add a space
    checkMissingSpace(userInput, hashTable, suggestedWords);

    // print out suggested words in alphabetic order, if there's none, print ???
    if (suggestedWords.empty()) {   // found none
        cout << "??? (Unable to provide a suggestion)\n";
    } else {
        cout << "The entered word is non-existent. Suggestions:\n";
        while (!suggestedWords.empty()) {
            cout << suggestedWords.top() << "\n";
            suggestedWords.pop();
        }
    }
}

int main() {
    HashTable hashTable{10000, "dictionary.txt"};
    string userInput;

    while (userInput != ".") {
        // Prompt user to enter a word
        cout << "\nPlease enter a word (type a single period '.' to terminate): ";
        cin >> userInput;           // save the word to userInput
        if (userInput != ".") {     // prevent the codes run in case of '.'
            if (hashTable.contain(userInput)) {
                cout << "* (The word is acceptable)\n";     // founded the word in hash table, it is acceptable
            }
            else {
                // the word is not found, suggesting..
                suggest(userInput, hashTable);
            }
        }
    }
}