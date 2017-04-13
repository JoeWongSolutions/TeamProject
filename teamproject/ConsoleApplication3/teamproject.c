#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "MU_HashTable.h"
#include "MU_HashTableSimpleBiDi.h"

#define MY_TEMP_STRING_MAX_LEN	512
#define MY_MAX_NODE_NUMBER		2000

#ifndef __LONG_MAX__
#define __LONG_MAX__         2147483647L
#endif

#define		INFINITY_VALUE		__LONG_MAX__

typedef struct
{
	char  *relationName;
	long   distance;
	int    isBidirectional;
} MyRelation;

// Only for generating graphviz dot file
typedef struct
{
	char  *pairName;
	char  *targetName;
	char  *sourceName;
	long   distance;
	int    isBidirectional;
	int    isShortestPath;
} MyGraphviz;

typedef struct
{
	int		nodeCount;                        // here the range is [1..nodeCount], not [0..nodeCount-1]. Why ? Let's reserve 0 for NULL
	int		edgeCount;
	struct MU_HashTable *relations;
	struct MU_HashTableSimpleBiDi *bidiNodes; // bi-directional hash table; node_name for Key, node_index for Value
	struct MU_HashTable *distances;           // use two levels of hashtables to save distances, 
											  // the first level is the rows(Key is for row index[1..nodeCount]; Value is for column_hash_table), 
											  // the second level is the column_hash_table(Key is for column index[1..nodeCount]; Value is for distance)
	struct MU_HashTable *graphviz;
} MyGraph;

// Function declarations
static int          InitGraph(MyGraph *graph);
static void         FreeGraph(MyGraph *graph);

static int          InputGraphRelations(MyGraph *graph);
static void         InputGraphData(MyGraph *graph);

static int          IndexOfNode(const MyGraph *graph, char *node);
static long			getDistanceBetweenNodes(const MyGraph *graph, int idsrc, int iddest);
static void         PrintGraph(const MyGraph *graph);

static void         Dijkstra(const MyGraph *graph, int idsrc, int iddest);

static void         updateGraphvizShortestPath(MyGraph *graph, int idsource, int idtarget);
static void         makeGraphvizDOT(MyGraph *graph);

static char        *trimspace(char *str);
static char        *my_safe_gets(char *buffer, size_t buflen);

// For Visual Studio compatibility
#ifdef _MSC_VER
#define snprintf	 _snprintf
#define strncasecmp  strnicmp
#define strcasecmp   stricmp
#endif

// For Linux/Unix compatibility
#ifdef __GNUC__
char *strlwr(char *str)
{
	char *p = str;

	if (str == NULL)
		return NULL;

	while (*p != '\0')
	{
		if (*p >= 'A' && *p <= 'Z')
			*p = (*p) + 0x20;

		p++;
	}

	return str;
}
#endif




// Main function
int main(int argc, char* argv[])
{
	MyGraph   graph;
	int idsource;
	int iddestination;
	char str[MY_TEMP_STRING_MAX_LEN];

	if (InitGraph(&graph) == 0)
	{
		printf("Graph initialization error!\n");
		return 0;
	}

	if (InputGraphRelations(&graph) <= 0)
	{
		printf("No relations found, check relation data file!\n");
		return 0;
	}

	printf("Creating the graph, please input node-pair line by line, \nformat: Name1 | Name2 | Relation\n");

	InputGraphData(&graph);

	PrintGraph(&graph);

	do 
	{
		printf("\n\nTo calculate the shortest path, please input the source node: ");
		if (NULL == my_safe_gets(str, sizeof(str)))
			continue;
		idsource = IndexOfNode(&graph, trimspace(str));
	} while (idsource == -1);

	do 
	{
		printf("\n\nPlease input the destination node: ");
		if (NULL == my_safe_gets(str, sizeof(str)))
			continue;
		iddestination = IndexOfNode(&graph, trimspace(str));
	} while (iddestination == -1);

	Dijkstra(&graph, idsource, iddestination);

	makeGraphvizDOT(&graph);

	// Free distances data
	for (int i = 1; i <= graph.nodeCount; i++)
	{
		struct MU_HashTable *row = MU_HashTable_search(graph.distances, (void *)i);
		if (row)
		{
			for (int j = 1; j <= graph.nodeCount; j++)
				MU_HashTable_delete(row, (void *)j, 0, 0);

			MU_HashTable_destroy(row, 0, 0);
		}
	}

	// Free the graph
	FreeGraph(&graph);

	return 0;
}

