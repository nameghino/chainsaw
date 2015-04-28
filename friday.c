// friday.c

#include <stdlib.h>
#include "jsmnSpark.h"

typedef struct ListNode {
  struct ListNode *parent;
  struct ListNode *next_sibling;
  struct ListNode *prev_sibling;
  struct ListNode *children;
  jsmntok_t *token;
} listnode_t;

listnode_t* create_node(jsmntok_t *token) {
  listnode_t *node = (listnode_t*)calloc(sizeof(listnode_t), 1);
  node->token = token;
  return node;
}

listnode_t* create_tree(jsmntok_t *tokens) {
  return NULL;
}

void delete_tree(listnode_t *root) {
  for (listnode_t *c = root->children;
    c != NULL;
    ++c) {
      delete_tree(c);
    }
    free(root);
}

int test() {
  return 0;
}

int main(int argc, char* argv[]) {
  return test();
}
