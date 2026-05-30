#ifndef STATCHANGE_H
#define STATCHANGE_H

struct StatChange
{
    enum Target { Hp, Hunger, Thirst, Sanity, Rest };
    Target target;
    double amount; // 正值=获得，负值=消耗
};

#endif // STATCHANGE_H
