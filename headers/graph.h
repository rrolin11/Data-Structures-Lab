#ifndef GRAPH_H
#define GRAPH_H

#include <functional> // Para poder pasar la función por parámetro en el método.

#include "tuple.h"
#include "heap.h"
#include "iterator.h"
#include "list.h"
#include "queue.h"
#include "stack.h"
#include "pqueue.h"

/// @brief Implementa un grafo híbrido, según el parámetro del constructor
/// que indica que es denso, si es verdadero, usará matrices, de lo contrario
/// usará listas de adyacencias.
class Graph
{
    private:
        class Edge 
        {
            public:
                int from;
                int to;
                int weight;
                Edge() {};
                Edge(int from, int to) : from(from), to(to), weight(1) {}
                Edge(int from, int to, int weight) : from(from), to(to), weight(weight) {}
                ~Edge(){}
                bool operator==(const Edge & o)
                {
                    return (this->from == o.from && this->to == o.to && this->weight == o.weight);
                }
        };
        int _totalNodes;
        int _totalEdges;
        bool _isDense;
        bool _isDirected;
        bool _isWeighted;
        /// @brief Cada índice del arreglo representa un nodo y dicho nodo tiene una lista de aristas asociadas.
        List<Edge> * _edgesArray;
        /// @brief El arreglo indica el grado de incidencia de cada nodo.
        int * _nodesInDegreeArray;
        int * _nodesOutDegreeArray;
        /// @brief Cada coordenada from, to, representa una arista con su peso. Si no hay arista, el valor es nulo.
        int *** _edgesMatrix;

        void initListGraph()
        {
            _edgesArray = new List<Edge>[_totalNodes+1];
        }

        void initMatrixGraph()
        {
            _edgesMatrix = new int**[_totalNodes+1];
            for(int i = 1; i <= _totalNodes; i++)
            {
                _edgesMatrix[i] = new int*[_totalNodes+1];
                // Si no es dirigido uso la mitad superior de la diagonal principal de la matriz.
                int j = _isDirected ? 1 : i;
                for (j = 1; j <= _totalNodes; j++)
                {
                    _edgesMatrix[i][j] = NULL;
                }                
            }
        }

        /// @brief Retorna un heap con los nodos de grado cero.
        /// Si no los hay, retorna un heap vacío.
        Heap<int> * getCeroInDegreeNodes() const
        {
            Heap<int> * nodesHeap = new Heap<int>(_totalNodes, MIN);
            for (int i = 1; i <= _totalNodes; i++)
            {
                if(_nodesInDegreeArray[i] == 0)
                {
                    nodesHeap->add(i);
                }
            }
            return nodesHeap;
        }

        #pragma region métodos grafo de matríz

        /// @brief Agrega una arista al grafo y actualiza los grados de incidencia de sus nodos. 
        /// Si el grafo es no dirigido, la arista se agrega en ambos nodos.
        /// Los nodos son numerados desde 1 hasta la cantidad máxima de nodos del grafo. 
        /// Si se intenta insertar un nodo con un número menor que 1, el método no tiene efecto.
        /// @param nodeFrom Nodo del cual sale la arista.
        /// @param nodeTo Nodo al cual se dirige la arista.
        /// @param weight Peso de la arista.
        void mAddEdge(int nodeFrom, int nodeTo, int weight = 1)
        {
            if(nodeFrom > 0 && nodeTo > 0)
            {
                int * w = new int;
                *w = _isWeighted ? weight : 1;
                _edgesMatrix[nodeFrom][nodeTo] = w;
                _nodesInDegreeArray[nodeTo]++;
                _nodesOutDegreeArray[nodeFrom]++;
                if(!_isDirected)
                {
                    *_edgesMatrix[nodeTo][nodeFrom] = _isWeighted ? weight : 1;
                    _nodesInDegreeArray[nodeFrom]++;
                    _nodesOutDegreeArray[nodeTo]++;                
                }
                _totalEdges++;
            }   
        }

        /// @brief Retorna el peso de la arista entre los nodos dados.
        /// Precondición: La arista existe.
        /// @return 
        int mEdgeWeight(int nodeFrom, int nodeTo)
        {
            if(!_isDirected)
            {
                // Si no es dirigido uso la mitad superior de la diagonal principal de la matriz.
                return nodeFrom >= nodeTo 
                ? *_edgesMatrix[nodeFrom][nodeTo] 
                : *_edgesMatrix[nodeTo][nodeFrom];
            }
            else
            {
                return *_edgesMatrix[nodeFrom][nodeTo];
            }
        }

