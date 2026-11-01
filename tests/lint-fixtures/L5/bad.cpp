// must-fail fixture for L5: function-local static with non-trivial init (C03)
struct Counter
{
    int v;
};

int tick_count()
{
    static Counter c;
    return c.v;
}
