#ifndef AVL_H
#define AVL_H

#include "list.h"

template <typename T>
class AVL
{
public:

    AVL() : rootNode(NULL) {}

    ~AVL()
    {
        destroyTree(this->rootNode);
        rootNode = NULL;
    }

    void insert(T element)
    {
        insertInAVLRec(rootNode, element);
    };

    void printInOrder()
    {
        printTreeInOrderRec(rootNode);
    };

    /// @brief Retorna los nodos del árbol en una lista en orden. 
    /// Si el ábrol es vacío, retorna una lista vacía.
    List<T> * toList()
    {
        List<T> * list = new List<T>();
        if(rootNode != NULL)
        {
            toListRec(list, rootNode);
        } 
        return list;
    }

private:
    class AVLNode
    {
    public:
        int height;
        T data;
        AVLNode *leftNode;
        AVLNode *rightNode;
        AVLNode(T data) : data(data), leftNode(NULL), rightNode(NULL), height(1) {}
        ~AVLNode() {}
    };

    AVLNode *rootNode;
    
    void toListRec(List<T> * list, const AVLNode * node)
    {
        if(node != NULL)
        {
            toListRec(list, node->leftNode);
            list->add(node->data);
            toListRec(list, node->rightNode);
        }
    }

    void destroyTree(AVLNode *treeNode)
    {
        if (treeNode != NULL)
        {
            destroyTree(treeNode->leftNode);
            destroyTree(treeNode->rightNode);

            delete treeNode;
            
            treeNode = NULL;
        }
    };

    int height(AVLNode *treeNode)
    {
        int result = 0;

        if (treeNode != NULL)
        {
            result = treeNode->height;
        }

        return result;
    };

    int getBalance(AVLNode *treeNode)
    {
        int result = 0;

        if (treeNode != NULL)
        {
            result = height(treeNode->rightNode) - height(treeNode->leftNode);
        }

        return result;
    };

    void leftRotation(AVLNode *&unbalancedNode)
    {
        AVLNode *rightChild = unbalancedNode->rightNode;
        AVLNode *subtree = rightChild->leftNode;

        rightChild->leftNode = unbalancedNode;
        unbalancedNode->rightNode = subtree;

        unbalancedNode->height = max(height(unbalancedNode->leftNode), height(unbalancedNode->rightNode)) + 1;
        rightChild->height = max(height(rightChild->leftNode), height(rightChild->rightNode)) + 1;

        unbalancedNode = rightChild;
    };

    void rightRotation(AVLNode *&unbalancedNode)
    {
        AVLNode *leftChild = unbalancedNode->leftNode;
        AVLNode *subtree = leftChild->rightNode;

        leftChild->rightNode = unbalancedNode;
        unbalancedNode->leftNode = subtree;

        unbalancedNode->height = max(height(unbalancedNode->leftNode), height(unbalancedNode->rightNode)) + 1;
        leftChild->height = max(height(leftChild->leftNode), height(leftChild->rightNode)) + 1;

        unbalancedNode = leftChild;
    };

    void insertInAVLRec(AVLNode *&treeNode, T element)
    {
        if (treeNode == NULL)
        {
            AVLNode *newNode = new AVLNode(element);
            treeNode = newNode;
        }
        else
        {
            if (element <= treeNode->data)
            {
                insertInAVLRec(treeNode->leftNode, element);
            }
            else
            {
                insertInAVLRec(treeNode->rightNode, element);
            }
        }

        treeNode->height = 1 + max(height(treeNode->leftNode), height(treeNode->rightNode));

        int balance = getBalance(treeNode);

        if (balance < -1)
        { // I

            if (element <= treeNode->leftNode->data)
            { // II
                rightRotation(treeNode);
            }
            else
            { // ID
                leftRotation(treeNode->leftNode);
                rightRotation(treeNode);
            }
        }
        else if (balance > 1)
        { // D

            if (element > treeNode->rightNode->data)
            { // D
                leftRotation(treeNode);
            }
            else
            { // DI
                rightRotation(treeNode->rightNode);
                leftRotation(treeNode);
            }
        }
    };

    void printTreeInOrderRec(AVLNode *tree)
    {
        if (tree != NULL)
        {
            printTreeInOrderRec(tree->leftNode);
            cout << tree->data << endl;
            printTreeInOrderRec(tree->rightNode);
        };
    };
};

#endif