        /// @brief Retrona el arreglo con los posibles nodos adyacentes al nodo indicado.
        /// Si el nodo no tiene adyacentes, retorna null.
        int ** mAdjacents(int from)
        {
            if(_nodesOutDegreeArray[from] > 0)
            {
                return _edgesMatrix[from];
            }
            return NULL;
        }     

        /// @brief Realiza el recorrido por anchura del grafo, aplicándole a cada nodo
        // la función enviada por parámetro.
        /// @param nodeFrom El nodo desde donde comienza la recorrida.
        /// @param function La función que se ejecuta sobre cada nodo.
        void mBfSearch(int nodeFrom, std::function<void(int, int)> f)
        {
            bool visitedNodes[_totalNodes+1];
            for(int i = 1; i <= _totalNodes; visitedNodes[i++] = false);            
            visitedNodes[nodeFrom] = true;
            Queue<Tuple<int, int>> nodesQueue = Queue<Tuple<int,int>>();
            nodesQueue.enqueue(Tuple<int,int>(nodeFrom, 0));
            while (!nodesQueue.isEmpty())
            {
                Tuple<int,int> tuple = nodesQueue.front();
                nodesQueue.dequeue();
                int node = tuple.first;
                int step = tuple.second;
                f(node, step);                
                int ** adjacentNodes = mAdjacents(node);
                if(adjacentNodes != NULL)
                {
                    for(int i = 1; i <= _totalNodes; i++)
                    {
                        if(adjacentNodes[i] != NULL && !visitedNodes[*adjacentNodes[i]])
                        {
                            visitedNodes[*adjacentNodes[i]] = true;
                            nodesQueue.enqueue(Tuple<int,int>(*adjacentNodes[i], step + 1));
                        }
                    }
                }
            }
        }
        
        /// @brief Retorna una lista con el órden topológico de los nodos, ordenados
        /// de menor a mayor en su numeración en lo posible. 
        /// Si el grafo contiene algún ciclo, retorna la una lista vacía.
        /// Precondición: El grafo es dirigido.
        List<int> * mTopoSort()
        {
            List<int> * resultList = new List<int>();
            if(!_isDirected)
            {
                return resultList;
            }
            int nodesInDegreeAuxArray[_totalNodes+1];
            // Se copia el arreglo para no modificar el global.
            for(int i = 1; i <= _totalNodes; i++)
            {
                nodesInDegreeAuxArray[i] = _nodesInDegreeArray[i];
            }
            Heap<int> * ceroInDegreeNodesHeap = getCeroInDegreeNodes();
            int visitedNodesCount = 0;
            while (!ceroInDegreeNodesHeap->isEmpty())
            {
                visitedNodesCount++;
                int node = ceroInDegreeNodesHeap->top();
                ceroInDegreeNodesHeap->removeTop();
                resultList->add(node);
                int ** nodeAdjacents = mAdjacents(node);
                if(nodeAdjacents != NULL)
                {                    
                    // Se "eliminan aristas" bajando el grado de todos los nodos incididos desde el nodo actual.
                    for(int i = 1; i <= _totalNodes; i++)
                    {
                        if(nodeAdjacents[i] != NULL && --nodesInDegreeAuxArray[i] == 0)
                        {
                            ceroInDegreeNodesHeap->add(i);
                        }
                    }
                }
            }
            delete ceroInDegreeNodesHeap;
            if(visitedNodesCount < _totalNodes)
            {
                resultList->clear();
                return resultList;
            }
            return resultList;
        }

