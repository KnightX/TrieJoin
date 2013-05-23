/* TrieTraverse.cpp 
1. Create a Trie on dataset;
2. Trie-Traverse in Pre-order and compute the active-node set
3. Output the similiar pair on the leaf node
reference: <<Efficient trie-based string similarity joins with edit-distance constraints>>

Author: KnightX
Data: 2013-05-15*/

#include "TrieTraverse.h"

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
	ROOT->type  = UNCOMPLETED;
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
	else if(ch == '_')
	{
		return 36;
	}
}

//Insert Node to Trie
int insert(const char chars[],int len,int lineNum)
{
	struct Node* ptr = ROOT;
	int i = 0;
	
	while(i < len)
	{
		int tempidx = charToindex(chars[i]);
		
		if(ptr->child[tempidx] == NULL)  //child doesn't exist
		{
			if(ptr->type == LEAF)  // at leaf node
			{
				ptr->type = COMPOSITE;
			}
			
			if(i < (len - 1)) //intermediate node
			{
				ptr->child[tempidx] = createNewNode(chars[i]);
				ptr->child[tempidx]->type = UNCOMPLETED;
				ptr->child[tempidx]->level = ptr->level + 1;
			}
			if(i == (len - 1)) //leaf node
			{
				ptr->child[tempidx] = createNewNode(chars[i]);
				ptr->child[tempidx]->type = LEAF;
				ptr->child[tempidx]->lineNum = lineNum;
				ptr->child[tempidx]->level = ptr->level + 1;
				ptr->child[tempidx]->content = chars;
			}
			 ptr = ptr->child[tempidx];
		}
		
		else if(ptr->child[tempidx] !=  NULL) // the node already existed
		{
			if(i == (len - 1))
			{
				ptr->child[tempidx]->type = COMPOSITE;
				ptr->child[tempidx]->lineNum = lineNum;
				ptr->child[tempidx]->content = chars;
			}
			 ptr = ptr->child[tempidx];	
		}
		i++;
	}
	return 0;
}	

//Computing Current Active Node Set
void ComputeRootActiveNode(struct Node *ptr)
{
	map<struct Node*,int> tempActiveNodeset = ptr->activeNodeSet;
	map<struct Node*,int> :: iterator tempActiveNodeSetit;
	
	tempActiveNodeset.insert(pair<struct Node*,int>(ptr,0));
		
	for(tempActiveNodeSetit = tempActiveNodeset.begin();tempActiveNodeSetit != tempActiveNodeset.end();tempActiveNodeSetit++)
	{
		for(int index = 0 ; index < MAX_NUM; index++)
		{
			if(tempActiveNodeSetit->first->child[index] != NULL) 
			{
				if(tempActiveNodeSetit->first->child[index]->level <= threshold)
				{
					tempActiveNodeset.insert(pair<struct Node*,int>(tempActiveNodeSetit->first->child[index],tempActiveNodeSetit->first->child[index]->level));
				}
			}
		}
	}
		
	ROOT->activeNodeSet = tempActiveNodeset;
	//printActiveNodeset(ROOT);
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
	//printActiveNodeset(child);
	return;
}
	
/*Travse With PreOrder*/
int TravseWithPreOrder(struct Node* parptr, struct Node* chiptr)
{
	ComputeActiveNode(parptr,chiptr);
	
	for(int i = 0; i < MAX_NUM; i++)
	{
		if(chiptr->child[i] != NULL)
		{
			TravseWithPreOrder(chiptr,chiptr->child[i]);
		}
	}
	
	if((chiptr->type == LEAF) || (chiptr->type == COMPOSITE))
	{
		OutputSimiliar(chiptr);
	}
	return 0;
}

/*OutputSimiliar Pair*/
void OutputSimiliar(struct Node* ptr)
{
	//cout << "print" << ptr->label<<"-"<<ptr->ch;
	if((ptr->type == 1) || (ptr->type == 3))
  	{
  		map<struct Node*,int> tempActiveNodeset = ptr->activeNodeSet;
  		map<struct Node*,int>::iterator setit;
  	  
  		for(setit = tempActiveNodeset.begin(); setit != tempActiveNodeset.end(); setit++)
  		{
			if(ptr != setit->first)
			{
				if((setit->first->type == LEAF) || (setit->first->type == COMPOSITE))
				{
					if(ptr->lineNum < setit->first->lineNum)
					{
						cout << setit->second <<" "<< ptr->lineNum << " " << setit->first->lineNum <<"\n";
						cout << ptr->content <<"\n";
						cout << setit->first->content << "\n\n";
					}
  				}
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
       	cout<<(*setit).first->label <<"-"<<(*setit).first->ch <<"-" <<(*setit).second<<",";
	}
	cout<<"\n";
}

/*Usage*/
void usage()
{
	 cout << "TrieJoin [edit-distance] [filepath]\n";
	 return;
}

/*Main Function*/
int main(int argc,char* argv[])
{
	if(argc < 2)
	{
		usage();
		return -1;
	}	
	
	char* filepath;
	char temp[128]={0};
	int count =0;
	FILE *fp;
	
	if(argc == 3)
	{
		threshold = atoi(argv[1]);
		//cout<<"edit distance threshold:"<<threshold<<endl;
		
		filepath = argv[2];
		//cout<<"processing file:"<<filepath<<endl;	
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
		temp[strlen(temp)-1]='\0';
		insert(temp,strlen(temp),count);
		memset(temp,0,sizeof(temp));
	}

	cout<<"Trie is Inserted "<< count <<" Words!\n";
	
	ComputeRootActiveNode(ROOT);
	
	for(int i = 0; i < MAX_NUM; i++)
	{
		if(ROOT->child[i] != NULL)
		{
			TravseWithPreOrder(ROOT,ROOT->child[i]);
		}
	}
	     
	return 0;  	
}