/*
 * Function:  Graph initialization
 * Parameters:
 *		[in] graph
 * Return: 0 if any error occur; 1 on success
 */
int InitGraph(MyGraph *graph)
{
	memset(graph, 0, sizeof(MyGraph));

	graph->relations = MU_HashTable_create(NULL, NULL, 100);
	graph->bidiNodes = MU_HashTableSimpleBiDi_create(150);
	graph->distances = MU_HashTable_create(MU_HashTable_index_hash, MU_HashTable_index_hash_cmp, MY_MAX_NODE_NUMBER);
	graph->graphviz  = MU_HashTable_create(NULL, NULL, 100);

	if (graph->relations == NULL || graph->bidiNodes == NULL || graph->distances == NULL || graph->graphviz == NULL)
	{
		FreeGraph(graph);
		return 0;
	}

	return 1;
}

/*
* Function:  Destroy graph
* Parameters:
*		[in] graph
*/
void FreeGraph(MyGraph *graph)
{
	if (graph->relations)
	{
		MU_HashTable_destroy(graph->relations, 1, 1);
		graph->relations = NULL;
	}

	if (graph->bidiNodes)
	{
		MU_HashTableSimpleBiDi_destroy(graph->bidiNodes);
		graph->bidiNodes = NULL;
	}

	if (graph->distances)
	{
		MU_HashTable_destroy(graph->distances, 0, 0);
		graph->distances = NULL;
	}

	if (graph->graphviz)
	{
		MU_HashTable_destroy(graph->graphviz, 1, 2);
		graph->graphviz = NULL;
	}
}

/*
 * Function:  Read graph relations from the relation data file
 * Parameters:
 *		[in] graph
 * Return: numbers of relations, or 0 if any error occur
 * Format of the relation data file: Each line is a relation. Each relation's format(case-insensitive) is: [relation string], [relation distance number], [whether relation is bidirectional(optional)]
 * Note: distance should be greater than 0
 * Here is some examples of the relation data file:
 *		parent, 20, true        (means A is B's parent, father or mother)
 * 		friend, 18, true
 * 		classmate, 14, true
 * 		neighbor, 10, true
 * 		schoolmate, 5, true
 *		know, 3                 (means A knows B, but B doesn't know A)
 *		know, 3, false          (means A knows B, but B doesn't know A)
 *		knoweachother, 3, true  (means A knows B, and B knows A too)
 *
 */
int InputGraphRelations(MyGraph *graph)
{
	FILE *fin = fopen("input.relation", "rt");
	char buf[MY_TEMP_STRING_MAX_LEN + 1];
	char *p1, *p2, *p3;
	int count = 0;
	int distance;
	int isbidirectional;

	while (fin && !feof(fin)) 
	{
		// read one line
		p1 = fgets(buf, MY_TEMP_STRING_MAX_LEN, fin);
		if (p1 == NULL)
			break;

		// skip the spaces
		while (*p1 == ' ')
		{
			p1++;
			continue;
		}

		if ((p2 = strchr(p1, ',')) == NULL)
			continue;

		*(p2++) = 0;

		// skip the spaces
		while (*p2 == ' ')
		{
			p2++;
			continue;
		}

		distance = atoi(p2);
		isbidirectional = 0;

		if (strlen(p1) <= 0 || distance <= 0.0)
			continue;

		if ((p3 = strchr(p2, ',')) != NULL)
		{
			*(p3++) = 0;

			// skip the spaces
			while (*p3 == ' ')
			{
				p3++;
				continue;
			}

			if (strncasecmp(p3, "true", 4) == 0)
				isbidirectional = 1;
		}

		MyRelation *r = (MyRelation *)malloc(sizeof(MyRelation));
		if (r == NULL)
		{
			printf("malloc error!\n");
			break;
		}

		r->relationName = strdup(strlwr(p1));
		r->distance = distance;
		r->isBidirectional = isbidirectional;

		if (r->relationName == NULL)
		{
			printf("strdup error!\n");
			free(r);
			break;
		}

		// user "r->relationName" as key, so we don't need to free it manually (MU_HashTable_destroy will free it)
		if (MU_HashTable_insert(graph->relations, r->relationName, r) < 0)
			break;

		count++;
	}

	if (fin)
		fclose(fin);

	return count;
}

/*
 * Function:  Input the graph data
 * Parameters:
 *		[in] graph
 * Return: None
 */