        /// @brief Retorna un stack con el camino más corto desde un nodo hasta otro.
        /// Precondición: el grafo es ponderado con pesos de arista no negativos.
        Stack<int> * mDijkstra(int from, int to)
        {
            Stack<int> * path = new Stack<int>();
            bool visitedNodes[_totalNodes+1];
            int previousNode[_totalNodes+1]; // indica el nodo previo al nodo [i].
            int fromDistanceTo[_totalNodes+1]; // representa la distancia del nodo "from" hasta el nodo [i].
            for (int i = 1; i <= _totalNodes; i++)
            {
                visitedNodes[i] = false;
                previousNode[i] = -1;
                fromDistanceTo[i] = INT32_MAX;
            }
            fromDistanceTo[from] = 0;
            PQueue<int> unvisitedNodes = PQueue<int>(_totalNodes+_totalEdges);
            unvisitedNodes.enqueue(from, 0);
            while (!unvisitedNodes.isEmpty())
            {
                int node = unvisitedNodes.front();
                unvisitedNodes.dequeue();
                if(!visitedNodes[node])
                {                      
                    visitedNodes[node] = true;
                    if(node != to)
                    {
                        int ** nodeAdjacents = mAdjacents(node);
                        for (int i = 1; nodeAdjacents != NULL && i <= _totalNodes; i++)
                        {
                            if(nodeAdjacents[i] != NULL)
                            {
                                int adjacentNode = i;
                                int weight = *nodeAdjacents[i];
                                // Si la distancia desde From al adyacente del nodo actual es mayor que la distancia desde From al 
                                // al nodo actual sumado a la distancia del nodo actual a su adyacente, entonces entra.
                                if(!visitedNodes[adjacentNode] && fromDistanceTo[adjacentNode] > fromDistanceTo[node] + weight)
                                {
                                    previousNode[adjacentNode] = node;
                                    fromDistanceTo[adjacentNode] = fromDistanceTo[node] + weight;
                                    unvisitedNodes.enqueue(adjacentNode, fromDistanceTo[node] + weight);
                                }
                            }
                        }
                    }
                }
            }
            // Si se visitó el nodo To hay un camino.
            if(visitedNodes[to])
            {
                path->push(to);
                int node = to;
                while(node != from)
                {       
                    node = previousNode[node];        
                    path->push(node);
                }
            }            
            return path;
        }

        /// @brief Retorna el coste asociado al árbol de cubrimiento mínimo del grafo,
        /// o retorna null si no se encontró uno.
        int * mprim(int from)
        {
            bool visitedNodes[_totalNodes+1];
            int nodeCost[_totalNodes+1];
            for (int i = 1; i <= _totalNodes; i++)
            {
                visitedNodes[i] = false;
                nodeCost[i] = INT32_MAX;
            }
            nodeCost[from] = 0;
            PQueue<int> nodesQueue = PQueue<int>(_totalNodes + _totalEdges);
            nodesQueue.enqueue(from, 0);
            int visitedNodesCount = 0;

            while (!nodesQueue.isEmpty())
            {
                int node = nodesQueue.front();
                nodesQueue.dequeue();
                if(!visitedNodes[node])
                {
                    visitedNodesCount++;
                    visitedNodes[node] = true;
                    int ** adjacentsNodes = mAdjacents(node);
                    if(adjacentsNodes != NULL)
                    {
                        for(int node = 1; node <= _totalNodes && adjacentsNodes[node] != NULL; node++)
                        {
                            int cost = *adjacentsNodes[node];
                            if(!visitedNodes[node] && nodeCost[node] > cost)
                            {
                                nodeCost[node] = cost;
                                nodesQueue.enqueue(node, cost);
                            }
                        }
                    }
                }                
            }
            if(visitedNodesCount == _totalNodes)
            {
                int sum = 0;
                for (int i = 1; i <= _totalNodes; i++)
                {
                    sum+= nodeCost[i];
                }
                int * result = new int;
                *result = sum;
                return result;     
            }
            else
            {
                return NULL;
            }
        }
        
        #pragma endregion métodos grafo de matríz

        #pragma region métodos grafo de listas de aydacencia

        /// @brief Agrega una arista al grafo y actualiza los grados de incidencia de sus nodos. 
        /// Si el grafo es no dirigido, la arista se agrega en ambos nodos.
        /// Los nodos son numerados desde 1 hasta la cantidad máxima de nodos del grafo. 
        /// Si se intenta insertar un nodo con un número menor que 1, el método no tiene efecto.
        /// @param nodeFrom Nodo del cual sale la arista.
        /// @param nodeTo Nodo al cual se dirige la arista.
        /// @param weight Peso de la arista.
        void lAddEdge(int nodeFrom, int nodeTo, int weight = 1)
        {
            if(nodeFrom > 0 && nodeTo > 0)
            {
                Edge edge = Edge(nodeFrom, nodeTo, _isWeighted ? weight : 1);
                _edgesArray[nodeFrom].add(edge);
                _nodesInDegreeArray[nodeTo]++;
                _nodesOutDegreeArray[nodeFrom]++;
                if(!_isDirected)
                {
                    edge.from = nodeTo;
                    edge.to = nodeFrom;
                    _edgesArray[nodeTo].add(edge);
                    _nodesInDegreeArray[nodeFrom]++;  
                    _nodesOutDegreeArray[nodeTo]++;                              
                }
                _totalEdges++;
            }            
        }

