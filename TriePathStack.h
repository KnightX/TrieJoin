/* TRIE-PATHSTACK.h
1. Create a Trie on dataset;
2. Trie-Traverse in Pre-order and compute the active-node set
3. Output the similiar pair on the leaf node
reference: <<Efficient trie-based string similarity joins with edit-distance constraints>>*/

#ifndef TRIE_PATHSTACK_H
#define TRIE_PATHSTACK_H

#include <string>
#include <map>
#include <stack>
#include <vector>
#include <iostream>

using namespace std;

#define MAX_NUM 37 //(a~z + "0~9' + '_')

/*Node Types in Trie*/
#define LEAF 1
#define UNCOMPLETED 2
#define COMPOSITE 3  // LEAF + INTERMEDIATE 

/*Node struct for Trie*/
struct Node
{
  struct Node* child[MAX_NUM]; /*array list of child*/
	string content;  /*leaf node 's string*/
	int type;        /*node type*/
	int level;       /*node level*/
	int label;       /*node label*/
	int lineNum;     /*line number*/
	char ch;         /*character*/
	bool bVisited;   /*visited*/
};

struct stackElement
{
	struct Node* ptr;
	map<struct Node*,int> activeNode;
};

/*global variable*/
struct Node* ROOT;
int node_label = -1;
int threshold = 0;

stack< stackElement, vector<stackElement> > TrieStack;
stack< stackElement, vector<stackElement> > TrieStackShadow;

/*Function Declaration*/
void usage();
int charToindex(char ch);
struct Node* createNewNode(char ch);
struct Node* initialization();
int insert(const char chars[],int len,int lineNum);
void printActiveNodeset(map<struct Node*,int> activenodeset);
int  TravseWithPreOrder(struct Node *ptr);
map<struct Node*,int> ComputeActiveNode(map<struct Node*,int> topActive, struct Node* child);
int  PruneAcitveNodeSet(struct Node* node);
void OutputSimiliar(struct Node* ptr,map<struct Node*,int> activeNodeset);
void UpdateStack(struct Node* node);
void printStackInfo(stack< stackElement, vector<stackElement> > triestack);

#endif
