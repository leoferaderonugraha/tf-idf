/*TODO:
 * - Performance optimization (e.g. using caching mechanism)
 * - Refactor
 */

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <unordered_map>
#include <cmath>

namespace fs = std::filesystem;


void get_files(const std::string& path, std::vector<std::string>& result) {
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            get_files(entry.path(), result);
        } else {
            result.push_back(entry.path());
        }
    }
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);

    if (!file) {
        std::cerr << "Failed to open the file " << path << std::endl;
        return "";
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    return content;
}


std::vector<std::string> split_str(const std::string& s, char delim) {
    std::vector<std::string> result;

    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delim)) {
        result.push_back(token);
    }

    return result;
}


int word_count(const std::string& content) {
    int counter = 0;
    for (const auto& c : content) {
        if (c == ' ' || c == '\n') {
            counter++;
        }
    }

    return counter + 1;
}


bool starts_with(const std::string& s1, const std::string& s2) {
    if (s1.size() < s2.size()) {
        return false;
    }

    for (auto i = 0, size = (int) s2.size(); i < size; i++) {
        if (tolower(s1[i]) != tolower(s2[i])) {
            return false;
        }
    }

    return true;
}


bool ends_with(const std::string& s1, const std::string& s2) {
    int s1_size = s1.size();
    int s2_size = s2.size();

    if (s1_size < s2_size) {
        return false;
    }

    for (auto i = 0; i < s2_size; i++) {
        if (tolower(s1[s1_size - s2_size + i]) != tolower(s2[i])) {
            return false;
        }
    }

    return true;
}


double get_file_tf(const std::string& path, const std::string& term) {
    std::string content = read_file(path);

    if (content.empty()) {
        return .0;
    }

    int term_counter = 0;
    std::vector<std::string> words = split_str(content, ' ');

    if (words.empty()) {
        return .0;
    }

    for (const std::string& word : words) {
        if (starts_with(word, term)) {
            term_counter++;
        }
    }

    return term_counter / (double) words.size();
}


void populate_idf(std::unordered_map<std::string, double>& idf_map, const std::string& terms, const std::vector<std::string>& files) {
    for (const std::string& file : files) {
        std::string content = read_file(file);

        if (content.empty()) {
            continue;
        }

        for (const std::string& term : split_str(terms, ' ')) {
            for (const std::string& word : split_str(content, ' ')) {
                if (starts_with(word, term)) {
                    idf_map[term] = idf_map[term] + 1;
                    break;
                }
            }
        }
    }

    for (auto& [term, idf] : idf_map) {
        if (idf == 0) {
            idf = 1;
        }

        idf = log(files.size() / idf);
        std::cout << idf << std::endl;
    }
}


int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <dir> <keyword>" << std::endl;
        return 1;
    }

    std::string dir = argv[1];
    std::string keyword = argv[2];

    std::unordered_map<std::string, double> result;
    std::unordered_map<std::string, double> idf_map;

    std::shared_ptr<std::vector<std::string>> files = std::make_shared<std::vector<std::string>>();

    get_files(dir, *files);
    populate_idf(idf_map, keyword, *files);

    for (const auto& file : *files) {
        for (const std::string& term : split_str(keyword, ' ')) {
            double tf = get_file_tf(file, term);
            double idf = idf_map[term];
            result[file] = result[file] + (tf * idf);
        }
    }

    std::cout << "IDF:" << std::endl;
    for (auto& [term, idf] : idf_map) {
        std::cout << term << " " << idf << std::endl;
    }

    std::cout << "Score:" << std::endl;
    for (const auto& [file, score] : result) {
        std::cout << file << " " << score << std::endl;
    }

    std::string best_file = "";
    double best_score = .0;

    for (const auto& [file, score] : result) {
        if (score > best_score) {
            best_file = file;
            best_score = score;
        }
    }

    std::cout << "\nBest file: " << best_file << std::endl;

    return 0;
}