        /// @brief Retorna el peso de la arista entre los nodos dados.
        /// Precondición: La arista existe.
        /// @return 
        int lEdgeWeight(int nodeFrom, int nodeTo)
        {
            int weight = 0;
            Iterator<Edge> * iter = lAdjacents(nodeFrom);
            while (iter->hasNext())
            {
                Edge e = iter->next();
                if(e.to == nodeTo)
                {
                    weight = e.weight;
                }
            }
            return weight;
        }

        /// @brief Realiza el recorrido por anchura del grafo, aplicándole a cada nodo
        // la función enviada por parámetro.
        /// @param nodeFrom El nodo desde donde comienza la recorrida.
        /// @param function La función que se ejecuta sobre cada nodo.
        void lBfSearch(int nodeFrom, std::function<void(int, int)> f)
        {
            bool visitedNodes[_totalNodes+1];
            for(int i = 1; i <= _totalNodes; visitedNodes[i++] = false);            
            Queue<Tuple<int, int>> nodesQueue = Queue<Tuple<int,int>>();
            nodesQueue.enqueue(Tuple<int,int>(nodeFrom, 0));
            while (!nodesQueue.isEmpty())
            {                
                Tuple<int,int> tuple = nodesQueue.front();
                nodesQueue.dequeue();
                int node = tuple.first;
                int step = tuple.second;
                visitedNodes[node] = true;
                f(node, step);                
                Iterator<Edge> * iter = lAdjacents(node);
                while (iter->hasNext())
                {
                    Edge edge = iter->next();
                    if(!visitedNodes[edge.to])
                    {
                        nodesQueue.enqueue(Tuple<int,int>(edge.to, step + 1));
                    }
                }
                delete iter;
            }
        }

        void lDfSearch(int nodeFrom, std::function<void(int)> f,  bool * visitedNodes)
        {
            visitedNodes[nodeFrom] = true;
            f(nodeFrom);
            Iterator<Edge> * adjacents = lAdjacents(nodeFrom);
            while (adjacents->hasNext())
            {
                int adjacentNode = adjacents->next().to;
                if(!visitedNodes[adjacentNode])
                {
                    lDfSearch(adjacentNode, f, visitedNodes);
                }
            }
        }

        bool lHasPath(int nodeFrom, int nodeTo, int * visitedNodes)
        {
            bool hasPath = nodeFrom == nodeTo;
            if(!hasPath)
            {
                visitedNodes[nodeFrom] = true;
                Iterator<Edge> * adjacents = lAdjacents(nodeFrom);
                while (adjacents->hasNext() && !hasPath)
                {
                    int adjacentNode = adjacents->next().to;
                    if(!visitedNodes[adjacentNode])
                    {
                        hasPath = lHasPath(adjacentNode, nodeTo, visitedNodes);
                    }
                }
            }
            return hasPath;            
        }

        /// @brief Retorna una lista con el órden topológico de los nodos, ordenados
        /// de menor a mayor en su numeración en lo posible. 
        /// Si el grafo contiene algún ciclo, retorna la una lista vacía.
        /// Precondición: El grafo es dirigido.
        List<int> * lTopoSort()
        {
            List<int> * resultList = new List<int>();
            if(!_isDirected)
            {
                return resultList;
            }
            int nodesInDegreeAuxArray[_totalNodes+1];
            // Se copia el arreglo para no modificar el global.
            for(int i = 1; i <= _totalNodes; i++)
            {
                nodesInDegreeAuxArray[i] = _nodesInDegreeArray[i];
            }
            Heap<int> * ceroInDegreeNodesHeap = getCeroInDegreeNodes();
            int visitedNodesCount = 0;
            while (!ceroInDegreeNodesHeap->isEmpty())
            {
                visitedNodesCount++;
                int node = ceroInDegreeNodesHeap->top();
                ceroInDegreeNodesHeap->removeTop();
                resultList->add(node);
                Iterator<Edge> * iter = lAdjacents(node);
                // Se "eliminan aristas" bajando el grado de todos los nodos incididos desde el nodo actual.
                while (iter->hasNext())
                {
                    Edge e = iter->next();
                    if(--nodesInDegreeAuxArray[e.to] == 0)
                    {
                        ceroInDegreeNodesHeap->add(e.to);
                    }
                }
                delete iter;
            }
            delete ceroInDegreeNodesHeap;
            if(visitedNodesCount < _totalNodes)
            {
                resultList->clear();
                return resultList;
            }
            return resultList;
        }

