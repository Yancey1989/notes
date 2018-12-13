#include <iostream>
#include <map>

using namespace std;

class Solution {
public:
  int romanToInt(std::string s) {
    int sum = 0;
    dict_.emplace('I', 1);
    dict_.emplace('V', 5);
    dict_.emplace('X', 10);
    dict_.emplace('L', 50);
    dict_.emplace('C', 100);
    dict_.emplace('D', 500);
    dict_.emplace('M', 1000);
    if (s.size() == 0) return 0;
    sum += dict_[s[0]];
    for (size_t i = 1; i < s.size(); ++i) {
      int prev = dict_[s[i-1]];
      int curr = dict_[s[i]];
      if (prev < curr) {
        sum = sum - prev * 2 + curr;
      } else {
        sum += curr;
      }
    }
    return sum;
  }

private:
  std::map<char, int> dict_;
};

int main(int argc, char** argv) {
  Solution s;
  std::cout << s.romanToInt("IV");
}
