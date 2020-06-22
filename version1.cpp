#include<cstdio>
#include<cstring>
const char sideName[2][5] = {"red","blue"};
const char warriorName[5][7] = {"dragon","ninja","iceman","lion","wolf"};
const int generateOrder[2][5] = {2,3,4,1,0,3,0,1,2,4};
int warriorInitStrength[5];
int M, mytime = 0;
class headQuarter
{
    friend class warrior;
private:
    int side;
    int unit;
    int warriorCount;
    int warriorPerType[5];
    int nextGenerateOrder;
public:
    headQuarter(int _side);
    ~headQuarter();
    bool generateWarrior();
};

class warrior
{
private:
    int side;
    int id;
    int kind;
    int strength;
public:
    warrior(headQuarter & _master, int _kind);
    ~warrior();
};

headQuarter::headQuarter(int _side):side(_side),unit(M),warriorCount(0),nextGenerateOrder(0)
{
    memset(warriorPerType,0,5*sizeof(int));
}

headQuarter::~headQuarter()
{
}

bool headQuarter::generateWarrior()
{
    int nextGenerateKind;
    for (int i = 0; i < 5; i++)
    {
        nextGenerateKind = generateOrder[side][(nextGenerateOrder++)%5];
        if (warriorInitStrength[nextGenerateKind] <= unit)
        {
            warrior * temp = new warrior(*this,nextGenerateKind);
            delete temp;
            return true;
        }
    }
    return false;
}

warrior::warrior(headQuarter & _master, int _kind):side(_master.side),id(++_master.warriorCount),kind(_kind),strength(warriorInitStrength[_kind])
{
    _master.unit -= warriorInitStrength[_kind];
    printf("%03d %s %s %d born with strength %d,%d %s in %s headquarter\n",mytime,sideName[side],warriorName[kind],id,strength,(++_master.warriorPerType[kind]),warriorName[kind],sideName[side]);
}

warrior::~warrior()
{
}

int main()
{
    int totalCase = 0;
    bool flag[2];
    scanf("%d",&totalCase);
    for (int Case = 1; Case <= totalCase; Case++)//测试数据的编号从1开始
    {
        scanf("%d",&M);
        for (int i = 0; i < 5; i++)
        {
            scanf("%d",warriorInitStrength + i);
        }
        headQuarter * headQuarters = new headQuarter[2]{0, 1};
        mytime = 0;
        memset(flag,0,2*sizeof(bool));
        printf("Case:%d\n",Case);
        while (!flag[0] || !flag[1])
        {
            for (int i = 0; i < 2; i++)
            {
                if (!headQuarters[i].generateWarrior())
                {
                    if (!flag[i])
                    {
                        printf("%03d %s headquarter stops making warriors\n",mytime,sideName[i]);
                        flag[i] = true;
                    }

                }   
            }
            mytime++;
        }
        delete [] headQuarters;
    }
    return 0;
}