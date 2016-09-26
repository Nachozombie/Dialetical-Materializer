#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <algorithm>
#include <vector>
#include <map>
#include <set>

using namespace std;

// Reads input to analyze from input.txt
// Outputs information to output.txt

// Reads sample dictionary from mood.txt, antonyms.txt, common.txt

#define INPUT "input.txt"
#define MOOD "mood.txt"
#define ANTONYM "antonym.txt"
#define COMMON "common.txt"
#define OUTPUT "output.txt"

vector<pair<string, string> > antonymList;
vector<vector<string> > moodList;
vector<string> commonList;
vector<string> inputList;

bool partialMatch(string word1, string word2) {
    string::iterator * its = new string::iterator[2];
    string::reverse_iterator * rits = new string::reverse_iterator[2];

    int index = -1;

    for (int i = 0; i < min(word1.length(), word2.length()); i++) {
        if (word1[i] == word2[i]) {
            continue;
        }

        index = i;
        break;
    }

    double value = (double)index / min(word1.length(), word2.length());

    if (value > 0.55 || value < 0) {
        return true;
    }

    return false;
}

bool oppositeMatch(string word1, string word2) {
    string start1 = word1.substr(0, 2);
    string start2 = word2.substr(0, 2);

    string end1 = (word1.size() > 4 ? word1.substr(word1.length()-5) : "");
    string end2 = (word2.size() > 4 ? word2.substr(word2.length()-5) : "");

    if (start1 == "un" || start1 == "in" || start1 == "im" || start1 == "ir" || start1 == "il") {
         return partialMatch(word1.substr(2), word2);
    } else if (start2 == "un" || start2 == "in" || start2 == "im" || start2 == "ir" || start1 == "il") {
        return partialMatch(word1, word2.substr(2));
    } else if (end1 == "less") {
        return partialMatch(word1.substr(0, word1.length()-4), word2);
    } else if (end2 == "less") {
        return partialMatch(word1, word2.substr(0, word2.length()-4));
    }

    return false;
}

bool antonymSearch(pair<string, string> first, pair<string, string> second) {
    if (partialMatch(first.first, second.first)) return false;
    //if (oppositeMatch(first.first, second.second)) return false;
    return (first.first < second.first);
}

bool sortBySecondElemBack(pair<string, int> first, pair<string, int> second) {
    return (first.second > second.second);
}

void identifyDifficultWords(ofstream & output) {
    output << "DifficultWords:" << endl;
    set<string> wordList;
    sort(commonList.begin(), commonList.end());
    for (string word : inputList) {
        bool found = binary_search(commonList.begin(), commonList.end(), word);
        if (!found) {
            wordList.emplace(word);
        }
    }

    for (string tempWord: wordList) {
        output << tempWord << endl;
    }
}

void analyzeConflict(ofstream & output) {
    output << "Conflict:" << endl;
    for (pair<string, string> ant : antonymList) {
        pair<string, string> opposite = make_pair(get<1>(ant), get<0>(ant));
        antonymList.push_back(opposite);
    }

    sort(antonymList.begin(), antonymList.end(), antonymSearch);

    for (vector<pair<string, string> >::iterator it = antonymList.begin(); it != antonymList.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }

    //cout << antonymList.size() << endl;

    vector<string> newInputList(inputList);
    sort(newInputList.begin(), newInputList.end());

    set<pair<string, string> > conflicts;

    for (int i = 0; i < newInputList.size(); i++) {
        for (int j = 0; j < antonymList.size(); j++) {
            if (oppositeMatch(newInputList[i], antonymList[j].first)) {
                newInputList[i] = antonymList[j].second;
            }
        }
    }

    for (string word : newInputList) {
        vector<pair<string, string> >::iterator it = lower_bound(antonymList.begin(), antonymList.end(), make_pair(word, ""), antonymSearch);
        vector<pair<string, string> >::iterator it_limit = upper_bound(antonymList.begin(), antonymList.end(), make_pair(word, ""), antonymSearch);
        if (it != antonymList.end()) {

            for (; it != it_limit; it++) {
                bool yes = binary_search(newInputList.begin(), newInputList.end(), it->second);

                if (yes) {
                    //cout << it->first << endl  << it->second << endl;
                    conflicts.emplace(make_pair(min(it->first, it->second), max(it->first, it->second)));
                }
            }
        }
    }

    for (pair<string, string> conf : conflicts) {
        output << conf.first << " vs. " << conf.second << endl;
    }


}

void analyzeMood(ofstream & output) {
    map<string, string> moodDual;
    map<string, int> moodCount;

    for (vector<string> mood : moodList) {
        string curMood = mood[0];
        for (int i = 1; i < mood.size(); i++) {
            moodDual.emplace(mood[i], curMood);
        }
        moodCount.emplace(curMood, 0);
    }

    for (string word : inputList) {
        if (moodDual.find(word) == moodDual.end()) {
            continue;
        }

        string theMood = moodDual[word];
        moodCount[theMood]++;
    }

    vector<pair<string, int> > moodCountList;

    for (map<string, int>::iterator it = moodCount.begin(); it != moodCount.end(); it++) {
        moodCountList.push_back(make_pair(it->first, it->second));
    }

    sort(moodCountList.begin(), moodCountList.end(), sortBySecondElemBack);

    output << "TextMood:" << endl;
    //cout << moodCountList.size() << endl;
    output << moodCountList[0].first << /*" and " << moodCountList[1].first << */endl;
}

void analyze(ofstream & output) {
    analyzeMood(output);
    analyzeConflict(output);
    identifyDifficultWords(output);
}

int main() {

    ifstream input(INPUT);
    ifstream mood(MOOD);
    ifstream antonym(ANTONYM);
    ifstream common(COMMON);
    ofstream output(OUTPUT);

    // Parsing input data

    bool negateNext = false;

    while (!input.eof()) {
        string temp;
        input >> temp;

        for (int i = 0; i < temp.size(); i++) {
            temp[i] = tolower(temp[i]);
            if (!isalpha(temp[i])) {
                temp = temp.substr(0, i) + temp.substr(i+1);
            }
        }

        if (negateNext) {
            temp = "un" + temp;
        }

        inputList.push_back(temp);

        if (temp == "not" || temp == "no") {
            negateNext = true;
        } else {
            negateNext = false;
        }
    }

    // Parsing mood data

    while (!mood.eof()) {
        string temp;
        mood >> temp;

        if (temp == "NewCategory") {
            vector<string> newMood;

            string tempWord;
            for (mood >> tempWord; tempWord != "EndCategory"; mood >> tempWord) {
                newMood.push_back(tempWord);
            }

            moodList.push_back(newMood);

        }

        //cout << moodList.size() << endl;
    }

    // Parsing antonyms

    while (!antonym.eof()) {
        string firstWord, secondWord;
        antonym >> firstWord >> secondWord;

        antonymList.push_back(pair<string, string>(firstWord, secondWord));
    }

    antonymList.erase(antonymList.end()-1);

    cout << antonymList.size() << endl;

    // Parsing common words

    while (!common.eof()) {
        string word;
        common >> word;

        commonList.push_back(word);
    }

    analyze(output);

    input.close();
    mood.close();
    antonym.close();
    common.close();
    output.close();

    return 0;
}
