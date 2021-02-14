/**
 * Example showing how a dynamically allocated tree structure can be
 * created in C code.
 *
 * Course: High Performance Programming, Uppsala University
 *
 * Author: Elias Rudberg <elias.rudberg@it.uu.se>
 *
 */

#include <stdio.h>
#include <stdlib.h>

struct treeNode_ {
  int x;
  struct treeNode_* child_A;
  struct treeNode_* child_B;
};
typedef struct treeNode_ treeNode;

void print_tree(treeNode* rootNode, int level) {
  int i;
  for(i = 0; i < level; i++)
    printf("   ");
  printf("x: %d\n", rootNode->x);
  if(rootNode->child_A)
    print_tree(rootNode->child_A, level+1);
  if(rootNode->child_B)
    print_tree(rootNode->child_B, level+1);
}

void free_tree(treeNode* rootNode) {
  if(rootNode->child_A)
    free_tree(rootNode->child_A);
  if(rootNode->child_B)
    free_tree(rootNode->child_B);
  free(rootNode);
}

int main() {
  printf("Hello!\n");

  treeNode* rootNode = (treeNode*)malloc(sizeof(treeNode));

  treeNode* child1 = (treeNode*)malloc(sizeof(treeNode));
  treeNode* child2 = (treeNode*)malloc(sizeof(treeNode));

  treeNode* child11 = (treeNode*)malloc(sizeof(treeNode));
  treeNode* child12 = (treeNode*)malloc(sizeof(treeNode));

  treeNode* child21 = (treeNode*)malloc(sizeof(treeNode));
  treeNode* child22 = (treeNode*)malloc(sizeof(treeNode));
  
  /* Create child11. */
  child11->x = 55;
  child11->child_A = NULL;
  child11->child_B = NULL;
  
  /* Create child12. */
  child12->x = 33;
  child12->child_A = NULL;
  child12->child_B = NULL;

  /* Create child1. */
  child1->x = 88;
  child1->child_A = child11;
  child1->child_B = child12;

  /* Create child21. */
  child21->x = 222;
  child21->child_A = NULL;
  child21->child_B = NULL;
  
  /* Create child22. */
  child22->x = 123;
  child22->child_A = NULL;
  child22->child_B = NULL;

  /* Create child2. */
  child2->x = 99;
  child2->child_A = child21;
  child2->child_B = child22;

  /* Create rootNode. */
  rootNode->x = 77;
  rootNode->child_A = child1;
  rootNode->child_B = child2;

  print_tree(rootNode, 0);
  free_tree(rootNode);

  printf("Goodbye!\n");
  return 0;
}
