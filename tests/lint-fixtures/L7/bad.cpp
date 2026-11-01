// must-fail fixture for L7: raw new/delete in engine code (C29)
struct Node
{
    int v;
};

int alloc_one()
{
    Node* n = new Node();
    delete n;
    return 0;
}
