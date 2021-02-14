#include <stdio.h>
#include <stdlib.h>
#include "quadtree.h"

void insert(treenode **node, double x, double y, double m)
{
    /*insert a new node to given quadtree*/
    if (!*node)
    {
        /*if the given tree is empty, create the tree and the given partical as the first node*/
        *node = (treenode *)malloc(sizeof(treenode));
        (*node)->isLeaf = 1;
        (*node)->m_x = x;
        (*node)->m_y = y;
        (*node)->m = m;
        (*node)->width = 1;
        (*node)->c_x = 0.5 * ((*node)->width);
        (*node)->c_y = 0.5 * ((*node)->width);
        (*node)->ne = NULL;
        (*node)->nw = NULL;
        (*node)->sw = NULL;
        (*node)->se = NULL;
        return;
    }

    if (*node)
    {
        if ((*node)->isLeaf == 1)
        {
            /*if the given partical belongs to a leaf node box, 
            which mean the box already contain one partical, we should split this box,
            and insert the new partical in lower level*/
            split(node);
        }

        if((*node)->isLeaf == 0) {
            /*when a new partical is inserted to a "non leaf" box, compare the position of
            the new partical and the centroid of the box, in order to allocate the new node
            on northeast, northwest, southeast or southwest of that box*/
            if(x >= (*node)->c_x) {
                if(y >= (*node)->c_y) {
                    if(!(*node)->ne) {
                        treenode *tn = (treenode *)malloc(sizeof(treenode));
                        tn->isLeaf = 1;
                        tn->m_x = x;
                        tn->m_y = y;
                        tn->m = m;
                        tn->ne = NULL;
                        tn->nw = NULL;
                        tn->sw = NULL;
                        tn->se = NULL;
                        tn->width = (*node)->width * 0.5;
                        tn->c_x =(*node)->c_x + tn->width * 0.5;
                        tn->c_y =(*node)->c_y + tn->width * 0.5;
                        (*node)->ne = tn;
                    }
                    else {
                        /*if the location is occupied, insert the new node to lower level*/
                        insert(&((*node)->ne), x, y, m);
                    }
                }

                else {
                    if(!(*node)->se) {
                        treenode *tn = (treenode *)malloc(sizeof(treenode));
                        tn->isLeaf = 1;
                        tn->m_x = x;
                        tn->m_y = y;
                        tn->m = m;
                        tn->ne = NULL;
                        tn->nw = NULL;
                        tn->sw = NULL;
                        tn->se = NULL;
                        tn->width = (*node)->width * 0.5;
                        tn->c_x =(*node)->c_x + tn->width * 0.5;
                        tn->c_y =(*node)->c_y - tn->width * 0.5;
                        (*node)->se = tn;
                    }
                    else {
                        insert(&((*node)->se), x, y, m);
                    }
                }
            }

            else {
                if(y >= (*node)->c_y) {
                    if(!(*node)->nw) {
                        treenode *tn = (treenode *)malloc(sizeof(treenode));
                        tn->isLeaf = 1;
                        tn->m_x = x;
                        tn->m_y = y;
                        tn->m = m;
                        tn->ne = NULL;
                        tn->nw = NULL;
                        tn->sw = NULL;
                        tn->se = NULL;
                        tn->width = (*node)->width * 0.5;
                        tn->c_x =(*node)->c_x - tn->width * 0.5;
                        tn->c_y =(*node)->c_y + tn->width * 0.5;
                        (*node)->nw = tn;
                    }
                    else {
                        insert(&((*node)->nw), x, y, m);
                    }
                }

                else {
                    if(!(*node)->sw) {
                        treenode *tn = (treenode *)malloc(sizeof(treenode));
                        tn->isLeaf = 1;
                        tn->m_x = x;
                        tn->m_y = y;
                        tn->m = m;
                        tn->ne = NULL;
                        tn->nw = NULL;
                        tn->sw = NULL;
                        tn->se = NULL;
                        tn->width = (*node)->width * 0.5;
                        tn->c_x =(*node)->c_x - tn->width * 0.5;
                        tn->c_y =(*node)->c_y - tn->width * 0.5;
                        (*node)->sw = tn;
                    }
                    else {
                        insert(&((*node)->sw), x, y, m);
                    }
                }
            }

            /*calculate the center of mass of this box*/
            (*node)->m_x = ((*node)->m_x * (*node)->m + x * m)/((*node)->m+m);
            (*node)->m_y = ((*node)->m_y * (*node)->m + y * m)/((*node)->m+m);
            (*node)->m += m;

        }
    }
}

void split(treenode **node) {
    /*split a leaf box to four boxes, and allocate the partical it contained*/
    
    /*create a node to store the partical contained by original box*/
    treenode *tn = (treenode *)malloc(sizeof(treenode));
    tn->isLeaf = 1;
    tn->m_x = (*node)->m_x;
    tn->m_y = (*node)->m_y;
    tn->m = (*node)->m;
    tn->width = 0.5*((*node)->width);
    tn->ne = NULL;
    tn->nw = NULL;
    tn->sw = NULL;
    tn->se = NULL;

    /*allocate that node*/
    if(tn->m_x >= (*node)->c_x) {
        if(tn->m_y >= (*node)->c_y) {
            (*node)->ne = tn;
            tn->c_x = (*node)->c_x + 0.5*(tn->width);
            tn->c_y = (*node)->c_y + 0.5*(tn->width);
            (*node)->isLeaf = 0;
        }
        else {
            (*node)->se = tn;
            tn->c_x = (*node)->c_x + 0.5*(tn->width);
            tn->c_y = (*node)->c_y - 0.5*(tn->width);
            (*node)->isLeaf = 0;
        }
    }

    else {
        if(tn->m_y >= (*node)->c_y) {
            (*node)->nw = tn;
            tn->c_x = (*node)->c_x - 0.5*(tn->width);
            tn->c_y = (*node)->c_y + 0.5*(tn->width);
            (*node)->isLeaf = 0;
        }
        else {
            (*node)->sw = tn;
            tn->c_x = (*node)->c_x - 0.5*(tn->width);
            tn->c_y = (*node)->c_y - 0.5*(tn->width);
            (*node)->isLeaf = 0;
        }
    }
}


void free_tree(treenode *node) {
    /*delete a tree, and free memory*/
    if(node->ne) 
        free_tree(node->ne);
    if(node->nw) 
        free_tree(node->nw);
    if(node->sw) 
        free_tree(node->sw);
    if(node->se) 
        free_tree(node->se);
    free(node);
    node = NULL;
}