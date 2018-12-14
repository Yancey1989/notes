#include <iostream>
#include <vector>
#include <stack>

using namespace std;

class Solution {
public:
  std::vector<std::string> letterCombinations(std::string digits) {
    std::vector<std::string> res;
    std::vector<std::string> dict  = {"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
    SearchDFSIteration(digits, dict, 0, "", res);
    return res;
  }
  void SearchDFSIteration(
   const std::string& digits, 
    const std::vector<std::string>& dict,
    size_t level,
    std::string path,
    std::vector<std::string>& res) {
    if (digits.size() == 0UL) {
      return;
    }

    if (res.size() == 0) res.push_back("");
    for (size_t i = 0; i < digits.size(); ++i) {
      std::string tmp = dict[digits[i] - '0'];
      std::vector<std::string> t;
      for (auto c : tmp) {
        for (auto s : res) {
          t.push_back(s + c);
        }
      }
      res = t;
    }
  }

  void SearchDFSRecursion(
    const std::string& digits, 
    const std::vector<std::string>& dict,
    size_t level,
    std::string path,
    std::vector<std::string>& res) {
    if (digits.size() == 0) {
      return;
    }
    if (level == digits.size()) {
      res.push_back(path);
      return;
    }
    std::string tmp = dict[digits[level] - '0'];
    for (auto c : tmp) {
      SearchDFSRecursion(digits, dict, level+1, path+c, res);
    }
  }
};

int main(int argc, char** argv) {
  Solution s;
  auto res = s.letterCombinations("23");
  for (auto s : res) {
    std::cout << s << std::endl;
  }
  return 0;
}