#ifndef MAP_H_
#define MAP_H_

typedef struct _Connection {
    int cost;
    struct NodeStruct *ptr;
} Connection;

// Types:
// 0 is regular black node (unnamed on the flex map)
// 1 is a pizza pick up node
// 2 is a house door node
// 3 is a house deposit node
typedef struct NodeStruct {
    unsigned int type;
    // Co-ordinates are relative to the "start" block
    int x;
    int y;
    // Length of the array called "connected" below
    int num_connections;
    // So we can easily "push" more nodes using the function. Just counts how many connections have been made already
    int counter;

    // NOT SET MANUALLY. This is used during Dijkstra's algorithm and is subject to change based on the source & target
    int path_cost;
    // Boolean value used during Dijkstra's algorithm to indicate whether or not the optimal path for the current
    // node has been found
    // TODO: Consider using bit fields?
    int done;

    // Used as a toggle / flag during DFS. Since we'll be running DFS several times, the plan is to
    // check the source node's visited value initially, and use that as the "unvisited value".
    // From then on, any nodes encountered which have the same value will be assumed to be univisted.
    int visited;

    // An array of "connection" structure pointers. As seen above, every connection has a cost associated with it (in time)
    // and a pointer to a node.
    // So, for example, node A might be connected to B, C, D.
    // A.connected[0] will be a struct which tells you the cost from A to B, and lets you move to node B from A.
    Connection **connected;
} Node;

Node* CreateNode(int x, int y, int num_connections);
void ConnectNodes(Node *a, Node *b, int cost);
void InitGraph();
Node* GetCurrentNode();

void DFSEval(Node *source_node, int unvisited_value, void fn());
void InitNodesDijkstra(Node* current_node);
#endif