void InputGraphData(MyGraph *graph)
{
	char inputstr[MY_TEMP_STRING_MAX_LEN];
	char *pnodeA, *pnodeB, *prelation;

	int  nodeAindex, nodeBindex;
	struct MU_HashTable *row;

	if (graph == NULL)
		return;

	graph->nodeCount = 0;
	graph->edgeCount = 0;

	// Input node values, format: [nodeA] | [nodeB] | [relation]
	// A blank line will end the input
	while(1)
	{
		my_safe_gets(inputstr, sizeof(inputstr));

		if (strlen(inputstr) <= 0)
			break;

		if ((pnodeB = strchr(inputstr, '|')) == NULL)
			continue;

		*(pnodeB++) = 0;

		if ((prelation = strchr(pnodeB, '|')) == NULL)
			continue;

		*(prelation++) = 0;

		prelation = strlwr(trimspace(prelation));
		pnodeB = strdup(trimspace(pnodeB));
    	pnodeA = strdup(trimspace(inputstr));

		if (pnodeA == NULL || pnodeB == NULL || strlen(pnodeA) <= 0 || strlen(pnodeB) <= 0 || strlen(prelation) <= 0)
		{
			printf("Bad input\n");
			continue;
		}

		MyRelation *rel = MU_HashTable_search(graph->relations, prelation);
		if (rel == NULL)
		{
			printf("Bad relation\n");
			continue;
		}

		// Get index of the node, if the node is new, insert it into the bidirectional hash table
		nodeAindex = IndexOfNode(graph, pnodeA);
		if (nodeAindex == -1) 
		{
			if (0 != MU_HashTableSimpleBiDi_insert(graph->bidiNodes, pnodeA, graph->nodeCount + 1))
			{
				MU_HashTableSimpleBiDi_delete(graph->bidiNodes, pnodeA, graph->nodeCount + 1); // be safe to delete it manually in case of insert failure
				printf("Adding node error\n");
				continue;
			}

			nodeAindex = graph->nodeCount + 1;
			graph->nodeCount++;
		}

		// Get index of the node, if the node is new, insert it into the bidirectional hash table
		nodeBindex = IndexOfNode(graph, pnodeB);
		if (nodeBindex == -1) 
		{
			if (0 != MU_HashTableSimpleBiDi_insert(graph->bidiNodes, pnodeB, graph->nodeCount + 1))
			{
				MU_HashTableSimpleBiDi_delete(graph->bidiNodes, pnodeB, graph->nodeCount + 1); // be safe to delete it manually in case of insert failure
				printf("Adding node error\n");
				continue;
			}

			nodeBindex = graph->nodeCount + 1;
			graph->nodeCount++;
		}

		// if current row doesn't exist, create and insert it
		row = MU_HashTable_search(graph->distances, (void *)nodeAindex);
		if (row == NULL)
		{
			if ((row = MU_HashTable_create(MU_HashTable_index_hash, MU_HashTable_index_hash_cmp, MY_MAX_NODE_NUMBER)) == NULL)
			{
				printf("Creating row error\n");
				continue;
			}

			if (0 != MU_HashTable_insert(graph->distances, (void *)nodeAindex, row))
			{
				printf("Adding row error\n");
				MU_HashTable_destroy(row, 0, 1);
				continue;
			}
		}

		if (0 != MU_HashTable_insert(row, (void *)nodeBindex, (void *)(rel->distance)))
		{
			printf("Adding column error\n");
			continue;
		}

		graph->edgeCount++;

		// For undirected graph, set distance for nodeB to nodeA
		if (rel->isBidirectional)
		{
			row = MU_HashTable_search(graph->distances, (void *)nodeBindex);
			if (row == NULL)
			{
				if ((row = MU_HashTable_create(MU_HashTable_index_hash, MU_HashTable_index_hash_cmp, MY_MAX_NODE_NUMBER)) == NULL)
				{
					printf("Creating row error\n");
					continue;
				}

				if (0 != MU_HashTable_insert(graph->distances, (void *)nodeBindex, row))
				{
					printf("Adding row error\n");
					MU_HashTable_destroy(row, 0, 1);
					continue;
				}
			}

			if (0 != MU_HashTable_insert(row, (void *)nodeAindex, (void *)(rel->distance)))
			{
				printf("Adding column error\n");
				continue;
			}
		}

		MyGraphviz *gv = (MyGraphviz *)malloc(sizeof(MyGraphviz));
		if (gv)
		{   
			snprintf(inputstr, MY_TEMP_STRING_MAX_LEN - 1, "%d %d", nodeAindex, nodeBindex);
			gv->pairName = strdup(inputstr); // MU_HashTable_destroy will free it
			gv->distance = rel->distance;
			gv->isBidirectional = rel->isBidirectional;
			gv->sourceName = pnodeA;
			gv->targetName = pnodeB;
			gv->isShortestPath = 0;

			MU_HashTable_insert(graph->graphviz, gv->pairName, gv);
		}
	}
}

