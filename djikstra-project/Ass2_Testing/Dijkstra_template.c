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

	PQ q = PQNew();
	PQInsert(q, src, 0);

	// dist from source is 0
    path.dist[src] = 0;

	while (!PQIsEmpty(q)) {
		Vertex curr_v = PQDequeue(q);

		AdjList curr = GraphOutIncident(g, curr_v);
		for (; curr != NULL; curr = curr->next) {
			
			//int vert_dist = path.dist[curr_v] + curr->weight;
			PQInsert(q, curr->v, path.dist[curr_v] + curr->weight);
			
			if (path.dist[curr_v] + curr->weight < path.dist[curr->v]) {
				PredNode *curr_node = path.pred[curr->v];
				PredNode *prev_node = path.pred[curr->v];
				path.dist[curr->v] = path.dist[curr_v] + curr->weight;
				if (curr_node == NULL) {
				} else if (curr_node->next == NULL) {
					free(curr_node);
				} else {
					while (curr_node != NULL) {
						curr_node = curr_node->next;
						free(prev_node);
						prev_node = curr_node;
					}
				}
				path.pred[curr->v] = NULL;
				// dist
				path.dist[curr->v] =  path.dist[curr_v] + curr->weight;
				// queue
				PredNode *node = malloc(sizeof(PredNode));
				node->next = NULL;
				node->v = curr_v;
				path.pred[curr->v] = node;
				

			} else if (path.dist[curr_v] + curr->weight == path.dist[curr->v]) {
				PredNode *current_node = path.pred[curr->v];
				while (current_node->next != NULL) {
					current_node = current_node->next;
				}
				PredNode *new_node = malloc(sizeof(PredNode));
				new_node->v = curr_v;
				new_node->next = NULL;
				current_node->next = new_node;

			}
		}	
	}	
	PQFree(q);
	return path;
}

// ShortestPaths create_path(Graph g, Vertex src) {
//     ShortestPaths new_path = malloc(sizeof(struct ShortestPaths));
//     new_path.numNodes = GraphNumVertices(g);
//     new_path.dist= malloc(sizeof(int) * GraphNumVertices(g));
//     new_path.pred = malloc(sizeof(struct PredNode) * GraphNumVertices(g));
//     new_path.src = src;
    
//     return new_path;
// }

void showShortestPaths(ShortestPaths sps) {

}

void freeShortestPaths(ShortestPaths sps) {

}

