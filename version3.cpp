#include<cstdio>
#include<iostream>
#include<cmath>
#include<cstring>
#include<string>
#include<memory.h>
#include<vector>
#include<set>
#include<algorithm>
using namespace std;

#define ERR 0.0001

enum _warriorKind{dragon, ninja, iceman, lion, wolf};

enum _weaponName{sword, bomb, arrow};

enum _sideKind{red, blue};

// enum _weaponInfo{weaponNumber, weaponUsedTime, weaponForce};

const char sideName[2][5] = {"red","blue"};
const char warriorName[5][7] = {"dragon","ninja","iceman","lion","wolf"};
const char weaponName[3][7] = {"sword", "bomb", "arrow"};
const int generateOrder[2][5] = {2,3,4,1,0,3,0,1,2,4};// 武士生产方式，先红后蓝
int warriorInitElements[5];
int warriorInitForce[5];

int M, N, K, T, mytime = 0, finished = 0;

inline void when()// 时间，最后带一个空格
{
    printf("%03d:%02d ", mytime/60, mytime%60);
}

class CWarrior;

class CHeadquarter;

CHeadquarter* headquarters[2];

class CCity;

CCity* citys[25];

class CWeapon
{
public:
    int kind;
    int used;
    virtual bool hit(CWarrior* a, CWarrior* b)=0;// 如果武器用完就没了，返回true
    static CWeapon* getWeapon(int kind);
    CWeapon(int _kind):kind(_kind),used(0){}
    void which(){printf("%s", weaponName[kind]);}
};

class CSword:public CWeapon
{
public:
    bool hit(CWarrior* a, CWarrior* b);
    CSword(int _kind):CWeapon(_kind){}
};

class CBomb:public CWeapon
{
public:
    bool hit(CWarrior* a, CWarrior* b);
    CBomb(int _kind):CWeapon(_kind){}
};

class CArrow:public CWeapon
{
public:
    bool hit(CWarrior* a, CWarrior* b);
    CArrow(int _kind):CWeapon(_kind){}
};

CWeapon* CWeapon::getWeapon(int kind)
{
    switch (kind)
    {
    case sword:
        return new CSword(kind);
    case bomb:
        return new CBomb(kind);
    case arrow:
        return new CArrow(kind);
    default:
        cerr << "getweapon error !" << endl;
        return NULL;
    }
}

bool wolfCompare(CWeapon* a, CWeapon* b)// wolf:如果敌人arrow太多没法都抢来，那就先抢没用过的。
{
    if (a->kind != b->kind)
    {
        return a->kind < b->kind;
    }
    return a->used < b->used;
}

bool fightCompare(CWeapon* a, CWeapon* b)// 编号小的武器，排在前面。若有多支arrow，用过的排在前面。
{
    if (a->kind != b->kind)
    {
        return a->kind < b->kind;
    }
    return a->used > b->used;
}

class CWarrior
{
public:
    int kind;
    int side;
    int id;// 编号
    int elements;// 生命值
    int force;// 攻击力
    vector<CWeapon*> weapons;// 拥有的武器
    
    void who()// 自报家门 'red iceman 1'
    {printf("%s %s %d", sideName[side], warriorName[kind], id);}

    void with()//报告生命值和攻击力 'with 20 elements and force 30\n'
    {printf("with %d elements and force %d\n", elements, force);}

    void remain()// 报告剩余生命值 'remaining 20 elements\n'
    {printf("remaining %d elements\n",elements);}
    
    // 武士降生 '000:00 blue dragon 1 born'
    CWarrior(int _kind, int _side, int _id):kind(_kind),side(_side),id(_id),elements(warriorInitElements[_kind]),force(warriorInitForce[_kind])
    {when();who();printf(" born\n");}

    ~CWarrior()
    {
        for (auto i = weapons.begin(); i != weapons.end(); ++i)
        {
            if (*i != NULL)
            {
                delete *i;
            }
        }
    }

    static CWarrior* getWarrior(int _kind, int _side, int _id, int _loyalty);
    virtual bool ranAway()=0;
    virtual void march()=0;
    virtual void tookWeapon(CWarrior* w, int city)=0;// wolf抢夺武器
    // void seized();
    void report();
    void sortWeapon();
};

