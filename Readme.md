<!-- TOC -->

- [任务分配](#任务分配)
- [How to use me ?](#how-to-use-me-)
    - [In **Windows**](#in-windows)
        - [就像这样](#就像这样)
    - [In **Linux**](#in-linux)
- [Some thing you should know about this Project](#some-thing-you-should-know-about-this-project)
    - [About input.relation](#about-inputrelation)
        - [Format](#format)
        - [Test](#test)
        - [About input.txt](#about-inputtxt)

<!-- /TOC -->
## 任务分配

|图数据结构 |hashtable | 优先队列 | Dijkstra_algorithm | graphviz | 说明文档 | 实验数据 |
-----------|----------|----------|--------------------|----------|---------|----------------|
|HuiMIng   | HuiMing | HuiMing | HuiMing | HuiMing | HuiMing  | Ding Hao |


## How to use me ?

### In **Windows**

> use `cmd` to clone this project to local.

```shell
git clone https://github.com/bamboovir/TeamProject_New

```

> open with Visual Studio 2013 / 2015 (use ConsoleApplication3.sln）, enter F5 to compile and run.)
#### 就像这样
![show][1]
[1]:https://github.com/bamboovir/TeamProject/blob/master/teamproject/1.gif
### In **Linux**  

> use `shell` to clone this project to local.

```shell
git clone https://github.com/bamboovir/TeamProject_New
cd graph
make

```
---

## Some thing you should know about this Project

### About input.relation 

#### Format

> Format of the relation data file: Each line is a relation. Each relation's format(case-insensitive) is: [relation string], [relation distance number], [whether relation is bidirectional(optional)]

> Note: distance should be greater than 0

> Here is some examples of the relation data file: 

> ( The relationship between the document you can also define by yourself) 

```c
				parent, 20, true        (means A is B's parent, father or mother)
				friend, 18, true
				classmate, 14, true
				neighbor, 10, true
				schoolmate, 5, true
				know, 3                 (means A knows B, but B doesn't know A)
				know, 3, false          (means A knows B, but B doesn't know A)
				knoweachother, 3, true  (means A knows B, and B knows A too)
```

#### Test
		
>       	    In test i use those data
>               note：true/false use to assign directed/undirected graph data时，会根据relation_name查询relation，并获取对应的distance

``` 
    			Alden | Blaine | classmate
				Alden | Cary | friend
				Blaine | Forrest | met
				Cary | Forrest | neighbor
				Cary | Graham | familiar
				David | Forrest | friend
				Forrest | Graham | parent
				Forrest | Herman | schoolmate
				Elbert | Herman | knoweachother
				Elbert | Graham | know
```

#### About input.txt
 
 > input.txt是一个测试用的输入数据（为了避免每次测试重复输入它们），其格式为：

 ```
			Format: [nodeA] | [nodeB] | [relation]
			A blank line will end the input
```

### Run
  
```
	Creating the graph, please input node-pair line by line, 
	format: Name1 | Name2 | Relation
	input：Alden | Blaine | classmate
    input：Alden | Cary | friend
	input：Blaine | Forrest | met
	input：Cary | Forrest | neighbor
	input：Cary | Graham | familiar
	input：David | Forrest | friend
	input：Forrest | Graham | parent
	input：Forrest | Herman | schoolmate
	input：Elbert | Herman | knoweachother
	input：Elbert | Graham | know                （注意：这是单向关系，表示Elbert认识Graham，而Graham不认识Elbert）
	input：                  （input '\n'，表示graph输入结束）
```

```	
	Print the graph data（示例，如果英文版Linux无法显示"无穷大符号"，可以改为输出"INF或者/"）:
				    ∞,      7,      8,     ∞,     ∞,     ∞,     ∞,     ∞,
				
				     7,     ∞,     ∞,      2,     ∞,     ∞,     ∞,     ∞,
				
				     8,     ∞,     ∞,      6,      4,     ∞,     ∞,     ∞,
				
				    ∞,      2,      6,     ∞,      9,      8,      3,     ∞,
				
				    ∞,     ∞,      4,      9,     ∞,     ∞,     ∞,      1,
				
				    ∞,     ∞,     ∞,      8,     ∞,     ∞,     ∞,     ∞,
				
				    ∞,     ∞,     ∞,      3,     ∞,     ∞,     ∞,      1,
				
				    ∞,     ∞,     ∞,     ∞,      1,     ∞,      1,     ∞,
				
	
	To calculate the shortest path, please input the source node: 
	输入：Alden						 （输入开始节点）
	
	Please input the destination node: 
	输入：Elbert            （输入结束节点）

  Shortest distance:         13
	Shortest chain (in reverse order): Elbert <- Herman <- Forrest <- Blaine <- Alden
	
	'output.dot' created, please open it in Graphviz 2.38(gvedit.exe) or above.
```
	
	测试：如果将Elbert和Graham的关系改为knoweachother, 则最短路径变为：Elbert <- Graham <- Cary <- Alden

## 主程序结构说明

     这个主程序还是从二维数组的dijkstra代码修改而来的。
   
	 所有的关系，通过input.relation来定义。关系(struct MyRelation)包括relation_name, distance, is_bidirectional
	 这些关系保存在一个hashtable中(struct MU_HashTable)，即graph.relations。
	 在输入graph data时，会根据relation_name查询relation，并获取对应的distance & is_bidirectional

	 为了有效处理稀疏矩阵，利用2层hash table模拟二维数组;
	 为了降低存储空间，在这个2层hash table中仅保存node index，由于不需要计算hashvalue，所以access speed更快;
	 因此这个2层hash table的第一层(graph.distances)的Key是row index、Value是第2层hash table;
	 第2层hash table的Key是column index、Value是"该column index对应的node"与"该row index对应的node"之间的distance;
	 为了简单起见，distance使用long类型;
	 由于使用 row index & column index 作为Key，因此它不需要malloc & free;
	 同样的道理，distance由于使用long类型，它也不需要malloc & free;
	 由于hash table内部使用NULL来判断是否存在该Key->Value pair，因此index range为[1..nodeCount], 不是[0..nodeCount-1]
	 	 
	 由于在graph data中，仅保存node index，所以，需要另外单独保存node_name & node_index的对应关系
	 这里采用了双向hash table(struct MU_HashTableSimpleBiDi)，即同时保存[Key->Value pair]和[Value->Key pair]。
	 因为既需要根据node name检索node index，又需要根据node index检索node name

## 生成 Graphviz(Version 2.38) DOT 文件

   使用的DOT文件模板(其中Rel1为undirected graph, Rel2为directed graph)，
	 模板中包括了node、edge、默认颜色、定制颜色、定制edge标签等，基本满足我们的需求：	 
     ```
							digraph {
							
							    A; B; C
							
							    subgraph Rel1 {
							        edge [dir=none, color=red]
							        A -> B [label=4, color=green];
											B -> C -> A 
							    }
							
							    subgraph Rel2 {
							        edge [color=blue]
							
							        B -> C
							        C -> A
							    }
							
							}
	  ```
      
    定义struct MyGraphviz结构体：
		isShortestPath表示该edge是否位于最短路径上，0 - NO、1 - YES。
		在计算出最短路径后，需要更新isShortestPath。由于edge需要两个nodes来标识，所以使
		用pairName是作为edge的唯一标识，格式为"idSource idTarget"
		isBidirectional表示该edge是否为无向图/双向图，0 - NO、1 - YES	 
	 