        /// @brief Retorna un interador a la lista de aristas adyacentes asociadas al nodo.
        Iterator<Edge> * lAdjacents(int from)
        {
            return _edgesArray[from].getIterator();
        }

        /// @brief Retorna un stack con el camino más corto desde un nodo hasta otro, comenzando desde el origen.
        /// Precondición: el grafo es ponderado con pesos de arista no negativos.
        Stack<int> * lDijkstra(int from, int to)
        {
            Stack<int> * path = new Stack<int>();
            bool visitedNodes[_totalNodes+1];
            int previousNode[_totalNodes+1]; // indica el nodo previo al nodo [i].
            int fromCostTo[_totalNodes+1]; // representa el costo desde el nodo "from" hasta el nodo [i].
            for (int i = 1; i <= _totalNodes; i++)
            {
                visitedNodes[i] = false;
                previousNode[i] = 0;
                fromCostTo[i] = INT32_MAX;
            }
            fromCostTo[from] = 0;
            PQueue<int> unvisitedNodes = PQueue<int>(_totalNodes+_totalEdges); // Cola de prioridad ordenada por menor costo.
            unvisitedNodes.enqueue(from, 0);
            while (!unvisitedNodes.isEmpty())
            {
                int node = unvisitedNodes.front();
                unvisitedNodes.dequeue();
                if(!visitedNodes[node])
                {                      
                    visitedNodes[node] = true;
                    if(node != to)
                    {
                        Iterator<Edge> * iter = lAdjacents(node);
                        while(iter->hasNext())
                        {
                            Edge e = iter->next();
                            int adjacentNode = e.to;
                            if(!visitedNodes[adjacentNode] && fromCostTo[adjacentNode] > fromCostTo[node] + e.weight)
                            {
                                previousNode[adjacentNode] = node;
                                fromCostTo[adjacentNode] = fromCostTo[node] + e.weight;
                                unvisitedNodes.enqueue(adjacentNode, fromCostTo[node] + e.weight);
                            }
                        }
                        delete iter;
                    }
                }
            }
            // Si se visitó el nodo To hay un camino.
            if(visitedNodes[to])
            {
                path->push(to);
                int node = to;
                while(node != from)
                {       
                    node = previousNode[node];        
                    path->push(node);
                }
            }
            return path;
        }

        /// @brief Retorna el coste asociado al árbol de cubrimiento mínimo del grafo,
        /// o retorna null si no se encontró uno.
        int * lprim(int from)
        {
            bool visitedNodes[_totalNodes+1];
            int nodeCost[_totalNodes+1];
            for (int i = 1; i <= _totalNodes; i++)
            {
                visitedNodes[i] = false;
                nodeCost[i] = INT32_MAX;
            }
            nodeCost[from] = 0;
            PQueue<int> nodesQueue = PQueue<int>(_totalNodes + _totalEdges);
            nodesQueue.enqueue(from, 0);
            int visitedNodesCount = 0;

            while (!nodesQueue.isEmpty())
            {
                int node = nodesQueue.front();
                nodesQueue.dequeue();
                if(!visitedNodes[node])
                {
                    visitedNodesCount++;
                    visitedNodes[node] = true;
                    Iterator<Edge> * iter = lAdjacents(node);

                    while (iter->hasNext())
                    {
                        Edge e = iter->next();
                        int adjacentNode = e.to;
                        if(!visitedNodes[adjacentNode] && nodeCost[adjacentNode] > e.weight)
                        {
                            nodeCost[adjacentNode] = e.weight;
                            nodesQueue.enqueue(adjacentNode, e.weight);
                        }
                    }
                    delete iter;
                }                
            }
            if(visitedNodesCount == _totalNodes)
            {
                int sum = 0;
                for (int i = 1; i <= _totalNodes; i++)
                {
                    sum+= nodeCost[i];
                }
                int * result = new int;
                *result = sum;
                return result;     
            }
            else
            {
                return NULL;
            }
        }

        #pragma endregion region métodos grafo de listas de aydacencia      