class CDragon:public CWarrior
{
public:
    CDragon(int _kind, int _side, int _id):CWarrior(_kind, _side, _id)
    {
        weapons.push_back(CWeapon::getWeapon(id%3));
    }
    bool ranAway(){return false;}
    void march(){}
    void tookWeapon(CWarrior* w, int city){}
};

class CNinja:public CWarrior
{
public:
    CNinja(int _kind, int _side, int _id):CWarrior(_kind, _side, _id)
    {
        weapons.push_back(CWeapon::getWeapon(id%3));
        weapons.push_back(CWeapon::getWeapon((id+1)%3));
    }
    bool ranAway(){return false;}
    void march(){}
    void tookWeapon(CWarrior* w, int city){}
};

class CIceman:public CWarrior
{
public:
    CIceman(int _kind, int _side, int _id):CWarrior(_kind, _side, _id)
    {
        weapons.push_back(CWeapon::getWeapon(id%3));
    }
    bool ranAway(){return false;}
    void march(){elements -= int(elements/10);}
    void tookWeapon(CWarrior* w, int city){}
};

class CLion:public CWarrior
{
public:
    int loyalty;

    // 如果造出的是lion，那么还要多输出一行 'Its loyalty is 24'
    CLion(int _kind, int _side, int _id, int _loyalty):CWarrior(_kind, _side, _id),loyalty(_loyalty)
    {
        printf("Its loyalty is %d\n", loyalty);
        // 编号为n的lion降生时即获得编号为n%3 的武器。
        weapons.push_back(CWeapon::getWeapon(id%3));
    }

    bool ranAway()// 如果要逃跑就返回true，并且输出逃跑信息
    {
        if (loyalty <= 0)// 忠诚度降至0或0以下，则该lion逃离战场,永远消失。
        {when();who();printf(" ran away\n");return true;}
        return false;
    }
    void march(){loyalty -= K;}
    void tookWeapon(CWarrior* w, int city){}
};

class CWolf:public CWarrior
{
public:
    CWolf(int _kind, int _side, int _id):CWarrior(_kind, _side, _id){}
    bool ranAway(){return false;}
    void march(){}
    void tookWeapon(CWarrior* w, int city)
    {
        if (kind!=wolf || w->kind==wolf)// 如果敌人也是wolf，则不抢武器。
        {
            return;
        }
        if (w->weapons.begin() == w->weapons.end())// 敌人没有武器
        {
            return;
        }
        sort(w->weapons.begin(), w->weapons.end(), wolfCompare);
        CWeapon* took = *(w->weapons.begin());
        int toTake = took->kind;
        int hadWeapon = weapons.size();
        int tookWeapon = 0;
        for (auto i = w->weapons.begin(); (i != w->weapons.end()) && (hadWeapon <= 10) && ((*i)->kind == toTake); ++hadWeapon, ++tookWeapon)
        {
            weapons.push_back(*i);
            w->weapons.erase(i);// 此时i自动指向下一个，无需++
        }
        if (tookWeapon>0)
        {
            when();
            who();
            printf(" took %d %s from ", tookWeapon, weaponName[toTake]);
            w->who();
            printf(" in city %d\n", city);
        }
    }
};

CWarrior* CWarrior::getWarrior(int _kind, int _side, int _id, int _loyalty)
{
    switch (_kind)
    {
    case dragon:
        return new CDragon(_kind, _side, _id);
        break;
    case ninja:
        return new CNinja(_kind, _side, _id);
        break;
    case iceman:
        return new CIceman(_kind, _side, _id);
        break;
    case lion:
        return new CLion(_kind, _side, _id, _loyalty);
        break;
    case wolf:
        return new CWolf(_kind, _side, _id);
        break;
    default:
        cerr << "new warrior error!" << endl;
        break;
    }
}

void CWarrior::report()
{
    int count[3] = {0,0,0};
    for (auto i = weapons.begin(); i != weapons.end(); ++i)
    {
        count[(*i)->kind] ++;
    }
    when();
    who();
    printf(" has %d sword %d bomb %d arrow and %d elements\n",count[sword],count[bomb],count[arrow],elements);
}

