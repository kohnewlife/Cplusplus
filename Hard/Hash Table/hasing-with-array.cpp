//  Name: Vo Huy
//  Assignment number: 5
//  Assignment: Spell Checking with the Hash Table Data Structure
//  File name: huy.cpp
//  Date last modified: March 27, 2018
//  Honor statement: I have neither given nor received any unauthorized help on this assignment. 

#include <iostream>     // std::cout, std::fixed
#include <vector>       // std::vector
#include <string>
#include <queue>        // std::priority_queue
#include <cstddef>      // std::size_t
using namespace std;

// Each Node in HashTable
class HashNode {
private:
    string data;
    HashNode* next = nullptr;
public:
    /***    CONSTRUCTOR     ***/
    HashNode(string word): data(word) {}
    string getData() {
        return data;
    }
    // getters and setters
    HashNode* getNext() {
        return next;
    }
    void setData(string word) {
        this->data = word;
    }
    void setNext(HashNode* next) {
        this->next = next;
    }
};

// HashTable class
class HashTable {
private:
    HashNode **nodesArray;      // hash nodes array
    unsigned tableSize;         // hash table size
    unsigned numElement;        // number of elements
public:
    /***    CONSTRUCTOR     ***/
    HashTable (unsigned tableSize, string fileName) {
        this->tableSize = tableSize;
        numElement = 0;         // Intial size = 0

        // Initialize the size of hash table
        nodesArray = new HashNode*[tableSize];
        // initialize all nodes as null
        for (unsigned i = 0; i < tableSize; i++) 
            nodesArray[i] = nullptr;

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
    unsigned hash(string word) {
        const unsigned fnv_prime = 0x811C9DC5;
        unsigned hash = 0;      // hashing index

        for (unsigned i = 0; i < word.length(); i++)
        {
            hash *= fnv_prime;
            hash ^= (word[i]);
        }
        // return the index the string is mapping to
        return hash % tableSize;
    }

    // insert method inserts a string into the hash table
    // return true if it successfully inserts the word into the table
    // otherwise, it should return false if the word already is present in the table
    bool insert(string word) {
        // the index of the word in the hash table
        unsigned index = hash(word); 
        // the head at the index
        HashNode* ptr = nodesArray[index];
        // loop to the end of the list at the index
        while (ptr != nullptr) {
            if (ptr->getData() == word) 
                return false;       // the word already is present 
            ptr = ptr->getNext();
        }
        // HashNode* hashNode = new HashNode;       // FIXME: Do I need this or delete?
        // hashNode->setData(str);
        ptr = new HashNode(word);   // add a new node

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
        // the head at the index
        HashNode* ptr = nodesArray[index];
        // the node previous to the ptr
        HashNode* prev = nullptr;
        // loop to the end of the list at the index
        while (ptr != nullptr) {
            if (ptr->getData() == word) {   // found the node
                if (prev != nullptr) {
                    prev->setNext(ptr->getNext());   // rechain the list
                }
                delete ptr;                 // remove the node

                // successfully found and removed the word
                numElement--;
                return true;
            }
            prev = ptr;
            ptr = ptr->getNext();
        }
        return false;               // the word is not found
    }

    // returns true if the hash table contains a given string
    // otherwise, the method returns false if the string is not present
    bool contains(string word) { 
        // the index of the word in the hash table
        unsigned index = hash(word); 
        // the head at the index
        HashNode* ptr = nodesArray[index];
        // loop through the list at the index
        while(ptr != nullptr) {
            if (ptr->getData() == word) 
                return true;        // found the word
            ptr = ptr->getNext();
        }
        return false;               // the word is not found
    }

    // returns the total number of strings in the hash table
    unsigned size() {
        return numElement;
    }
};

// Ordering a to z for priority queue
class CompareAlphabets
{
public:
    bool operator()(string word1, string word2) {
        // return a is ordered after z
        return word1 > word2; 
    }
};

// checking for an omission
void checkOmission(string userInput, HashTable* hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& correctedWords) {  
    // Try adding a single letter at all possible places in the word
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 0; i <= userInput.length(); i++) {
        for (char j = 'a'; j <= 'z'; j++) {
            string testingWord = userInput;     // make a copy of userInput to test 
            if (hashTable->contains(testingWord.insert(i, string(1, j)))) { // FIXME: j doesn't work, has to convert j to string??
                correctedWords.push(testingWord);
            }
        }
    }
}