    public:
        /// @brief Crea un grafo según los parámetros indicados.
        /// @param totalNodes Cantidad de nodos que contendrá el grafo.
        /// @param isDirected Indica si el grafo será dirigido.
        /// @param isWeighted Indica si será ponderado.
        /// @param isDense Indica si el grafo será denso en cantidad de aristas.
        explicit Graph(int totalNodes, bool isDirected, bool isWeighted, bool isDense)
        {
            _totalNodes = totalNodes;
            _totalEdges = 0;
            _isDirected = isDirected;
            _isWeighted = isWeighted;
            _isDense = isDense;
            // No se utilizan los índices 0.
            _nodesInDegreeArray = new int[_totalNodes+1];
            _nodesOutDegreeArray = new int[_totalNodes+1];
            _nodesInDegreeArray[0] = -1;
            for (int i = 1; i < _totalNodes+1; i++)
            {
                _nodesInDegreeArray[i] = _nodesOutDegreeArray[i] = 0;
            }
            if(_isDense)
            {
                initMatrixGraph();
            }
            else
            {
                initListGraph();
            }
        }

        ~Graph()
        {
            delete[] _nodesInDegreeArray;
            delete[] _nodesOutDegreeArray;
            if(_isDense)
            {
                for(int i = 1; i <= _totalNodes; i++)
                {
                    for (int j = 1; j <= _totalNodes; j++)
                    {
                        delete _edgesMatrix[i][j];
                    } 
                    delete[] _edgesMatrix[i];
                }
                delete[] _edgesMatrix;
            }
            else
            {
                delete[] _edgesArray;
            }
        }
            
        /// @brief Agrega una arista al grafo y actualiza los grados de incidencia de sus nodos. 
        /// Si el grafo es no dirigido, la arista se agrega en ambos nodos.
        /// Los nodos son numerados desde 1 hasta la cantidad máxima de nodos del grafo. 
        /// Si se intenta insertar un nodo con un número menor que 1, el método no tiene efecto.
        /// @param nodeFrom Nodo del cual sale la arista.
        /// @param nodeTo Nodo al cual se dirige la arista.
        /// @param weight Peso de la arista.
        void addEdge(int nodeFrom, int nodeTo, int weight = 1)
        {
            if(_isDense)
            {
                mAddEdge(nodeFrom, nodeTo, weight);
            }
            else
            {
                lAddEdge(nodeFrom, nodeTo, weight);
            }           
        }

        /// @brief Retorna el peso de la arista entre los nodos dados.
        /// Precondición: La arista existe.
        /// @return 
        int edgeWeight(int nodeFrom, int nodeTo)
        {
            if(_isDense)
            {
                return mEdgeWeight(nodeFrom, nodeTo);
            }
            else
            {
                return lEdgeWeight(nodeFrom, nodeTo);
            }
        }

        /// @brief Realiza el recorrido por anchura del grafo, aplicándole a cada nodo
        // la función enviada por parámetro.
        /// @param nodeFrom El nodo desde donde comienza la recorrida.
        /// @param function La función que se ejecuta sobre cada nodo.
        void bfSearch(int nodeFrom, std::function<void(int, int)> f)
        {
            if(_isDense)
            {
                mBfSearch(nodeFrom, f);
            }
            else
            {
                lBfSearch(nodeFrom, f);
            }
        }

        void dfSearch(int nodeFrom, std::function<void(int)> f)
        {
            bool visitedNodes[_totalNodes+1];
            for (int i = 1; i <= _totalNodes; visitedNodes[i++] = false);
            if(_isDense)
            {
                lDfSearch(nodeFrom, f, visitedNodes);
            }
            else
            {
                lDfSearch(nodeFrom, f, visitedNodes);
            }
        }

        /// @brief Retorna una lista con el órden topológico de los nodos, ordenados
        /// de menor a mayor en su numeración en lo posible. 
        /// Si el grafo contiene algún ciclo, retorna la una lista vacía.
        /// Precondición: El grafo es dirigido.
        List<int> * topoSort()
        {
            if(_isDense)
            {
                return mTopoSort();
            }
            else
            {
                return lTopoSort();
            }
        }        

        /// @brief Retorna un stack con el camino más corto desde un nodo hasta otro.
        /// Precondición: el grafo es ponderado con pesos de arista no negativos.
        Stack<int> * dijkstra(int from, int to)
        {
            if(_isDense)
            {
                return mDijkstra(from, to);
            }
            else
            {
                return lDijkstra(from, to);
            }
        }

        int * prim(int from)
        {
            if(_isDense)
            {
                return mprim(from);
            }
            else
            {
                return lprim(from);
            }
        }
        
};

#endif