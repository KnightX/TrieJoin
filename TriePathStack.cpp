/* TriePathStack.cpp 
1. Create a Trie on dataset;
2. Trie-Traverse in Pre-order and compute the active-node set
3. Output the similiar pair on the leaf node
reference: <<Efficient trie-based string similarity joins with edit-distance constraints>>

Author: KnightX
Data: 2013-05-21*/

#include "TriePathStack.h"

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

void UpdateStack(struct Node* node)
{
	struct stackElement *tempstackElement = new struct stackElement;
	struct stackElement  tempstackElement2;
	map<struct Node*,int>  topActiveNodeset;
	map<struct Node*,int>::iterator mapit;
	
	topActiveNodeset = TrieStack.top().activeNode;
	
	tempstackElement->ptr = node;
	tempstackElement->activeNode = ComputeActiveNode(topActiveNodeset,node);
	
	if((node->type == LEAF)||(node->type == COMPOSITE))
	{
		OutputSimiliar(node,tempstackElement->activeNode);
	}
	
	TrieStackShadow.push(*tempstackElement);
	
	/*update ancestor, need to be modified*/
	for(int i = 0; i < threshold; i++)
	{
		tempstackElement2 = TrieStack.top();
		TrieStack.pop();
		tempstackElement2.activeNode.insert(pair<struct Node*,int>(node,i+1));
		TrieStackShadow.push(tempstackElement2);
	}
	
	while(!TrieStackShadow.empty())
	{
		TrieStack.push(TrieStackShadow.top());
		TrieStackShadow.pop();
	}
}


map<struct Node*,int> ComputeActiveNode(map<struct Node*,int> topActive, struct Node* child)
{
	map<struct Node*,int> tempActiveNodeset = topActive;
	map<struct Node*,int> tempActiveNodeset2;
	map<struct Node*,int> :: iterator tempActiveNodeSetit;
	
	for(tempActiveNodeSetit = tempActiveNodeset.begin();tempActiveNodeSetit != tempActiveNodeset.end();tempActiveNodeSetit++)
	{
		if(tempActiveNodeSetit->first->bVisited == true)
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
					//cout << "called\n";
				}
			}
		
			for(int i = 0; i < MAX_NUM; i++)
			{
				if((tempActiveNodeSetit->first->child[i] != NULL) && (tempActiveNodeSetit->first->child[i]->bVisited == true))
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
								if((tempActiveNodeSetit->first->child[i]->child[index] != NULL )&&(tempActiveNodeSetit->first->child[i]->child[index]->bVisited == true))
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
	}
	tempActiveNodeset2.insert(pair<struct Node*,int>(child,0));
	return tempActiveNodeset2;
}

/*Travse With PreOrder*/
int TravseWithPreOrder(struct Node* ptr)
{
	int i;
	
	for(i = 0; i < MAX_NUM; i++)
	{
		if(ptr->child[i] != NULL)
		{
			ptr->child[i]->bVisited = true;
			UpdateStack(ptr->child[i]);
			TravseWithPreOrder(ptr->child[i]);
		}
	}
	
	if(!TrieStack.empty())
	{
		TrieStack.pop();
	}
	return 0;
}

/*OutputSimiliar Pair*/
void OutputSimiliar(struct Node* ptr,map<struct Node*,int> activeNodeset)
{
	map<struct Node*,int> tempActiveNodeset = activeNodeset;
  	map<struct Node*,int>::iterator setit;
  	  
  	for(setit = tempActiveNodeset.begin(); setit != tempActiveNodeset.end(); setit++)
  	{
		if(ptr != setit->first)
		{
			if((setit->first->type == LEAF) || (setit->first->type == COMPOSITE))
			{
				cout << setit->second <<" "<< ptr->lineNum << " " << setit->first->lineNum <<"\n";
				cout << ptr->content <<"\n";
				cout << setit->first->content << "\n\n";
  			}
  		}
  	}
}

void printActiveNodeset(map<struct Node*,int> activenodeset)
{
	map<struct Node*,int>::iterator setit;
		
	for(setit = activenodeset.begin(); setit!=activenodeset.end(); setit++)
	{
       	cout<<setit->first->label <<"-"<< setit->first->ch <<"-" <<setit->second<< ",";
	}
	cout<< endl;
}


/*PrintStackInfo*/
void printStackInfo(stack< stackElement, vector<stackElement> > triestack)
{
	map<struct Node*,int>::iterator it;
	
	while(!triestack.empty())
	{
		stackElement tempStackEle = triestack.top();
		cout << "<" << tempStackEle.ptr->label << ",";
		
		for(it = tempStackEle.activeNode.begin(); it!= tempStackEle.activeNode.end();it++)
		{
			cout << "(" << it->first->label << "," << it->second << ")";
		}
		cout << '>' <<"\n";
		triestack.pop();
	}
}

/*Usage*/
void usage()
{
	 cout << "TriePathStack [edit-distance] [filepath]\n";
	 return;
}

/*Main Function*/
int main(int argc,char* argv[])
{
	struct stackElement rootstackElement;
	int count = 0;
	FILE *fp;
	char* filepath;
	char temp[128]={0};
	
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

	while(fgets(temp,128,fp) != NULL)  //construct a new Trie
	{
		count++;
		temp[strlen(temp)-1]='\0';
		insert(temp,strlen(temp),count);
		memset(temp,0,sizeof(temp));
	}

	cout<<"Trie is Inserted "<< count <<" Words!\n";
	
	/*Initialize the ROOT */
	
	rootstackElement.ptr = ROOT;
	rootstackElement.activeNode.insert(pair<struct Node*,int>(ROOT,0));
	ROOT->bVisited = true;
	TrieStack.push(rootstackElement);
	
	TravseWithPreOrder(ROOT);
    
	return 0;  	
}
