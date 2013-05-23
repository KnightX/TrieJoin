/* TrieDynamic.cpp
1. Dynamically Creating TRIE on dataset;
2. Computing the ActiveNodeSet Dynamically using symmetric rules;
3. Output the similiar pair on the leaf node;
reference: <<Efficient trie-based string similarity joins with edit-distance constraints>>

Author: KnightX
Data: 2013-05-20*/

#include "TrieDynamic.h"

//create a node for Trie
struct Node* createNewNode(char ch)
{
  struct Node* new_node = new struct Node;
	new_node->ch = ch;
	new_node->label = ++node_label;
	
	return new_node;
}

/*create a empty Trie with root*/
struct Node* initialization()
{
	ROOT = createNewNode('#');
	ROOT->type  = 2;
	ROOT->level = 0;
	return ROOT;
}

/*character to index of array,[abcdefghijklmnopqrstuvwxyz0123456789_]*/
int charToindex(char ch)
{
	if ((ch < 123) && (ch > 96)) //a~z 
	{
		return ch - 97;
	}
	else if(ch > 47 && ch < 58)//0~9
	{
		return ch - 22;
	}
	else if(ch == '_') //"_"
	{
		return 36;
	}
}
struct Node* find(const char chars[],int len)
{
	struct Node* ptr = ROOT;
	int i = 0;
	
	while(i < len)
	{
		if(ptr->child[charToindex(chars[i])] == NULL)
		{
			break;
		}
		ptr = ptr->child[charToindex(chars[i])];
		i++;
	}
	
	if( (i == len) && ((ptr->type == 1)||(ptr->type == 3)))
		return ptr;
	else
		return NULL;
}


//Insert Node to Trie
void insert(char ch, struct Node * ptr, int type)
{
	int tempidx = charToindex(ch);

	if((ptr->type == LEAF) && (type == UNCOMPLETED)) 
	{
		ptr->type == COMPOSITE;
	}

	ptr->child[tempidx] = createNewNode(ch);
	ptr->child[tempidx]->type = type;
	ptr->child[tempidx]->level = ptr->level + 1;
	return;
}


//Computing Current Active Node Set
void ComputeRootActiveNode()
{	
	ROOT->activeNodeSet.insert(pair<struct Node*,int>(ROOT,0));
}

void ComputeActiveNode(struct Node*parent, struct Node* child)
{
	map<struct Node*,int> tempActiveNodeset = parent->activeNodeSet;
	map<struct Node*,int> tempActiveNodeset2;
	map<struct Node*,int> :: iterator tempActiveNodeSetit;
	
	tempActiveNodeset[child] = 0;
	
	for(tempActiveNodeSetit = tempActiveNodeset.begin();tempActiveNodeSetit != tempActiveNodeset.end();tempActiveNodeSetit++)
	{
		if(tempActiveNodeSetit->second + 1 <= threshold)
		{
			if(tempActiveNodeSetit->first == child)
			{
				tempActiveNodeset2.insert(pair<struct Node*,int>(tempActiveNodeSetit->first,tempActiveNodeSetit->second));
			}
			else
			{
				tempActiveNodeset2.insert(pair<struct Node*,int>(tempActiveNodeSetit->first,tempActiveNodeSetit->second + 1 ));
			}
		}
		
		for(int i = 0; i < MAX_NUM; i++)
		{
			if(tempActiveNodeSetit->first->child[i] != NULL)
			{
				if(tempActiveNodeSetit->first->child[i]->ch != child->ch) //mismatching
				{
					if(tempActiveNodeset[tempActiveNodeSetit->first] + 1 <= threshold)
					{
						tempActiveNodeset2.insert(pair<struct Node*,int>(tempActiveNodeSetit->first->child[i],tempActiveNodeset[tempActiveNodeSetit->first] + 1 ));
					}
				}
				else //matching
				{
					if(tempActiveNodeset[child] <= threshold)
					{
						tempActiveNodeset2.insert(pair<struct Node*,int>(tempActiveNodeSetit->first->child[i],tempActiveNodeset[tempActiveNodeSetit->first]));
							
						for(int index = 0 ; index < MAX_NUM; index++)
						{
							if(tempActiveNodeSetit->first->child[i]->child[index] != NULL)
							{
								if((tempActiveNodeSetit->first->child[i]->child[index]->level - tempActiveNodeSetit->first->child[i]->level)<= (threshold - tempActiveNodeset[tempActiveNodeSetit->first]))
								{
									tempActiveNodeset2.insert(pair<struct Node*,int>(tempActiveNodeSetit->first->child[i]->child[index],tempActiveNodeset[tempActiveNodeSetit->first] + tempActiveNodeSetit->first->child[i]->child[index]->level - tempActiveNodeSetit->first->child[i]->level));
								}
							}
						}
					}
				}
			}
		}
	}
	child->activeNodeSet = tempActiveNodeset2;
	return;
}

