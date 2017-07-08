<!-- TOC -->

- [Strongest weighted chain of people connecting two people in a social network](#Strongest weighted chain of people connecting two people in a social network)
- [Contributions](#Contributions)
- [How to use me ?](#how-to-use-me-)
    - [In **Windows**](#in-windows)
        - [Example](#Example)
    - [In **Linux(Centos & ubuntu)**](#in-linuxcentos--ubuntu)
    - [Memory Summary](#Memory Summary)
- [Program Configuration](#Program Configuration)
    - [About input.relation](#about-inputrelation)
        - [Format](#format)
        - [Test](#test)
        - [About input.txt](#about-inputtxt)

<!-- /TOC -->

##Strongest weighted chain of people connecting two people in a social network

- [x] Graph
- [x] hashtable
- [x] Priority Queue
- [x] Dijkstra_algorithm

## Contributions

|Adjacency List | Hashtable | Priority Queue | Dijkstra_algorithm | graphviz | Readme | Data Testing |
-----------|----------|----------|--------------------|----------|---------|----------------|
|HuiMIng   | HuiMing | HuiMing Joe Wong | HuiMing Joe Wong| HuiMing | HuiMing Joe Wong | Ding Hao |


## How to use me ?

### In **Windows**

> use `cmd` to clone this project to local.

```shell
git clone https://github.com/bamboovir/TeamProject_New

```

> open with Visual Studio 2013 / 2015 (use ConsoleApplication3.sln）, enter F5 to compile and run.)
#### Example
![show][1]

[1]:https://github.com/bamboovir/TeamProject/blob/master/teamproject/1.gif

### In **Linux(Centos & ubuntu)**  

> use `shell` to clone this project to local.

```shell
git clone https://github.com/bamboovir/TeamProject
cd graph
make

```

> **如果想像使用任何linux系统原生命令一样使用它，请把编译出的二进制文件放入`PATH`环境变量所在文件夹中** 就像`ls`一样

```shell
echo $PATH
after finding the path use:
cp teamproject /usr/local/bin/teamproject
```

> To run the program

```shell
If running in manual entry mode use: ./teamproject
If running with input.txt auto entry mode use: ./teamproject < input.txt

If having trouble running the program, make sure to use:
chmod +x teamproject
```

> To Install Graphviz.

```shell
centos(red hat) : yum install graphviz
ubuntu: apt-get install graphviz

In order to create the png, use:
dot -Tpng output.dot -o output.png

```


### Memory Summary

![show1][2]

[2]:https://github.com/bamboovir/TeamProject/blob/master/teamproject/Valgrind.jpg

---

## Some thing you should know about this Project

### About input.relation 

#### Format

> Format of the relation data file: Each line is a relation. Each relation's format(case-insensitive) is: [relation string], [relation distance number], [whether relation is bidirectional(optional)]

> Note: distance should be greater than 0

> Here is some examples of the relation data file: 

> ( The relationship between the document you can also define by yourself) 

```c
				parent, 1, true        (means A is B's parent, father or mother)
				friend, 2, true
				classmate, 3, true
				neighbor, 4, true
                familiar, 5, true
				schoolmate, 6, true
				know, 9                 (means A knows B, but B doesn't know A)
				know, 9, false          (means A knows B, but B doesn't know A)
				knoweachother, 9, true  (means A knows B, and B knows A too)
```

#### Test
		
>       	    In test i use those data
>               note：when true/false is used to assign directed/undirected graph data，
>               the variable relation_name parses the relation file，and distance is calculated
>               based on assigned values given in the file.

``` c
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
 
 > input.txt is a document holding test graph edges（So we don't have to manually key in each edge)
 > Format of an edge is as follows:

 ```
			Format: [nodeA] | [nodeB] | [relation]
			Hitting "Enter" on a blank line (no information keyed) will end data entry.
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
	input：Elbert | Graham | know (NOTE：This is a directed edge，Elbert knows Graham，but Graham does not know Elbert）
	input：     （input '\n' ends data entry）
```

```	
	Print the graph data（Warning, some Linux systems may not be able to show infinite symbol"）:
				    ∞,      7,      8,     ∞,     ∞,     ∞,     ∞,     ∞,
				
				     7,     ∞,     ∞,      2,     ∞,     ∞,     ∞,     ∞,
				
				     8,     ∞,     ∞,      6,      4,     ∞,     ∞,     ∞,
				
				    ∞,      2,      6,     ∞,      9,      8,      3,     ∞,
				
				    ∞,     ∞,      4,      9,     ∞,     ∞,     ∞,      1,
				
				    ∞,     ∞,     ∞,      8,     ∞,     ∞,     ∞,     ∞,
				
				    ∞,     ∞,     ∞,      3,     ∞,     ∞,     ∞,      1,
				
				    ∞,     ∞,     ∞,     ∞,      1,     ∞,      1,     ∞,
				
	
	To calculate the shortest path, please input the source node: 
	Input：Alden						 （输入开始节点）
	
	Please input the destination node: 
	Input：Elbert            （输入结束节点）

  Shortest distance:         13
	Shortest chain (in reverse order): Elbert <- Herman <- Forrest <- Cary <- Alden
	
	'output.dot' created, please open it in Graphviz 2.38(gvedit.exe) or above.
```
	
>  **TEST：If we change Elbert and Graham's relation to `knoweachother`, the following results：`Elbert <- Graham <- Cary <- Alden`**

## Main Algorithm 

The main algorithm is a modification of the classic Dijkstra Algorithm.
Every relation is parsed against `input.relation`。Within (`struct MyRelation`) the values are `relation_name`, `distance`, `is_bidirectional`
These relations are stored in a `hashtable(struct MU_HashTable)`，as `graph.relations`。
When inputing graph data, the variable `relation_name` corresponds to a `relation` which derives `distance` & `is_bidirectional`
In order to effectively handle sparse arrays，We use a double layer `hash table` to represent a 2d space;
In order to minimize space requirements，the double layer `hash table` only saves `node index`，Since we don't need to store `hashvalue`，`access speed` increases;
Thus the first layer of the double layer `hash table`(`graph.distances`)'s`Key` is `row index`、`Value`is the second layer's `hash table` key;
Second layer `hash table`'s`Key`is`column index`、`Value`is" the distance between the node in `column index` and the node in `row index`;
To simplify the representation of `distance`，we chose to use `long` variable type;
Since we use `row index & column index` as `Key`，we won't need to `malloc & free`;
Similarly，since `distance`is of type`long`，we won't be needing `malloc & free`;
For`hash table`since we use`NULL`in order to check if`Key->Value pair` exists，we start the`index`at 1.`[1..nodeCount]`, rather than `[0..nodeCount-1]` 	 
Since`graph data` contains only `node index`，we must save the link between `node_name` & `node_index` so we use `hash table(struct MU_HashTableSimpleBiDi)`，while saving `[Key->Value pair]`and `[Value->Key pair]`at the same time.

## Using Graphviz(Version 2.38) DOT 

   To use`DOT`template (Rel1 represents `undirected graph`, Rel2 represents `directed graph`)，	 
	 	 
```c
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
      
###    Definition of struct MyGraphviz：

>	    isShortestPath tells whether the `edge` is on the shortest path，0 - NO、1 - YES。
>		When calculating distances, we need to update isShortestPath。Since an edge requires two nodes to represent，
>		we use pairName to show an edge，format is: "idSource idTarget"
>		isBidirectional represents whether the edge is directed，0 - NO、1 - YES	 
	 
