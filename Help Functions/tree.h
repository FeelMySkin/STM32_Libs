#ifndef TREE_H
#define TREE_H
#include "defines.h"

struct TreeData
{
    TreeData *left;
    TreeData *right;
	void *payload;
    uint8_t data;
};

class Tree
{
    public:
        void AddData(uint8_t data, void* payload = nullptr);
        void RotateLeft(uint8_t data);
        void RotateRight(uint8_t data);
        void RemoveData(uint8_t data);
        void Normalize();
        uint16_t Depth();

		void *GetPayload(uint8_t data);

    private:
        uint16_t max_depth_found;
        void DepthWalker(TreeData* curr, uint16_t curr_depth);
        bool Normalize(TreeData* next);

        uint16_t NodesCount(TreeData* from);
        uint16_t Depth(TreeData* from);

        TreeData* Find(uint8_t data);
        TreeData* FindParent(TreeData* child);
        TreeData *tree;

        void RotateLeft(TreeData* node);
        void RotateRight(TreeData* node);



};
#endif