void UpdateActiveNodeset(struct Node* ptr)
{
	map<struct Node*,int> tempActiveNodeset = ptr->activeNodeSet;
	map<struct Node*,int> :: iterator tempActiveNodeSetit;
	
	for(tempActiveNodeSetit = tempActiveNodeset.begin();tempActiveNodeSetit != tempActiveNodeset.end();tempActiveNodeSetit++)
	{
		tempActiveNodeSetit->first->activeNodeSet.insert(pair<struct Node*,int> (ptr,tempActiveNodeSetit->second));
	}
	
}

/*OutputSimiliar Pair*/
void OutputSimiliar(struct Node* ptr)
{
	map<struct Node*,int> tempActiveNodeset = ptr->activeNodeSet;
  	map<struct Node*,int>::iterator setit;
  	
  	for(setit = tempActiveNodeset.begin(); setit != tempActiveNodeset.end(); setit++)
  	{
		if(ptr != (*setit).first)
		{
			if(((*setit).first->type == LEAF) || ((*setit).first->type == COMPOSITE))
			{
				cout << setit->second << " " << ptr->lineNum << " " << setit->first->lineNum << "\n";
				cout << ptr->content <<"\n";
				cout << setit->first->content << "\n\n";
  			}
  		}
  	}
}

/* PrintActiveNodeSet*/
void printActiveNodeset(struct Node *ptr)
{
	map<struct Node*,int> activens = ptr->activeNodeSet;
	map<struct Node*,int>::iterator setit;
		
	cout << ptr->label << "-" <<ptr->ch<< ":";
	
	for(setit = activens.begin(); setit!=activens.end(); setit++)
	{
       	cout<<(*setit).first->label <<"-"<<(*setit).first->ch <<"-" <<(*setit).second<<"-" <<(*setit).first->type << ",";
	}
	cout<< endl;
}

/*Usage*/
void usage()
{
	 cout<<"TrieDynamic [edit-distance] [filepath]\n";
	 return;
}

/*Main Function*/
int main(int argc,char* argv[])
{
	int count = 0;
	int length = 0;
	FILE *fp;
	char* filepath;
	char temp[128]={0};
	struct Node * tempnode;
	char ch;
	
	if(argc < 2)
	{
		usage();
		return -1;
	}	
	
	if(argc == 3)
	{
		threshold = atoi(argv[1]);
		filepath = argv[2];
	}
	
	if((ROOT = initialization()) == NULL)
	{
		cout<<"initialize the Trie error!\n";
		return -1;
	}
	
	if((fp = fopen(filepath,"r"))== NULL)
	{
		cout<<"can't not find file\n";
		return -1;
	}

	while(fgets(temp,128,fp) != NULL)
	{
		count++;
		length = strlen(temp);
		temp[length-1] = '\0';
		
		int lpp = 0;
		struct Node* lppNode = ROOT;
		ComputeRootActiveNode();
		
		while( lpp < length)
		{
			ch = temp[lpp];
			
			if((lppNode->child[charToindex(ch)]) != NULL)
			{
				lppNode = lppNode->child[charToindex(ch)];
				lpp++;
			}
			else
			{
				break;
			}
		}

		if(lpp < length - 1 )
		{
			for(int i = lpp ; i < length - 1; i++)
			{
			if( i < length - 2 ) //intermediate node
			{
				insert(temp[i],lppNode,UNCOMPLETED);
				ComputeActiveNode(lppNode, lppNode->child[charToindex(temp[i])]);
				UpdateActiveNodeset(lppNode->child[charToindex(temp[i])]);
				lppNode = lppNode->child[charToindex(temp[i])];
			}
			if(i == length - 2) // leaf node
			{
				insert(temp[i],lppNode,LEAF);
				ComputeActiveNode(lppNode, lppNode->child[charToindex(temp[i])]);
				UpdateActiveNodeset(lppNode->child[charToindex(temp[i])]);
				lppNode = lppNode->child[charToindex(temp[i])];
				lppNode->content = temp;
				lppNode->lineNum = count;
				OutputSimiliar(lppNode);
			}
		}
		}
		else if (lpp == length - 1 )
		{
			lppNode->content = temp;
			lppNode->type = COMPOSITE;
			lppNode->lineNum = count;
			lppNode->activeNodeSet.insert(pair<struct Node*,int>(lppNode,0));
			UpdateActiveNodeset(lppNode);
			OutputSimiliar(lppNode);
		}
		memset(temp,0,128);
	}

	cout<<"Trie is Inserted "<< count <<" Words!\n";
	
	return 0;  	
}