void CWarrior::sortWeapon()
{
    sort(weapons.begin(), weapons.end(), fightCompare);
}

bool CSword::hit(CWarrior* a, CWarrior* b)
{
    int force = a->force * 2 / 10;
    b->elements -= force;
    return false;
}

bool CBomb::hit(CWarrior* a, CWarrior* b)
{
    int force = a->force * 4 / 10;
    b->elements -= force;
    if (a->kind != ninja)
    {
        a->elements -= force/2;
    }
    return true;
}

bool CArrow::hit(CWarrior* a, CWarrior* b)
{
    int force = a->force * 3 / 10;
    b->elements -= force;
    if (used == 1)
    {
        return true;
    }
    else
    {
        used = 1;
        return false;
    }
}

class CHeadquarter
{
public:
    int side;
    int elements;
    int warriorNum;
    CHeadquarter(int _side):side(_side),elements(M),warriorNum(1){}
    void reset(){elements = M;warriorNum = 1;}
    void generateWarrior();
    void report(){when();printf("%d elements in %s headquarter\n", elements, sideName[side]);}
};

class CCity
{
public:
    int id;
    CWarrior* warriorInCity[2];
    bool killed[2];// 每次要记得抹去
    CCity(int _id):id(_id){warriorInCity[0] = NULL;warriorInCity[1] = NULL;killed[0]=false;killed[1]=false;}
    void reset(){warriorInCity[0] = NULL;warriorInCity[1] = NULL;}
    void lionRanAway();
    void declareMarch();
    void march(int side);
    void wolfTookWeapon();
    void report();
    bool nothingChange();
    void fight();
};

void CHeadquarter::generateWarrior()
{
    if (warriorNum == -1)
    {
        return;
    }
    int next = generateOrder[side][(mytime/60)%5];
    if (elements < warriorInitElements[next])
    {
        warriorNum = -1;
        return;
    }
    elements -= warriorInitElements[next];
    citys[side==red?0:N+1]->warriorInCity[side] = CWarrior::getWarrior(next, side, warriorNum, elements);
    warriorNum ++;
}

void CCity::lionRanAway()
{
    for (int i = 0; i < 2; i++)
    {
        if (warriorInCity[i] != NULL && warriorInCity[i]->kind == lion)
        {
            if(warriorInCity[i]->ranAway())
            {
                delete warriorInCity[i];
                warriorInCity[i] = NULL;
            }
        }
    }
}

void CCity::declareMarch()
{
    if (id == 0 || id == N + 1)// 司令部
    {
        int comeFrom = (id==0?1:N);
        int i = (id==0?blue:red);
        if (citys[comeFrom]->warriorInCity[i] != NULL)// 对面有warrior进来了
        {
            citys[comeFrom]->warriorInCity[i]->march();
            when();
            citys[comeFrom]->warriorInCity[i]->who();
            printf(" reached %s headquarter ", sideName[1-i]);
            citys[comeFrom]->warriorInCity[i]->with();
            when();
            printf("%s headquarter was taken\n", sideName[1-i]);
            finished = 1;
        }
        return;
    }
    for (int i = 0; i < 2; i++)
    {
        int comeFrom = id + (i==red?-1:1);
        if (citys[comeFrom]->warriorInCity[i] != NULL)
        {
            citys[comeFrom]->warriorInCity[i]->march();
            when();
            citys[comeFrom]->warriorInCity[i]->who();
            printf(" marched to city %d ",id);
            citys[comeFrom]->warriorInCity[i]->with();
        }
    }
}

void CCity::march(int side)
{
    int comeFrom = id + (side==red?-1:1);
    if (comeFrom < 0 || comeFrom > N + 1)
    {
        return;
    }
    if (citys[comeFrom]->warriorInCity[side] != NULL)
    {
        citys[id]->warriorInCity[side] = citys[comeFrom]->warriorInCity[side];
        citys[comeFrom]->warriorInCity[side] = NULL;
    }
}

