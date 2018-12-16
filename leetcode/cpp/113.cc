#include<iostream>
#include<vector>
using namespace std;
struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
  vector<vector<int>> pathSum(TreeNode* root, int sum) {
    std::vector<std::vector<int>> res;
    std::vector<int> path = {};
    if (root)
      searchDFS(root, 0, sum, res, path);
    return res;
  }

  void searchDFS(TreeNode *node, int sum, int target, std::vector<std::vector<int>> &res, std::vector<int>& path) {
    path.push_back(node->val);
    sum += node->val;

    if (node->left == nullptr && node->right == nullptr) {
      if (sum == target) res.push_back(path);
      return;
    }
    if (node->left) 
      searchDFS(node->left, sum, target, res, path);
    if (node->right)
      searchDFS(node->right, sum, target, res, path);
  }
};

int main(int argc, char** argv) {
  Solution s;
  TreeNode *root = new TreeNode(5);
  auto res = s.pathSum(root, 5);
  for (auto l : res) {
    for (auto e : l) {
      std::cout << e << " "; 
    }
    std::cout << std::endl;
  }
  return 0;
}