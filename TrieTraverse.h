/* TrieTraverse.h
1. Create a Trie on dataset;
2. Trie-Traverse in Pre-order and compute the active-node set
3. Output the similiar pair on the leaf node
reference: <<Efficient trie-based string similarity joins with edit-distance constraints>>

Author: KnightX
Data: 2013-05-15*/


#ifndef TRIE_TRAVERSE_H
#define TRIE_TRAVERSE_H

#include <string>
#include <set>
#include <map>
#include <iostream>

using namespace std;

/*Array capacity (a~z + "0~9' + '_')*/
#define MAX_NUM 37 

/*Node Type In Trie*/
#define LEAF 1
#define UNCOMPLETED 2
#define COMPOSITE 3

/*Node struct for Trie*/
struct Node
{
  map<struct Node*,int> activeNodeSet; /*active node set*/
	struct Node* child[MAX_NUM]; /*array list of child*/
	string content;  /*leaf node 's string*/
	int type;        /*node type*/
	int level;       /*node level*/
	int label;       /*node label*/
	int lineNum;     /*line number*/
	char ch;         /*character*/
};

/*global variable*/
struct Node* ROOT;
int node_label = -1;
int threshold = 0;

/*Function Declaration*/
void usage();
int charToindex(char ch);
struct Node* createNewNode(char ch);
struct Node* initialization();
int insert(const char chars[],int len,int lineNum);
int  TravseWithPreOrder(struct Node *ptr);
void ComputeRootActiveNode();
void ComputeActiveNode(struct Node*parent, struct Node* child);
void printActiveNodeset(struct Node *ptr);
int  PruneAcitveNodeSet(struct Node* node);
void OutputSimiliar(struct Node* ptr);

#endif
