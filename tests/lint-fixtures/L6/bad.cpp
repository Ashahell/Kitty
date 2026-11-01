// must-fail fixture for L6: namespace-scope object with a non-trivial ctor (C03/C29)
struct Palette
{
    Palette(int n);
};

static Palette g_palette(4);
