#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <stdexcept>

using namespace std;


template <typename T>
void displayInfo(string label, T value) {
    cout << label << ": " << value << endl;
}


class Tokenizer {
public:
    string clean(string raw) {
        string result = "";
        for (char c : raw) {
            if (isalnum(c)) result += tolower(c);
        }
        return result;
    }
};


class Query {
public:
    virtual void execute(map<string, int>& v1, map<string, int>& v2) = 0;
    virtual ~Query() {}
};


class TopKQuery : public Query {
private:
    int k;
public:
    TopKQuery(int val) : k(val) {}
    void execute(map<string, int>& v1, map<string, int>& v2) override {
        vector<pair<string, int>> words(v1.begin(), v1.end());
        sort(words.begin(), words.end(), [](auto& a, auto& b) {
            return b.second < a.second;
        });
        cout << "Top-" << k << " Words:" << endl;
        for (int i = 0; i < k && i < words.size(); i++) {
            cout << words[i].first << " -> " << words[i].second << endl;
        }
    }
};


class WordQuery : public Query {
private:
    string target;
public:
    WordQuery(string t) : target(t) {}
    
    
    void execute(map<string, int>& v1) {
        cout << "Count: " << v1[target] << endl;
    }

    
    void execute(map<string, int>& v1, map<string, int>& v2) override {
        int diff = abs(v1[target] - v2[target]);
        cout << "Difference for '" << target << "': " << diff << endl;
    }
};

class FileProcessor {
public:
    void process(string path, int kbSize, map<string, int>& counts) {
        ifstream file(path, ios::binary);
        if (!file) throw runtime_error("File not found: " + path);

        Tokenizer tk;
        int bufSize = kbSize * 1024;
        char* buffer = new char[bufSize];
        string leftover = "";

        while (file.read(buffer, bufSize) || file.gcount() > 0) {
            string content = leftover + string(buffer, file.gcount());
            leftover = "";
            string word = "";
            for (char c : content) {
                if (isspace(c) || ispunct(c)) {
                    if (!word.empty()) { counts[tk.clean(word)]++; word = ""; }
                } else { word += c; }
            }
            leftover = word;
        }
        if (!leftover.empty()) counts[tk.clean(leftover)]++;
        delete[] buffer;
    }
};

int main(int argc, char* argv[]) {
    string f1, f2, qType, target;
    int buf = 256, k = 0;

    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        if (a == "--file") f1 = argv[++i];
        else if (a == "--file1") f1 = argv[++i];
        else if (a == "--file2") f2 = argv[++i];
        else if (a == "--buffer") buf = stoi(argv[++i]);
        else if (a == "--query") qType = argv[++i];
        else if (a == "--word") target = argv[++i];
        else if (a == "--top") k = stoi(argv[++i]);
    }

    auto start = chrono::high_resolution_clock::now();
    map<string, int> v1, v2;
    FileProcessor engine;

    try {
        if (!f1.empty()) engine.process(f1, buf, v1);
        if (!f2.empty()) engine.process(f2, buf, v2);

        if (qType == "word") {
            WordQuery wq(target);
            wq.execute(v1);
        } else if (qType == "top") {
            TopKQuery tkq(k);
            tkq.execute(v1, v2);
        } else if (qType == "diff") {
            WordQuery wq(target);
            wq.execute(v1, v2);
        }

    } catch (exception& e) { cout << e.what() << endl; }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    displayInfo("Allocated Buffer (KB)", buf);
    displayInfo("Total Time (sec)", elapsed.count());

    return 0;
}