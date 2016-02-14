#ifndef MAP_H_
#define MAP_H_

typedef struct _Connection {
    float cost;
    struct NodeStruct *ptr;
} Connection;

// Types:
// 0 is regular black node (unnamed on the flex map)
// 1 is a pizza pick up node
// 2 is a house door node
// 3 is a house deposit node
typedef struct NodeStruct Node; // Forward declaration
typedef struct NodeStruct {
    // A char array for the name string. TODO: Remove for finals
    char *name;
    // Co-ordinates are relative to the "start" block
    int x;
    int y;
    // Length of the array called "connected" below
    int num_connected;
    // So we can easily "push" more nodes using the function. Just counts how many connections have been made already
    int counter;

    // NOT SET MANUALLY. This is used during Dijkstra's algorithm and is subject to change based on the source & target
    float path_cost;
    // Boolean value used during Dijkstra's algorithm to indicate whether or not the optimal path for the current
    // node has been found
    // TODO: Consider using bit fields?
    int done;
    // Pointer to the node which got us to this node in Dijkstra's algorithm.
    // We store the prev_node so that we can go backwards from the target node to find
    // the path we took to get there all the way from the source node.
    // For A-B-C-D
    // D->prev_node = C
    // C->prev_node = B
    // B->prev_node = A (which is the source)
    // If we use reverse iteration like this in a stack, we can pop the nodes off the stack
    // and just move the bot to each node one by one
    Node *prev_node;
    // The angle at which we arrive at the current node. Also used in Dijsktra's to add a rotation_cost
    float enter_radians;

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

typedef struct _Graph {
    Node *start; // For our map, we can get to any node from the starting position, so all we need is this
    int num_nodes;
} Graph;

// A "stack" of Nodes. Len indicates the next empty element, so to use it as a stack, use
// for (i = top-1; i >= 0; i--)
typedef struct _PathStack {
    Node **path;
    int top;
    float total_cost;
} PathStack;

// A struct to store the position of the bot
typedef struct _Pos {
    Node *cur_node;
    float cur_radians;
} Position;

Node* CreateNode(int x, int y, int num_connected, char *name);
void ConnectNodes(Node *a, Node *b, float cost);
void InitGraph();
Node* GetCurrentNode();

void DFSEval(Node *source_node, int unvisited_value, void fn());
void InitNodesDijkstra(Node* current_node);
void MoveBotToNode(Node* target_node);
PathStack* Dijsktra(Node *source_node, Node *target_node);

#endif