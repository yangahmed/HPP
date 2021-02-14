#ifndef __quadtree__
#define __quadtree__

typedef struct treeNode
{
    int isLeaf;        //if a node is a leaf (isLeaf == 1), means this node represent a partical; otherwise (isLeaf == 0), it represent a box containing particals
    double m_x;
    double m_y;        //center of mass; for a partical or a box contains only 1 partical, it equals to the partical's position
    double m;          //total mass for a box containing many particals; for a partical it's its mass
    double c_x;
    double c_y;        //centroid of the box
    double width;      //width of the box
    struct treeNode* ne;
    struct treeNode* nw;
    struct treeNode* sw;
    struct treeNode* se;   //each node has 4 children nodes
} treenode;

void insert(treenode **node, double x, double y, double m);
void split(treenode **node);
void free_tree(treenode *node);


#endif