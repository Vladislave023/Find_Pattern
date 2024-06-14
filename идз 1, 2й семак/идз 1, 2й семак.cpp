#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace chrono;

struct Key {
    int s;
    struct Group {
        char l;
        int num;
    } group;
    string fio;
    struct Direction {
        string title;
    } dir;
};

const int alphabet_size = 256;
const int pattern_max_len = 100;

void bad_symbol(const string& pattern, int bad_symbols[]) {
    int pattern_len = pattern.length();
    for (int i = 0; i < alphabet_size; ++i) {
        bad_symbols[i] = pattern_len;
    }
    for (int j = 0; j < pattern_len - 1; ++j) {
        bad_symbols[static_cast<unsigned char>(pattern[j])] = pattern_len - j - 1;
    }
}

void good_suffix(const string& pattern, int good_suffixs[]) {
    int pattern_len = pattern.length();
    int f[pattern_max_len];
    for (int i = 0; i < pattern_len; ++i) {
        good_suffixs[i] = 0;
    }
    f[pattern_len] = pattern_len + 1;
    for (int i = pattern_len; i > 0; --i) {
        int j = pattern_len + 1;
        while ((j <= pattern_len) && (pattern[i - 1] != pattern[j - 1])) {
            if (good_suffixs[j] == 0) {
                good_suffixs[j] = j - 1;
            }
            j = f[j];
        }
        f[i - 1] = --j;
    }
    int p = f[0];
    for (int j = 0; j <= pattern_len; ++j) {
        if (good_suffixs[j] == 0) {
            good_suffixs[j] = p;
        }
        if (j == p) {
            p = f[p];
        }
    }
}

int max_for_TBM(int a, int b, int c) {
    if (a >= b && a >= c) return a;
    if (b >= a && b >= c) return b;
    return c;
}

int TBM(const string& str, const string& pattern, int good_suffixs[], int bad_symbols[], int min_count) {
    int str_len = str.length();
    int pattern_len = pattern.length();
    int count = 0;
    int str_index = 0;
    int good_suffix_rem = 0;
    int pattern_shift = pattern_len;
    while (str_index <= str_len - pattern_len) {
        int pattern_index = pattern_len - 1;

        while ((pattern_index >= 0) && (pattern[pattern_index] == str[str_index + pattern_index])) {
            --pattern_index;
            if ((good_suffix_rem != 0) && (pattern_index == pattern_len - 1 - pattern_shift)) {
                pattern_index -= good_suffix_rem;
            }
        }
        if (pattern_index < 0) {
            count++;
            if (count >= min_count) {
                return true;
            }
            str_index += pattern_shift;
            good_suffix_rem = 0;
        }
        else {
            int v = pattern_len - 1 - pattern_index;
            int turbo_shift = good_suffix_rem - v;
            int bad_symbol_shift = bad_symbols[static_cast<unsigned char>(str[str_index + pattern_index])];
            pattern_shift = max_for_TBM(turbo_shift, bad_symbol_shift, good_suffixs[pattern_index + 1]);
            if (pattern_shift == good_suffixs[pattern_index + 1]) {
                if ((pattern_len - pattern_shift) < v) {
                    good_suffix_rem = pattern_len - pattern_shift;
                }
                else {
                    good_suffix_rem = v;
                }
            }
            else {
                if (turbo_shift < bad_symbol_shift) {
                    if (pattern_shift < (good_suffix_rem + 1)) {
                        pattern_shift = good_suffix_rem + 1;
                    }
                }
                good_suffix_rem = 0;
            }
            str_index += pattern_shift;
        }
    }
    return false;
}

