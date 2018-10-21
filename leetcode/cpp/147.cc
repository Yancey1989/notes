#include <iostream>
#include <vector>
using namespace std;

struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};
void print_list(ListNode *root) {
  while(root!=NULL) {
    std::cout << root->val << " ";
    root = root->next;
  }
  std::cout <<  std::endl << "-----" << std::endl;;
}

class Solution {
public:

  ListNode* insertionSortList(ListNode* head) {
    if (head == NULL) return head;
    ListNode* ele = head->next;
    ListNode* ele_prev = head;

    while (ele != NULL) {
      ListNode *iter = head;
      ListNode *iter_prev = head;

      while(iter->val <= ele->val && iter != ele) {
        if (iter_prev != iter)  iter_prev = iter_prev->next;
        iter = iter->next;
      }

      if (iter == ele) {
        ele = ele->next;
        ele_prev = ele_prev->next;
      } else {
        ele_prev->next = ele->next;
        ListNode *tmp = ele;
        ele = ele->next;
        tmp->next = iter;

        if (iter == iter_prev) {
          head = tmp;
        } else {
          iter_prev->next = tmp;
        }
      }
    }

    return head;
  }
};

ListNode* make_list(const std::vector<int>& vec) {
  ListNode *root = new ListNode(vec[0]);
  ListNode *node = root;
  for (size_t i = 1; i < vec.size(); ++i) {
    ListNode *tmp= new ListNode(vec[i]);
    node->next = tmp;
    node = node->next;
  }
  return root;
}


int main(int argc, char** argv) {
  std::vector<int> vec {4,2,1,3};

  ListNode *root = make_list(vec);
  Solution s;
  print_list(root);
  auto t = s.insertionSortList(root);
  std::cout << "after insert sort:" << std::endl;
  print_list(t);
}