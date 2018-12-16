#include <iostream>
#include <vector>
#include <limits.h>

using namespace std;
struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
  /**
   * O(NlogN)
   */
  ListNode* mergeKLists(vector<ListNode*>& lists) {
    if (lists.size() == 0) return nullptr;
    if (lists.size() == 1 && lists[0] == nullptr) return nullptr;
    std::vector<int> seq;
    for (size_t i = 0; i < lists.size(); ++i) {
      ListNode *l = lists[i];
      while (l) {
        seq.push_back(l->val);
        l = l->next;
      }
    }
    if (seq.size() == 0UL) return nullptr;
    sort(seq.begin(), seq.end());
    ListNode *res = new ListNode(seq[0]);
    ListNode *tail = res;
    for (size_t i = 1; i < seq.size(); ++i) {
      ListNode *t = new ListNode(seq[i]);
      tail->next = t;
      tail = tail->next;
    }
    return res;    
  }
  /**
   * O(N^2)
   */
  ListNode* mergeKLists_N2(vector<ListNode*>& lists) {
    if (lists.size() == 0UL) return nullptr;
    auto *node = findMinAndMove(lists);
    if (node == nullptr) return nullptr;
    ListNode *res = new ListNode(node->val);
    ListNode *tail = res;
    while ((node = findMinAndMove(lists)) && node) {
      ListNode *t = new ListNode(node->val);
      tail->next = t;
      tail = tail->next;
    }

    return res;    
  }

  ListNode* findMinAndMove(vector<ListNode*>& lists) {
    size_t idx = -1;
    int min = INT_MAX; 
    for (size_t i = 0; i < lists.size(); ++i) {
      if (lists[i] && lists[i]->val < min) {
        idx = i;
        min = lists[i]->val;
      }
    }
    if (idx == -1) return nullptr;
    ListNode *ret = lists[idx];
    lists[idx] = lists[idx]->next;
    return ret;
  }
};

int main(int argc, char** argv) {
  Solution s;
  std::vector<ListNode*> seq;
  s.mergeKLists(seq);
}