void read_file(vector<Key>& arr, const string& filename) {
    ifstream in(filename);
    if (in.is_open()) {
        string line;
        int count = 0;
        while (getline(in, line)) {
            count += 1;
            stringstream ss(line);
            Key k;

            ss >> k.fio;
            string middle_name, last_name;
            ss >> middle_name >> last_name;
            k.fio += " " + middle_name + " " + last_name;

            string group;
            ss >> group;
            k.group.l = group[0];
            k.group.num = stoi(group.substr(1));

            ss >> k.dir.title;

            if (ss.fail()) {
                cerr << "Error parsing line: " << line << endl;
                continue;
            }

            k.s = count;
            arr.push_back(k);
        }
        in.close();
    }
    else {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
}

void write_file(const vector<Key>& arr, duration<double> diff, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& k : arr) {
            file << "Line number: " << k.s << "\n";
            file << "Line content: " << k.fio << " " << k.group.l << k.group.num << " " << k.dir.title << "\n";
            cout << "Line number: " << k.s << "\n";
            cout << "Line content: " << k.fio << " " << k.group.l << k.group.num << " " << k.dir.title << "\n";
        }
        file << fixed << setprecision(6) << diff.count() << " s" << "\n";
        cout << fixed << setprecision(6) << diff.count() << " s" << "\n";
        file.close();
    }
    else {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
}

void Call_Turbo_Boyer_Moore() {
    vector<Key> arrTBM;
    vector<Key> answer;
    read_file(arrTBM, "input.txt");
    long n = arrTBM.size();

    string const pattern_fio = "Ivan";
    int good_fio_suffixs[pattern_max_len], bad_fio_symbols[alphabet_size];
    good_suffix(pattern_fio, good_fio_suffixs);
    bad_symbol(pattern_fio, bad_fio_symbols);

    string const pattern_title = "i";
    int good_title_suffix[pattern_max_len], bad_title_symbols[alphabet_size];
    good_suffix(pattern_title, good_title_suffix);
    bad_symbol(pattern_title, bad_title_symbols);

    int min_count_fio = 3;
    int min_count_title = 1;

    auto startTBM = high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        string str_fio = arrTBM[i].fio;
        string str_title = arrTBM[i].dir.title;

        if (TBM(str_fio, pattern_fio, good_fio_suffixs, bad_fio_symbols, min_count_fio) &&
            TBM(str_title, pattern_title, good_title_suffix, bad_title_symbols, min_count_title)) {
            answer.push_back(arrTBM[i]);
        }
    }
    auto endTBM = high_resolution_clock::now();
    duration<double> diffTBM = endTBM - startTBM;
    write_file(answer, diffTBM, "outTBM.txt");
}

int simpleSearch(const string& str, const string& pattern) {
    int i = 0, k = 0, count = 0, p_size = pattern.length();
    bool flag = 0;
    for (int i = 0; i < str.length(); i++) {
        k = 0;
        flag = 0;
        while ((str[i] == pattern[k]) and (!flag) and (i < str.length())) {
            if (k == p_size - 1) {
                count++;
                k = 0;
                flag = 1;
            }
            else {
                k++;
                i++;
            }
        }
    }

    return count;
}

void Call_Simple_Search(const string& inputFile = "input.txt", const string& outputFile = "outSimpleSearch.txt") {
    vector<Key> arrSimpleSearch;
    vector<Key> answer;
    read_file(arrSimpleSearch, inputFile);
    long n = arrSimpleSearch.size();

    string const pattern_fio = "Ivan";
    string const pattern_title = "i";
    int count_fio = 3;
    int count_title = 1;

    auto startSimpleSearch = high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        string str_fio = arrSimpleSearch[i].fio;
        string str_title = arrSimpleSearch[i].dir.title;

        if (simpleSearch(str_fio, pattern_fio) >= count_fio &&
            simpleSearch(str_title, pattern_title) >= count_title) {
            answer.push_back(arrSimpleSearch[i]);
        }
    }
    auto endSimpleSearch = high_resolution_clock::now();
    duration<double> diffSimpleSearch = endSimpleSearch - startSimpleSearch;
    write_file(answer, diffSimpleSearch, outputFile);
}

int main() {
    Call_Turbo_Boyer_Moore();
    Call_Simple_Search();
    return 0;
}