void CCity::wolfTookWeapon()
{
    if (warriorInCity[0]&&warriorInCity[1])
    {
        for (int i = 0; i < 2; i++)
        {
            warriorInCity[i]->tookWeapon(warriorInCity[1-i], id);
        }
    }
}

void CCity::report()
{
    for (int i = 0; i < 2; i++)
    {
        if (warriorInCity[i] != NULL)
        {
            warriorInCity[i]->report();
        }
    }
}

bool CCity::nothingChange()
{
    for (int i = 0; i < 2; i++)
    {
        for (vector<CWeapon*>::iterator j = warriorInCity[i]->weapons.begin(); j != warriorInCity[i]->weapons.end(); ++j)
        {
            if ((*j)->kind!=sword)
            {
                return false;
            }
            if ((*j)->kind==sword)
            {
                if ((warriorInCity[i]->force)*2/10 > 0)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void CCity::fight()
{
    // 先把战斗记录归零
    killed[0]=false;
    killed[1]=false;
    // 武器排序
    if (warriorInCity[0]&&warriorInCity[1])// 有两个武士才能战斗
    {
        for (int i = 0; i < 2; i++)
        {
            warriorInCity[i]->sortWeapon();// 战斗开始前，双方先对自己的武器排好使用顺序
        }
        int attacker = id%2?red:blue;// 在奇数编号城市，红武士先发起攻击
        vector<CWeapon*>::iterator pRedWeapon = warriorInCity[red]->weapons.begin();
        vector<CWeapon*>::iterator pBlueWeapon = warriorInCity[blue]->weapons.begin();
        while (!(killed[0]||killed[1]))
        {
            vector<CWeapon*>::iterator & pweapon = (attacker==red?pRedWeapon:pBlueWeapon);
            // 如果没有武器就不能动手
            if (pweapon!=warriorInCity[attacker]->weapons.end())
            {
                bool lost = (*(pweapon))->hit(warriorInCity[attacker],warriorInCity[1-attacker]);
                if (lost)
                {
                    delete (*(pweapon));
                    warriorInCity[attacker]->weapons.erase(pweapon);
                }
                else
                {
                    // 用过了，下一件等待
                    ++pweapon;
                }
                // 一轮用完再来一轮
                if (pweapon == warriorInCity[attacker]->weapons.end())
                {
                    pweapon = warriorInCity[attacker]->weapons.begin();
                }
            }
            for (int i = 0; i < 2; i++)
            {
                if (warriorInCity[i]->elements<=0)
                {
                    killed[i] = true;
                }
            }
            if (killed[red]&&killed[blue])// 同归于尽
            {
                when();
                printf("both ");
                warriorInCity[red]->who();
                printf(" and ");
                warriorInCity[blue]->who();
                printf(" died in city %d\n",id);
                break;
            }
            for (int i = 0; i < 2; i++)
            {
                if (killed[i])
                {
                    when();
                    warriorInCity[1-i]->who();
                    printf(" killed ");
                    warriorInCity[i]->who();
                    printf(" in city %d ",id);
                    warriorInCity[1-i]->remain();
                    break;
                }
            }
            if (nothingChange()&&(!killed[red])&&(!killed[blue]))
            {
                when();
                printf("both ");
                warriorInCity[red]->who();
                printf(" and ");
                warriorInCity[blue]->who();
                printf(" were alive in city %d\n",id);
                break;
            }
            // 如果没有打死就交换角色
            attacker = 1 - attacker;
        }
    }
    // 缴获武器
    for (int i = 0; i < 2; i++)
    {
        if ((!killed[i])&&killed[1-i]&&warriorInCity[i]&&warriorInCity[1-i])
        {
            CWarrior* winner = warriorInCity[i];
            CWarrior* loser = warriorInCity[1-i];
            sort(loser->weapons.begin(), loser->weapons.end(), wolfCompare);
            int hadWeapon = winner->weapons.size();
            for (auto i = loser->weapons.begin(); (i != loser->weapons.end()) && (hadWeapon <= 10); ++hadWeapon)
            {
                winner->weapons.push_back(*i);
                loser->weapons.erase(i);// 此时i自动指向下一个，无需++
            }
        }
    }
    // 尸体清除
    for (int i = 0; i < 2; i++)
    {
        if (killed[i])
        {
            delete warriorInCity[i];
            warriorInCity[i] = NULL;
        }
    }
    // 武士欢呼
    for (int i = 0; i < 2; i++)
    {
        if ((killed[i]==false)&&warriorInCity[i]&&(warriorInCity[i]->kind==dragon))
        {
            if (killed[1-i]||warriorInCity[1-i])
            {
                when();
                warriorInCity[i]->who();
                printf(" yelled in city %d\n",id);
            }
        }
    }
}

void init()// 全局变量初始化
{
    // 初始化headquarters状态
    for (int i = 0; i < 2; i++)
    {
        headquarters[i] = new CHeadquarter(i);
    }
    
    // 初始化citys的状态
    for (int i = 0; i <= 20 + 1; i++)
    {
        citys[i] = new CCity(i);
    }
}

void reset()// 全局重置
{
    mytime = 0;
    finished = 0;

    scanf("%d %d %d %d",&M, &N, &K, &T);
    for (int i = 0; i < 5; i++)
    {
        scanf("%d",warriorInitElements + i);// 初始生命值
    }
    for (int i = 0; i < 5; i++)
    {
        scanf("%d",warriorInitForce + i);// 攻击力
    }

    // 重置headquarters状态
    for (int i = 0; i < 2; i++)
    {
        headquarters[i]->reset();
    }
    
    // 重置citys的状态
    for (int i = 0; i <= N + 1; i++)
    {
        citys[i]->reset();
    }
}

int main()
{
    // debug
    // freopen("datapub.in","r",stdin);
    // freopen("test0621.out","w",stdout);

    int t;// 测试数据组数
    init();
    scanf("%d",&t);
    for (int count = 0; count < t; count++)
    {
        reset();
        printf("Case %d:\n",count + 1);// Case n:
        while (mytime <= T)
        {
            // 在每个整点，即每个小时的第0分， 双方的司令部中各有一个武士降生。
            for (int i = 0; i < 2; i++)
            {
                headquarters[i]->generateWarrior();
            }
            mytime += 5;
            if (mytime > T){break;}
            // 在每个小时的第5分，该逃跑的lion就在这一时刻逃跑了。
            for (int i = 0; i <= N+1; i++)
            {
                citys[i]->lionRanAway();
            }
            mytime += 5;
            if (mytime > T){break;}
            // 在每个小时的第10分：所有的武士朝敌人司令部方向前进一步。即从己方司令部走到相邻城市，或从一个城市走到下一个城市。或从和敌军司令部相邻的城市到达敌军司令部。
            // 先输出march，全部输出完再逆向执行march
            for (int i = 0; i <= N+1; i++)
            {
                citys[i]->declareMarch();
            }
            for (int i = N + 1; i > 0; i--)// 先动红，从右到左逆向归纳
            {
                citys[i]->march(red);
            }
            for (int i = 0; i < N + 1; i++)// 再动蓝，从左到右逆向归纳
            {
                citys[i]->march(blue);
            }
            mytime += 25;
            if (mytime > T){break;}
            if (finished){break;}
            // 在有wolf及其敌人的城市，wolf要抢夺对方的武器。
            for (int i = 1; i < N + 1; i++)
            {
                citys[i]->wolfTookWeapon();
            }
            mytime += 5;
            if (mytime > T){break;}
            // 在有两个武士的城市，会发生战斗。武士欢呼
            for (int i = 1; i < N+1; i++)
            {
                citys[i]->fight();
            }
            mytime += 10;
            if (mytime > T){break;}
            // 司令部报告它拥有的生命元数量。
            for (int i = 0; i < 2; i++)
            {
                headquarters[i]->report();
            }
            mytime += 5;
            if (mytime > T){break;}
            // 每个武士报告其拥有的武器情况。
            for (int i = 0; i <= N+1; i++)
            {
                citys[i]->report();
            }
            mytime += 5;
            if (mytime > T){break;}
        }
    }
    return 0;
}

// testing data 
// echo 1 20 1 10 400 20 20 30 10 20 5 5 5 5 5 |./a.out