/*
 * Function:  Print the distance matrix of the graph
 * Parameter:  [in] graph
 * Return: None
 */
void    PrintGraph(const MyGraph *graph)
{
	long distance;

	if (graph == NULL)
		return;

	printf("\nPrint the graph data:\n");

	for (int m = 1; m <= graph->nodeCount; m++)
	{
		for (int n = 1; n <= graph->nodeCount; n++)
		{
			distance = getDistanceBetweenNodes(graph, m, n);
			if (distance <= 0)
				printf("    ∞,\t");
			else
				printf("%6d,\t", distance);
		}

		printf("\n\n");
	}
}

/*
 * Function: Get index of the node
 * Parameters:
 *		[in] graph
 *		[in] node
 * Return: The index of the node. or -1 if not found.
 */
int IndexOfNode(const MyGraph *graph, char *node)
{
	void *ptr;

	if (graph == NULL)
		return -1;

	ptr = MU_HashTableSimpleBiDi_search(graph->bidiNodes, node);

	if (ptr == NULL)
		return -1;

	return (int)ptr;
}

/*
* Function: Get the distance from the source node to the destination node
* Parameters:
*		[in] graph
*       [in] index of source node
*       [in] index of destination node
* Return: distance between the two nodes, or -1 for N/A
*/
long getDistanceBetweenNodes(const MyGraph *graph, int idsrc, int iddest)
{
	struct MU_HashTable *row = MU_HashTable_search(graph->distances, (void *)idsrc);
	long distance;

	if (row != NULL)
	{
		distance = (long)MU_HashTable_search(row, (void *)iddest);

		if (distance == 0) // 0 is NULL
			return -1;

		return distance;
	}

	return -1;
}

/*
 * Function: Dijkstra algorithm to find the shortest path
 * Parameters:
 *		[in] graph
 *      [in] index of source node
 *      [in] index of destination node
 * Return: None
 */
void Dijkstra(const MyGraph *graph, int idsrc, int iddest)
{
	long distance[MY_MAX_NODE_NUMBER + 1] = { 0 };  // The distance between source node and the [i]th node
	long tmpdistance;
	int prev[MY_MAX_NODE_NUMBER + 1] = { 0 };       // The shortest chain list, -1 for root node
	int finished[MY_MAX_NODE_NUMBER + 1] = { 0 };           // true if the [i]th node's shortest distance is calculated, otherwise false

	if (graph == NULL)
		return;

	// Initialize distance to the source node
	for (int i = 1; i <= graph->nodeCount; i++) {
		distance[i] = getDistanceBetweenNodes(graph, idsrc, i);
		if (distance[i] <= 0) distance[i] = INFINITY_VALUE;

		finished[i] = 0;
		
		if (distance[i] == INFINITY_VALUE)
			prev[i] = -1;
		else
			prev[i] = idsrc;
	}

	distance[idsrc] = 0;						// The distance from the source node to itself is 0
	finished[idsrc] = 1;						// Mark it as finished


	for (int i = 1 + 1; i <= graph->nodeCount; i++) // To calculate the shortest path of the N-1 nodes left
	{
		long mindistance = INFINITY_VALUE;
		int u = idsrc;
		for (int j = 1; j <= graph->nodeCount; j++)
		{
			if ((!finished[j]) && distance[j] < mindistance) 
			{
				u = j;							// Index of the mindistance node
				mindistance = distance[j];
			}
		}
		finished[u] = 1;

		if (u == iddest) 
		{	// Print the shortest path
			printf("\n\nShortest distance: %10ld\n", mindistance);
			printf("Shortest chain (in reverse order): ", mindistance);

			do 
			{
				printf("%s", (char *)MU_HashTableSimpleBiDi_searchByValue(graph->bidiNodes, u));
				if (u != idsrc) 
				{
					printf(" <- ");
					updateGraphvizShortestPath(graph, prev[u], u);
				}

				u = prev[u];
			} while (u >= 0);

			return;
		}

		// Update the shortest distance through [u] node
		for (int j = 1; j <= graph->nodeCount; j++)
		{
			tmpdistance = getDistanceBetweenNodes(graph, u, j);
			if (tmpdistance <= 0) tmpdistance = INFINITY_VALUE;
			if ((!finished[j]) && tmpdistance < INFINITY_VALUE)
			{
				if (distance[u] + tmpdistance < distance[j])
				{
					distance[j] = distance[u] + tmpdistance;
					prev[j] = u;
				}
			}
		}
	}

	printf("Shortest path not available !\n");
}

