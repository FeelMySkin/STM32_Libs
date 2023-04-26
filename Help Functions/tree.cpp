#include "tree.h"

void Tree::Normalize()
{
    if(tree == nullptr) return;
    while(!Normalize(tree)) ;
}

bool Tree::Normalize(TreeData* next)
{
    uint16_t left_depth, right_depth;

    while(next->left != nullptr)
    {
        if(Normalize(next->left)) break;
    }
    while(next->right != nullptr)
    {
        if(Normalize(next->right)) break;
    }
    
    //if(next->left !=nullptr) while(!Normalize(next->left)) ;
    //if(next->right !=nullptr) while(!Normalize(next->right)) ;

    max_depth_found = 0;
    if(next->left != nullptr) DepthWalker(next->left,1);
    left_depth = max_depth_found;

    max_depth_found = 0;
    if(next->right != nullptr) DepthWalker(next->right,1);
    right_depth = max_depth_found;

    if(left_depth>right_depth+1)
    {
        if(NodesCount(next->right)*2 <= NodesCount(next->left))RotateLeft(next->left);
        //if((next->right == nullptr || next->right->right == nullptr) && (next->left->left  == nullptr || next->left->left->left  == nullptr)) RotateLeft(next->left);
        RotateRight(next);
		return false;
    }
    else if(right_depth>left_depth+1)
    {
        if(NodesCount(next->left)*2 <= NodesCount(next->right))RotateLeft(next->right);
        //if((next->left == nullptr || next->left->left == nullptr) && (next->right->right  == nullptr || next->right->right->right  == nullptr)) RotateLeft(next->right);
        RotateLeft(next);
        return false;
    }



    return true;

}

uint16_t Tree::NodesCount(TreeData* from)
{
    if(from == nullptr) return 0;
    TreeData* cursor = from;
    uint16_t dat = 0;
    if(cursor->right != nullptr) dat += NodesCount(cursor->right);
    if(cursor->left != nullptr) dat += NodesCount(cursor->left);
    dat+=1;
    return dat;
}

void Tree::AddData(uint8_t data, void* pload)
{
    TreeData *newer = new TreeData();
    newer->data = data;
	newer->payload = pload;
    if(tree == nullptr)
    {
        tree = newer;
        return;
    }
    if(Find(data) != nullptr) return;
    TreeData *cursor = tree;
    while(1)
    {
        if(data > cursor->data)
        {
            if(cursor->right == nullptr)
            {
                cursor->right = newer;
                return;
            }
            else cursor = cursor->right;
        }
        else
        {
            if(cursor->left == nullptr)
            {
                cursor->left = newer;
                return;
            }
            else cursor = cursor->left;
        }
    }
}

void Tree::RotateLeft(uint8_t data)
{
    TreeData *rotator = Find(data);
    RotateLeft(rotator);
}

void Tree::RotateRight(uint8_t data)
{
    TreeData *rotator = Find(data);
    RotateRight(rotator);
}

void Tree::RotateLeft(TreeData* rotator)
{
    if(rotator->right == nullptr) return;
    TreeData *parent = FindParent(rotator);
    if(rotator == tree)
    {
        tree = rotator->right;
        rotator->right = tree->left;
        tree->left = rotator;
    }
    else 
    {
        /*if(parent->right == rotator) parent->right = rotator->right;
        else parent->left = rotator->right;
        TreeData* cursor = rotator->right;
        while(cursor->left != nullptr) cursor = cursor->left;
        cursor->left = rotator;*/

        if(parent->left == rotator) parent->left = rotator->right;
        else parent->right = rotator->right;
        TreeData* cursor = rotator->right;

        rotator->right = rotator->right->left;
        cursor->left = rotator;
    }
}

void Tree::RotateRight(TreeData* rotator)
{
    if(rotator->left == nullptr) return;
    TreeData *parent = FindParent(rotator);
    if(rotator == tree)
    {
        tree = rotator->left;
        rotator->left = tree->right;
        tree->right = rotator;
    }
    else 
    {
        if(parent->right == rotator) parent->right = rotator->left;
        else parent->left = rotator->left;
        TreeData* cursor = rotator->left;

        rotator->left = rotator->left->right;
        cursor->right = rotator;
        
    }

}

void *Tree::GetPayload(uint8_t data)
{
	TreeData* ret = Find(data);

	return ret == nullptr?nullptr:ret->payload;
}

TreeData* Tree::Find(uint8_t data)
{
    TreeData* cursor = tree;

    while(1)
    {
        if(cursor == nullptr) return nullptr;
        if(cursor->data == data) return cursor;
        if(data > cursor->data) cursor = cursor->right;
        else cursor = cursor->left;
    }
}

TreeData* Tree::FindParent(TreeData* child)
{
    TreeData* cursor = tree;
    if(child == cursor) return cursor;

    while(1)
    {
        
        if(((cursor-> left != nullptr) && (child->data == cursor->left->data)) || 
            ((cursor-> right != nullptr) && (child->data == cursor->right->data))) return cursor;
        else if((cursor-> right != nullptr) && (child->data > cursor->data)) cursor = cursor->right;
        else if((cursor-> left != nullptr) && (child->data < cursor->data)) cursor = cursor->left;

    }
}

uint16_t Tree::Depth()
{
    max_depth_found = 0;
    if(tree == nullptr) return 0;
    //max_depth_found = 1;
    DepthWalker(tree,1);
    return max_depth_found;

}

uint16_t Tree::Depth(TreeData* from)
{
    max_depth_found = 0;
    if(from == nullptr) return 0;
    //max_depth_found = 1;
    DepthWalker(from,1);
    return max_depth_found;
}

void Tree::DepthWalker(TreeData* curr, uint16_t curr_depth)
{
    if(curr_depth>max_depth_found) max_depth_found = curr_depth;

    if(curr->right != nullptr) DepthWalker(curr->right,curr_depth+1);
    if(curr->left != nullptr) DepthWalker(curr->left,curr_depth+1);
}

void Tree::RemoveData(uint8_t data)
{
    if(Find(data) == nullptr) return;
    TreeData* cursor = tree;
    TreeData* parent = cursor;

    while(cursor->data != data)
    {
        parent = cursor;
        if(data>cursor->data) cursor = cursor->right;
        else cursor = cursor->left;
    }

    if(cursor == parent->right)
    {
         parent->right = cursor->left;
         TreeData* buf = parent;
        while (buf->right != nullptr) buf = buf->right;
        buf->right = cursor->right;
    }
    else
    {
        parent->left = cursor->right;
        TreeData* buf = parent;
        while (buf->left != nullptr) buf = buf->left;
        buf->left = cursor->left;
    }
    delete cursor;


}