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

class Solution {
public:
    ListNode* splitByMid(ListNode *head) {
      ListNode *fast = head;
      ListNode *slow = head;
      while (true) {
        fast = fast->next;
        if (fast == NULL) break;
        fast = fast->next;
        if (fast == NULL) break;
        slow = slow->next;
      }
      ListNode *ret = slow->next;
      slow->next = nullptr;
      return ret;
    }

    ListNode* mergeSort(ListNode *left, ListNode* right) {
      ListNode *head = nullptr;
      if (left->val < right->val) {
        head = left;
        left = left->next;
      } else {
        head = right;
        right = right->next;
      }
      ListNode *ret = head;

      while (left != nullptr && right != nullptr) {
        if (left->val < right->val) {
          head->next = left;
          left = left->next;
        } else {
          head->next = right;
          right = right->next;
        }
        head = head->next;
      }
      if (left) {
        head->next = left;
      } else if (right) {
        head->next = right;
      }
      return ret;
    }

    ListNode* sortList(ListNode* head) {
      if (head == nullptr || head->next == nullptr) return head;
      ListNode *first = head;
      ListNode *mid = splitByMid(head);
      ListNode *left = sortList(first);
      ListNode *right = sortList(mid);
      return mergeSort(left, right);
    }
};

int main(int argc, char **argv) {
  std::vector<int> vec {4,2,3};

  ListNode *root = make_list(vec);
  Solution s;
  root = s.sortList(root);
  print_list(root);
  return 0;
}