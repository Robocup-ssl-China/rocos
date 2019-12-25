#ifndef MONTAGE_H
#define MONTAGE_H
#include <singleton.hpp>
class CMontage
{
public:
    CMontage();
    void run(bool);
};
typedef Singleton<CMontage> Montage;
#endif // MONTAGE_H