void updateGraphvizShortestPath(MyGraph *graph, int idsource, int idtarget)
{
	char str[MY_TEMP_STRING_MAX_LEN];
	MyGraphviz *gv;

	snprintf(str, MY_TEMP_STRING_MAX_LEN - 1, "%d %d", idsource, idtarget);

	gv = MU_HashTable_search(graph->graphviz, str);
	
	if (gv == NULL)
	{
		snprintf(str, MY_TEMP_STRING_MAX_LEN - 1, "%d %d", idtarget, idsource);

		gv = MU_HashTable_search(graph->graphviz, str);

		if (gv == NULL)
			return;
	}

	gv->isShortestPath = 1;
}

static FILE *g_dotfile = NULL;
static int   g_subgraphstage = 1;

static void graphviz_hashtable_callback(void* key, void* value)
{
	MyGraphviz *gv = value;

	if (g_subgraphstage == 1) // undirected path
	{
		if (gv->isBidirectional)
			fprintf(g_dotfile, "\t\t%s -> %s [label=%d%s];\n", gv->sourceName, gv->targetName, gv->distance, (gv->isShortestPath)?", color=blue":"");
	}
	else if (g_subgraphstage == 2) // directed path
	{
		if (! (gv->isBidirectional))
			fprintf(g_dotfile, "\t\t%s -> %s [label=%d%s];\n", gv->sourceName, gv->targetName, gv->distance, (gv->isShortestPath) ? ", color=blue" : "");
	}
}

void makeGraphvizDOT(MyGraph *graph)
{
	g_dotfile = fopen("output.dot", "wt");

	if (g_dotfile == NULL)
	{
		printf("Failed to create output.dot\n");
		return;
	}

	fprintf(g_dotfile, "digraph {\n\t");

	// output node list
	for (int i = 1; i <= graph->nodeCount; i++)
	{
		char *pnodename = MU_HashTableSimpleBiDi_searchByValue(graph->bidiNodes, i);
		if (pnodename == NULL)
			continue;

		fprintf(g_dotfile, "%s", pnodename);
		if (i != graph->nodeCount)
			fprintf(g_dotfile, "; ");
	}

	fprintf(g_dotfile, "\n\n");

	g_subgraphstage = 1; // undirected path
	fprintf(g_dotfile, "\tsubgraph Rel1 {\n\t\tedge[dir = none, color = black]\n");
	MU_HashTable_traverse(graph->graphviz, graphviz_hashtable_callback);
	fprintf(g_dotfile, "\t}\n\n");

	g_subgraphstage = 2; // directed path
	fprintf(g_dotfile, "\tsubgraph Rel2 {\n\t\tedge [color=black]\n");
	MU_HashTable_traverse(graph->graphviz, graphviz_hashtable_callback);
	fprintf(g_dotfile, "\t}\n");

	fprintf(g_dotfile, "}\n");

	fclose(g_dotfile);

	printf("\n\n'output.dot' created, please open it in Graphviz 2.38(gvedit.exe) or above.\n");
}


/**
* Function: Trim the prefix and suffix space of the input string
* Parameter: [in] string
* Return: The trimmed string, or NULL if the input string is NULL
*/
static char *trimspace(char *str)
{
	char *ptr;

	if (str == NULL)
		return NULL;

	while (*str)
	{
		if (*str != ' ')
			break;

		str++;
	}

	ptr = str + strlen(str) - 1;

	while (ptr >= str)
	{
		if (*ptr != ' ')
		{
			break;
		}

		*(ptr--) = 0;
	}

	return str;
}

/**
* Function: this is a safe replacement for gets() function
* Parameter: 
*        [in] string buffer
*        [in] buffer length
* Return: The input string, or NULL if error occurs
*/
char *my_safe_gets(char *buf, size_t buflen)
{
	if (buflen <= 1)
		return NULL;

	memset(buf, 0, buflen);

	if (fgets(buf, buflen - 3, stdin) != NULL)
	{
		size_t len = strlen(buf);
		if (len > 0 && buf[len - 1] == '\n')
			buf[len - 1] = '\0';

		return buf;
	}

	return NULL;
}
