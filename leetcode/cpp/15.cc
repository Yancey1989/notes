#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> res;
    if (nums.size() < 2) return res;
    for (size_t k = 0; k < nums.size()-2; ++k) {
      if (nums[k] > 0 ) break;
      if (k > 0 && nums[k] == nums[k-1]) continue;
      int target = 0 - nums[k];
      int i = k + 1;
      int j = nums.size() - 1;
      while (i < j) {
        if (nums[i] + nums[j] == target) {
          res.push_back({nums[k], nums[i], nums[j]});
          while(i < j && nums[i] == nums[i+1]) ++i;
          while(i < j && nums[j] == nums[j-1]) --j;
          ++i;
          --j;
        } else if (nums[i] + nums[j] > target) {
          --j;
        } else {
          ++i;
        }
      }
    }
    return res;
  }
};

int main(int argc, char** argv) {
  Solution s;
  std::vector<int> nums = {-1, 0 , 1, 2, -1, -4};
  auto res = s.threeSum(nums);
  for (auto e : res) {
    std::cout << e[0] << "," << e[1] << "," << e[2] << std::endl;
  }
  return 0;
}