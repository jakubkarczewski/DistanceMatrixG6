# G6 DistanceMatrix for graphs in G6 format

C extension for Python3.6 that enables:
- loading graphs from G6 (text format)
- adding/deleting edges and vertices
- graph comparison

Only graphs of order 10 and less are supported.

Graphs are internally represented as distance matrices computed using Dijkstra algo.

 Run ```python3.6 setupy.py build``` to build the ```simple_graphs``` module.

###API:
- Import: ```from simple_graphs import DistanceMatrix``` 
- Load a graph (argument is an example): ```graph = Distancematrix('G?r@`_')```
- Get the order of graph: ```graph.order()```
- Add a vertex: ```graph.addVertex()```
- Delete a vertex: ```graph.deleteVertex(index)```
- Add an edge:```graph.addEdge(vertex_a, vertex_b)```
- Add an edge:```graph.deleteEdge(vertex_a, vertex_b)```
- Check if vertices are connected: ```graph.isEdge(vertex_a, vertex_b)```

###Exceptions:
 - ```G6Error``` if G6 input is corrupted
 - ```NoVerticesError``` when deleting last vertice of graph
 - ```TooManyVerticesError``` when trying to break the order limit
 

 ---
 This project was made as an GUT assignment.
 