// checking for an extra letter
void checkExtra(string userInput, HashTable* hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& correctedWords) {
    // Try removing a single letter from the word
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 0; i < userInput.length(); i++) {
        string testingWord = userInput;         // make a copy of userInput to test
        if (hashTable->contains(testingWord.erase(i, 1))) {
            correctedWords.push(testingWord);
        }
    }
}

// checking for a typo
void checkTypo(string userInput, HashTable* hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& correctedWords) {
    // Try replacing an existing letter in the word with some other letter
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 0; i < userInput.length(); i++) {
        for (char j = 'a'; j <= 'z'; j++) {
            string testingWord = userInput;     // make a copy of userInput to test 
            if (hashTable->contains(testingWord.replace(i, 1, string(1, j)))) { // FIXME: j doesn't work, has to convert j to string??
                correctedWords.push(testingWord);
            }
        }
    }
}

// checking for a transposition
void checkTransposition(string userInput, HashTable* hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& correctedWords) {
    // Try transposing adjacent letters
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 0; i < userInput.length() - 1; i++) {
        string testingWord = userInput;         // make a copy of userInput to test
        swap(testingWord[i], testingWord[i + 1]);       // swapping 2 adjacent elements
        if (hashTable->contains(testingWord)) {
            correctedWords.push(testingWord);
        }
    }
}

// checking for a missing space
void checkMissingSpace(string userInput, HashTable* hashTable, 
    priority_queue<string, std::vector<string>, CompareAlphabets>& correctedWords) {
    // Try adding a single space at all possible places in the word
    // If any match a word in the hash table, push into the queue for printing later
    for (size_t i = 1; i < userInput.length(); i++) {
        string testingWord1 = userInput.substr(0, i);   // make a copy of the first word
        string testingWord2 = userInput.substr(i);      // make a copy of the second word
        if (hashTable->contain(testingWord1) && hashTable->contain(testingWord2)) {
            // both words match words in the hash table, add the word pair
            suggestedWords.push(testingWord1 + ' ' + testingWord2);
        } else if (hashTable->contain(testingWord1)) {
            suggestedWords.push(testingWord1);
        } else if (hashTable->contain(testingWord2)) {
            suggestedWords.push(testingWord2);
        }
    }
}

// Attempt to correct the user input
void correction(string userInput, HashTable* hashTable) {
    // Make an empty priority queue
    priority_queue<string, std::vector<string>, CompareAlphabets> correctedWords;
    // Note: The functions below CHANGE correctedWords
    // check if the user omitted a letter
    checkOmission(userInput, hashTable, correctedWords);
    // check if the user added an extraneous letter
    checkExtra(userInput, hashTable, correctedWords);
    // check if the user mistyped a letter
    checkTypo(userInput, hashTable, correctedWords);
    // check if the user tranposed 2 adjacent letter
    checkTransposition(userInput, hashTable, correctedWords);
    // check if the user forgot to add a space
    checkMissingSpace(userInput, hashTable, correctedWords);

    // print out corrected words in alphabetic order, if there's none, print ???
    if (correctedWords.empty()) {   // found none
        cout << "???";
    } else {
        while (!correctedWords.empty()) {
            cout << correctedWords.top() << "\n";
            correctedWords.pop();
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
                correction(userInput, hashTable);
            }
        }
    }
    
    // deallocate the hashTable
    delete hashTable;
    return 0;
}