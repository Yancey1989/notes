#include <iostream>
#include <stack>

using namespace std;
class Solution {
public:
  bool isValid(string s) {
    std::stack<char> stack;
    for (auto c : s) {
      if (c == '(' || c == '{' || c == '[') {
        stack.push(c);
      } else {
        if (stack.empty()) return false;
        char prev = stack.top();
        if ((c == ')' && prev == '(') || (c == ']' && prev == '[') || (c == '}' && prev == '{')) {
          stack.pop();
          continue;
        }
        return false;
      }
    }
    return stack.empty();
  }
};

int main(int argc, char** argv) {
  Solution s;
  std::cout << s.isValid("{)") << std::endl;
  return 0;
}