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
struct NodeStruct; // Forward declaration
typedef struct NodeStruct {
    // A char array for the name string. TODO: Remove for finals
    char *name;
    // Co-ordinates are relative to the "start" block
    float x;
    float y;
    // Length of the array called "connected" below
    char num_connected;
    // So we can easily "push" more nodes using the function. Just counts how many connections have been made already
    char counter;

    // NOT SET MANUALLY. This is used during Dijkstra's algorithm and is subject to change based on the source & target
    float path_cost;
    // Boolean value used during Dijkstra's algorithm to indicate whether or not the optimal path for the current
    // node has been found
    // TODO: Consider using bit fields?
    char done;
    // Pointer to the node which got us to this node in Dijkstra's algorithm.
    // We store the prev_node so that we can go backwards from the target node to find
    // the path we took to get there all the way from the source node.
    // For A-B-C-D
    // D->prev_node = C
    // C->prev_node = B
    // B->prev_node = A (which is the source)
    // If we use reverse iteration like this in a stack, we can pop the nodes off the stack
    // and just move the bot to each node one by one
    struct NodeStruct *prev_node;
    // The angle at which we arrive at the current node. Also used in Dijsktra's to add a rotation_cost
    float enter_deg;

    // Used as a toggle / flag during DFS. Since we'll be running DFS several times, the plan is to
    // check the source node's visited value initially, and use that as the "unvisited value".
    // From then on, any nodes encountered which have the same value will be assumed to be univisted.
    char visited;

    // An array of "connection" structure pointers. As seen above, every connection has a cost associated with it (in time)
    // and a pointer to a node.
    // So, for example, node A might be connected to B, C, D.
    // A.connected[0] will be a struct which tells you the cost from A to B, and lets you move to node B from A.
    Connection **connected;
} Node;

typedef struct _Graph {
    Node *start; // For our map, we can get to any node from the starting position, so all we need is this
    char num_nodes;
} Graph;

typedef struct _CurveInfo {
    Node *curve_center; // The center of the circle whose arc segments form the curve on the map
    Node **curve_nodes; // Nodes that are involved in the arcs
    char curve_nodes_len; // Number of nodes involved
} CurveInfo;

Node *CreateNode(float x, float y, char num_connected, char *name);
void ConnectNodes(Node *a, Node *b, float cost);
void InitGraph();
Node *GetCurrentNode();
void DFSEval(Node *source_node, char unvisited_value, char (*fn)(Node *));
char CheckNodeName(Node *current_node);
Node *GetNodeByName(char *name);
char IndexOfNode(Node **node_arr, unsigned char len, Node *needle);
void UpdateNodeInArray(Node **node_costs, unsigned char *len, Node *new_node);
CurveInfo *GetCurveInfo();
Graph *GetGraph();
float GetRotationCost(float deg);
float RadToDeg(float radians);
float MakePositiveDeg(float angle);
float MakePositiveRad(float angle);
float GetAngularVelocity(Node *node1, Node *node2);
char GetCurveDirection(Node *source_node, Node *target_node);
int GetShortestDeg(int deg);

#endif