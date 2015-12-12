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

    // An array of "connection" structure pointers. As seen above, every connection has a cost associated with it (in time)
    // and a pointer to a node.
    // So, for example, node A might be connected to B, C, D.
    // A.connected[0] will be a struct which tells you the cost from A to B, and lets you move to node B from A.
    Connection **connected;
} Node;



Node* CreateNode(int x, int y, int num_connections);
void ConnectNodes(Node *a, Node *b, int cost);

void InitAllNodes();

#endif