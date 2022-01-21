// z5316983 Jeremy Lee

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "Dijkstra.h"
#include "PQ.h"

#define MAX_INT 2147483647

// ShortestPaths create_path(Graph g, Vertex src);
void freePred(PredNode *current);

ShortestPaths dijkstra(Graph g, Vertex src) {

    // ShortestPaths path = create_path(g, src);
    ShortestPaths path;
    path.numNodes = GraphNumVertices(g);
    path.dist= malloc(sizeof(int) * GraphNumVertices(g));
    path.pred = malloc(sizeof(PredNode) * GraphNumVertices(g));
    path.src = src;

    for (int i = 0; i < path.numNodes; i++) {
        path.dist[i] = MAX_INT;
        path.pred[i] = NULL;
    }
	// dist from source is 0
    path.dist[src] = 0;
	
    // adding src into PQ
    PQ q = PQNew();
	PQInsert(q, src, path.dist[src]);
	// Loop through PQ 
	while (!PQIsEmpty(q)) {
		Vertex curr_vertex = PQDequeue(q);
		AdjList curr = GraphOutIncident(g, curr_vertex);
        // Loop through neighbours
		for (; curr != NULL; curr = curr->next) {
			// Set path.dist
			if (path.dist[curr_vertex] + curr->weight < path.dist[curr->v] 
				&& path.dist[curr_vertex] != MAX_INT) {
				// Update distance
				path.dist[curr->v] = path.dist[curr_vertex] + curr->weight;
				// free pred
				PredNode *curr_node = path.pred[curr->v];
				freePred(curr_node);
				// create new node and set pred head as new node
				PredNode *new_node = malloc(sizeof(PredNode));
				new_node->v = curr_vertex;
				new_node->next = NULL;
				path.pred[curr->v] = new_node;
				// insert pred into PQ
				PQInsert(q, curr->v, path.dist[curr->v]);
			
			} else if (path.dist[curr_vertex] + curr->weight == path.dist[curr->v]) {
				PredNode *current_node = path.pred[curr->v];
				PredNode *new_node = malloc(sizeof(PredNode));
				new_node->v = curr_vertex;
				new_node->next = NULL;
				while (current_node->next != NULL) {
					current_node = current_node->next;
				}
				current_node->next = new_node;
			} 
		}	
	}	
	// sets any inf to 0
	for (int k = 0; k < path.numNodes; k++) {
		if (path.dist[k] == MAX_INT) path.dist[k] = 0;
	}
	// finally, free the PQ
	PQFree(q);

	return path;
}

// Recursively frees a predecessor list
void freePred(PredNode *current) {
	if (current == NULL) return;
	freePred(current->next);
	free(current);
}

void showShortestPaths(ShortestPaths sps) {

}

void freeShortestPaths(ShortestPaths sps) {
	for (int i = 0; i < sps.numNodes; i++) freePred(sps.pred[i]);
	free(sps.dist);
	free(sps.pred);
}

