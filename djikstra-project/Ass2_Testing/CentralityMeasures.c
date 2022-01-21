// Centrality Measures ADT implementation
// COMP2521 Assignment 2

#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "Dijkstra.h"
#include "PQ.h"
#include "math.h"

double findTotalPaths(ShortestPaths path, Vertex src, Vertex dest);
double findIncluded(ShortestPaths path, Vertex src, Vertex dest, Vertex mid_node);

NodeValues closenessCentrality(Graph g) {

	NodeValues nvs;
	nvs.numNodes = GraphNumVertices(g);
	nvs.values = calloc(nvs.numNodes, sizeof(double));

	for (int src = 0; src < nvs.numNodes; src++) {

		ShortestPaths path = dijkstra(g, src);
		// set reachable as 1 since we have basically "reached" the current node
		double dist_sum = 0, reachable = 1;
		for (int vert = 0; vert < nvs.numNodes; vert++) {
			if (path.dist[vert] > 0) {
				reachable++;
				dist_sum += path.dist[vert];
			}
		}
		
		double nV = nvs.numNodes;
		if (GraphInIncident(g, src) == NULL && GraphOutIncident(g, src) == NULL) {
			nvs.values[src] = 0;
		} else if (dist_sum == 0) {
			nvs.values[src] = 0;
		} else if (dist_sum != 0) {
			nvs.values[src] = (reachable - 1)/(nV - 1) * (reachable - 1)/(dist_sum);
		} 
		
		freeShortestPaths(path);
	}
	return nvs;
}

NodeValues betweennessCentrality(Graph g) {
	NodeValues nvs;
	nvs.numNodes = GraphNumVertices(g);
	nvs.values = calloc(nvs.numNodes, sizeof(double));
	// for each node that could be in the middle of a shortest path
	for (Vertex mid_node = 0; mid_node < nvs.numNodes; mid_node++) {
		// for each src
		for (Vertex src = 0; src < nvs.numNodes; src++) {
			ShortestPaths path = dijkstra(g, src);
			// for each dest vert
			for (Vertex dest = 0; dest < nvs.numNodes; dest++) {
				// for each node in between src and dest vert
				if (mid_node != src && mid_node != dest && src != dest) {
					double total_paths = 0, counter = 0;
					total_paths = findTotalPaths(path, src, dest);
					counter = findIncluded(path, src, dest, mid_node);
					// can't divide by 0
					if (total_paths != 0) {
						nvs.values[mid_node] += counter / total_paths;
					}
				}	
			}
			freeShortestPaths(path);
		}
	}
	return nvs;
}

double findTotalPaths(ShortestPaths path, Vertex src, Vertex dest) {
	double total_paths = 0;
	// only 1 path if src = dest;
	if (src == dest) {
		return 1;
	// else, iteratively go through each pred node and recursively find
	// all shortest paths
	} else {
		for (PredNode *curr = path.pred[dest]; curr != NULL; curr = curr->next) {
			total_paths += findTotalPaths(path, src, curr->v);
		}
	}
	return total_paths;
}

double findIncluded(ShortestPaths path, Vertex src, Vertex dest, Vertex mid_node) {
	double instances = 0;
	// return total paths if mid_node = dest
	if (mid_node == dest) {
		instances = findTotalPaths(path, src, mid_node);
	// else, iteratively go through each pred node and recursively find
	// paths with mid_node in the middle
	} else {
		for (PredNode *curr = path.pred[dest]; curr != NULL; curr = curr->next) {
			instances += findIncluded(path, src, curr->v, mid_node);
		}
	}
	return instances;
}

NodeValues betweennessCentralityNormalised(Graph g) {
	NodeValues nvs = betweennessCentrality(g);
	for (int i = 0; i < nvs.numNodes; i++) {
		nvs.values[i] = nvs.values[i] / (nvs.numNodes - 1) / (nvs.numNodes - 2);
	}
	return nvs;
}

void showNodeValues(NodeValues nvs) {
	for (int j = 0; j < nvs.numNodes; j++) {
		printf("%d: %lf\n", j, nvs.values[j]);
	}
}

void freeNodeValues(NodeValues nvs) {

}

