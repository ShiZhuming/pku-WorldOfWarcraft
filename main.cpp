#include<cstdio>
#include<cmath>
#include<cstring>
#include<memory.h>
using namespace std;

#define ERR 0.0001

enum _warriorKind{dragon, ninja, iceman, lion, wolf};

enum _weaponName{sword, bomb, arrow};

enum _sideKind{red, blue, null};

enum _weaponInfo{weaponNumber, weaponUsedTime, weaponForce};

const char sideName[2][5] = {"red","blue"};
const char warriorName[5][7] = {"dragon","ninja","iceman","lion","wolf"};
const int generateOrder[2][5] = {2,3,4,1,0,3,0,1,2,4};// 武士生产方式，先红后蓝
int warriorInitElements[5];
int warriorInitForce[5];

int M, N, R, K, T, mytime = 0, finished = 0;

inline void when()
{
    printf("%03d:%02d ", mytime/60, mytime%60);
}

class CWarrior;

class CHeadquarter
{
public:
    int side;
    int elements;
    int warriorGenerated;// 已经制造的武士数量，从1开始编号，开始的时候是0个
    CWarrior * warriorInHeadquarter;
    void generateWarrior();
    CHeadquarter(int _side):side(_side), elements(M), warriorGenerated(0){
        warriorInHeadquarter = NULL;
    }
};

CHeadquarter* headquarters[2];

class CWarrior
{
public:
    int id;
    int kind;
    int elements;
    int force;
    int loyalty;// lion
    double morale;// dragon
    int elementsBeforeFight;// for lion
    int steps;// for iceman
    int weapons[3][3];// 第一个维度为武器种类，第二个维度为武器信息weaponNumber, weaponUsedTime, weaponForce，按enum部分定义
    CWarrior(int _id, int _kind):id(_id), kind(_kind), loyalty(0), morale(0), elements(warriorInitElements[_kind]), force(warriorInitForce[_kind]), elementsBeforeFight(elements), steps(0){
        memset(weapons, 0, 9*sizeof(int));
        switch (kind)
        {
        case dragon:
        case iceman:
            getWeapon(id%3);
            break;
        case ninja:
            getWeapon(id%3);
            getWeapon((id+1)%3);
        default:
            break;
        }
    }
    void getWeapon(int weaponKind){
        weapons[weaponKind][weaponNumber] += 1;
        switch (weaponKind)
        {
        case sword:
            if (int(force * 0.2) > 0)
            {
                weapons[sword][weaponForce] = force * 0.2;
            }
            else
            {
                weapons[sword][weaponNumber] -= 1;// 如果武士降生时得到了一个初始攻击力为0的sword，则视为武士没有sword.
            }
            break;
        case arrow:
            weapons[arrow][weaponForce] = R;
        case bomb:
        default:
            break;
        }
    }
};

class CCity
{
public:
    int id;
    int elements;
    int flag;
    int lastWin;
    int matchHappened[2];// 记录本轮是否发生武士前进到本城市，用后清零！
    int killHappened[2];// 记录本轮是否有武士被杀死，不含bomb炸死的，每轮要收尸并且清零！
    CWarrior* warriorInCity[2];
    CCity():elements(0), flag(null), lastWin(null){
        warriorInCity[0] = warriorInCity[1] = NULL;
        matchHappened[0] = matchHappened[1] = 0;
        killHappened[0] = killHappened[1] = 0;
    }
    void lionRunAway();
    void matchPrint();// 同时调整iceman
    void addElements(){elements += 10;}
    void shotArrows();
    void clearBody();// 收尸，但死过人依旧记录好，等待下一大轮清零
    void useBomb();
    void fight();// 6-8武士战斗
    void dragonYelled();// 9武士欢呼
    void getElements();// 10武士获取生命元
    void flagRaise();// 11旗帜升起
    void lionRememberElements();
    void awardElements(int side);
    void reportWeapon(int side);
};
CCity citys[25];

void CCity::reportWeapon(int side)
{
    if (warriorInCity[side] != NULL && killHappened[side] == 0)
    {
        when();
        printf("%s %s %d has ",sideName[side], warriorName[warriorInCity[side]->kind], warriorInCity[side]->id);
        // int ** w = warriorInCity[side]->weapons;
        bool flag = false;
        if (warriorInCity[side]->weapons[arrow][weaponNumber] > 0)
        {
            printf("arrow(%d)",3 - warriorInCity[side]->weapons[arrow][weaponUsedTime]);
            flag = true;
        }
        if (warriorInCity[side]->weapons[bomb][weaponNumber] > 0)
        {
            if (flag)
            {
                printf(",");
            }
            printf("bomb");
            flag = true;
        }
        if (warriorInCity[side]->weapons[sword][weaponNumber] > 0)
        {
            if (flag)
            {
                printf(",");
            }
            printf("sword(%d)",warriorInCity[side]->weapons[sword][weaponForce]);
            flag = true;
        }
        if (!flag)
        {
            printf("no weapon");
        }
        printf("\n");
    }
    
}

void CCity::flagRaise()
{
    int thisTime;
    if (warriorInCity[red] == NULL || warriorInCity[blue] == NULL)// 没有发生战斗
    {
        return ;
    }
    if (killHappened[red] == 0 && killHappened[blue] == 0)// 平局
    {
        thisTime = null;
        lastWin = null;
        return;
    }
    else if (killHappened[red] == 0 && killHappened[blue] == 1)
    {
        thisTime = red;
    }
    else
    {
        thisTime = blue;
    }
    if (thisTime == lastWin && flag != thisTime)// 没有旗帜或者升着败方的旗帜
    {
        flag = thisTime;
        when();
        printf("%s flag raised in city %d\n",sideName[thisTime], id);
    }
    else
    {
        lastWin = thisTime;
    }
}

void CCity::awardElements(int side)
{
    if (warriorInCity[red] != NULL && warriorInCity[blue] != NULL && killHappened[side] == 0 && killHappened[1 - side] == 1)
    {
        if (headquarters[side]->elements >= 8)
        {
            warriorInCity[side]->elements += 8;
            headquarters[side]->elements -= 8;
        }
    }
}

void CCity::getElements()
{
    for (int i = 0; i < 2; i++)
    {
        if (warriorInCity[red] != NULL && warriorInCity[blue] != NULL && killHappened[i] == 0 && killHappened[1 - i] == 1)
        {
            when();
            printf("%s %s %d earned %d elements for his headquarter\n",sideName[i], warriorName[warriorInCity[i]->kind], warriorInCity[i]->id, elements);
            headquarters[i]->elements += elements;
            elements = 0;
        }
    }
}

void CCity::lionRememberElements()
{
    for (int i = 0; i < 2; i++)
    {
        if (warriorInCity[i] != 0 && warriorInCity[i]->kind == lion)
        {
            warriorInCity[i]->elementsBeforeFight = warriorInCity[i]->elements;
        }
    }
}

void CCity::dragonYelled()
{
    for (int i = 0; i < 2; i++)
    {
        if (warriorInCity[i] != NULL && warriorInCity[1 - 1] != NULL)// 有两个武士来过，视为发生过战斗
        {
            switch (warriorInCity[i]->kind)
            {
            case dragon:
                if (killHappened[i] == 0 && killHappened[1 - i] == 1)// 对方死了
                {
                    warriorInCity[i]->morale += 0.2;
                }
                else if (killHappened[i] == 0 && killHappened[1 - i] == 0)// 都活着，没有取得胜利
                {
                    warriorInCity[i]->morale -= 0.2;
                }
                if (killHappened[i] == 0 && warriorInCity[i]->morale > 0.8 - ERR)// 如果还没有战死，而且士气值大于0.8，就会欢呼。
                {
                    if (flag == i || (flag == null && ((id%2==1)?(i==red):(i==blue))))// 在一次在它主动进攻的战斗结束后，
                    {
                        when();
                        printf("%s %s %d yelled in city %d\n", sideName[i], warriorName[warriorInCity[i]->kind], warriorInCity[i]->id, id);
                    }
                }
                break;
            case lion:
                if (killHappened[1-i] == 0)// 每经过一场未能杀死敌人的战斗，忠诚度就降低K。
                {
                    warriorInCity[i]->loyalty -= K;
                }
                if (killHappened[i] == 1)// lion 若是战死，则其战斗前的生命值就会转移到对手身上。
                {
                    warriorInCity[1 - i]->elements += warriorInCity[i]->elementsBeforeFight;
                }
                break;
            case wolf:
                if (killHappened[i] == 0 && killHappened[1 - i] == 1)// 对方死了
                {
                    for (int j = 0; j < 3; j++)
                    {
                        if (warriorInCity[i]->weapons[j][weaponNumber] == 0)// 自己已有的武器就不缴获了
                        {
                            if (warriorInCity[1-i]->weapons[j][weaponNumber] != 0)
                            {
                                for (int k = 0; k < 3; k++)// 缴获敌人的武器
                                {
                                    warriorInCity[i]->weapons[j][k] = warriorInCity[1-i]->weapons[j][k];
                                }
                            }
                        }
                    }
                }
            default:
                break;
            }
        }
    }
}

void CCity::fight()
{
    if (warriorInCity[red] == NULL || warriorInCity[blue] == NULL || killHappened[red] == 1 || killHappened[blue] == 1)// 只有一个warrior
    {
        return ;
    }
    CWarrior * currentWarrior;
    CWarrior * oppositeWarrior;
    int oppositeSide;
    if (flag == red || (flag == null && id % 2 == 1))//在插红旗的城市，以及编号为奇数的无旗城市，由红武士主动发起进攻。
    {
        currentWarrior = warriorInCity[red];
        oppositeWarrior = warriorInCity[blue];
        oppositeSide = blue;
    }
    else// 在插蓝旗的城市，以及编号为偶数的无旗城市，由蓝武士主动发起进攻。
    {
        currentWarrior = warriorInCity[blue];
        oppositeWarrior = warriorInCity[red];
        oppositeSide = red;
    }
    when();
    printf("%s %s %d attacked %s %s %d in city %d with %d elements and force %d\n", sideName[1-oppositeSide], warriorName[currentWarrior->kind], currentWarrior->id, sideName[oppositeSide], warriorName[oppositeWarrior->kind], oppositeWarrior->id, id, currentWarrior->elements, currentWarrior->force);
    oppositeWarrior->elements -= currentWarrior->force;
    if (currentWarrior->weapons[sword][weaponNumber] > 0)// 有sword
    {
        oppositeWarrior->elements -= currentWarrior->weapons[sword][weaponForce];
        currentWarrior->weapons[sword][weaponForce] *= 0.8;// sword每经过一次战斗(不论是主动攻击还是反击)，就会变钝，攻击力变为本次战斗前的80% (去尾取整)
        if (currentWarrior->weapons[sword][weaponForce] <= 0)
        {
            currentWarrior->weapons[sword][weaponForce] = 0;
            currentWarrior->weapons[sword][weaponNumber] = 0;
        }
    }
    if (oppositeWarrior->elements <= 0)// 对面warrior死亡
    {
        when();
        printf("%s %s %d was killed in city %d\n", sideName[oppositeSide], warriorName[oppositeWarrior->kind], oppositeWarrior->id, id);
        killHappened[oppositeSide] = 1;
        // delete oppositeWarrior;
        // warriorInCity[oppositeSide] = NULL;
        return ;
    }
    
    if (oppositeWarrior->kind == ninja)// ninja 挨打了也从不反击敌人。
    {
        return ;
    }
    when();
    printf("%s %s %d fought back against %s %s %d in city %d\n", sideName[oppositeSide], warriorName[oppositeWarrior->kind], oppositeWarrior->id, sideName[1-oppositeSide], warriorName[currentWarrior->kind], currentWarrior->id, id);
    currentWarrior->elements -= 0.5*oppositeWarrior->force;// 开始反击
    if (oppositeWarrior->weapons[sword][weaponNumber] > 0)// 有sword
    {
        currentWarrior->elements -= oppositeWarrior->weapons[sword][weaponForce];
        oppositeWarrior->weapons[sword][weaponForce] *= 0.8;// sword每经过一次战斗(不论是主动攻击还是反击)，就会变钝，攻击力变为本次战斗前的80% (去尾取整)
        if (oppositeWarrior->weapons[sword][weaponForce] <= 0)
        {
            oppositeWarrior->weapons[sword][weaponForce] = 0;
            oppositeWarrior->weapons[sword][weaponNumber] = 0;
        }
    }
    if (currentWarrior->elements <= 0)// warrior死亡
    {
        when();
        printf("%s %s %d was killed in city %d\n", sideName[1 - oppositeSide], warriorName[currentWarrior->kind], currentWarrior->id, id);
        killHappened[1 - oppositeSide] = 1;
        // delete currentWarrior;
        // warriorInCity[1 - oppositeSide] = NULL;
        return ;
    }
}

void CCity::useBomb()
{
    // 判断战斗结果
    if (warriorInCity[red] == NULL || warriorInCity[blue] == NULL)// 只有一个warrior
    {
        return ;
    }
    CWarrior * currentWarrior;
    CWarrior * oppositeWarrior;
    int oppositeSide;
    if (flag == red || (flag == null && id % 2 == 1))//在插红旗的城市，以及编号为奇数的无旗城市，由红武士主动发起进攻。
    {
        currentWarrior = warriorInCity[red];
        oppositeWarrior = warriorInCity[blue];
        oppositeSide = blue;
    }
    else// 在插蓝旗的城市，以及编号为偶数的无旗城市，由蓝武士主动发起进攻。
    {
        currentWarrior = warriorInCity[blue];
        oppositeWarrior = warriorInCity[red];
        oppositeSide = red;
    }
    if(/*currentWarrior->weapons[sword][weaponNumber] > 0 && */oppositeWarrior->elements - currentWarrior->force - currentWarrior->weapons[sword][weaponForce] <= 0)// 对方死亡
    {
        if (oppositeWarrior->weapons[bomb][weaponNumber] > 0)// 拥有bomb的武士，在战斗开始前如果判断自己将被杀死
        {
            when();
            printf("%s %s %d used a bomb and killed %s %s %d\n", sideName[oppositeSide], warriorName[oppositeWarrior->kind], oppositeWarrior->id, sideName[1 - oppositeSide], warriorName[currentWarrior->kind], currentWarrior->id);
            delete oppositeWarrior;
            delete currentWarrior;
            warriorInCity[0] = warriorInCity[1] = NULL;
            return ;
            // 使用bomb和敌人同归于尽
        }
    }
    // 对方幸存并反击，ninja 挨打了也从不反击敌人。
    if(oppositeWarrior->kind != ninja &&/* oppositeWarrior->weapons[sword][weaponNumber] > 0 &&*/ currentWarrior->elements - oppositeWarrior->force - oppositeWarrior->weapons[sword][weaponForce] <= 0)// 对方死亡
    {
        if (currentWarrior->weapons[bomb][weaponNumber] > 0)// 拥有bomb的武士，在战斗开始前如果判断自己将被杀死
        {
            when();
            printf("%s %s %d used a bomb and killed %s %s %d\n", sideName[1 - oppositeSide], warriorName[currentWarrior->kind], currentWarrior->id, sideName[oppositeSide], warriorName[oppositeWarrior->kind], oppositeWarrior->id);
            delete oppositeWarrior;
            delete currentWarrior;
            warriorInCity[0] = warriorInCity[1] = NULL;
            return ;
            // 使用bomb和敌人同归于尽
        }
    }
}

void CCity::clearBody()
{
    for (int i = 0; i < 2; i++)// i = side
    {
        if (killHappened[i] && warriorInCity[i] != NULL)
        {
            delete warriorInCity[i];
            warriorInCity[i] = NULL;
            killHappened[i] = 0;
        }
    }
}

void CCity::shotArrows()
{
    for (int i = 0; i < 2; i++)// 武士的side
    {
        // 不能攻击对方司令部里的敌人
        if (id == 1 && i == blue)// 在红方司令部前的蓝方
        {
            continue;
        }
        if (id == N && i == red)// 在蓝方司令部前的红方
        {
            continue;
        }
        if (warriorInCity[i] != NULL && citys[i==red?id+1:id-1].warriorInCity[1-i] != NULL)
        {
            if (warriorInCity[i]->weapons[arrow][weaponNumber] == 0)
            {
                continue;
            }
            CWarrior * currentWarrior = warriorInCity[i];
            CWarrior * oppositeWarrior = citys[i==red?id+1:id-1].warriorInCity[1-i];
            currentWarrior->weapons[arrow][weaponUsedTime] ++;
            oppositeWarrior->elements -= R;// arrow使敌人的生命值减少R
            when();
            printf("%s %s %d shot", sideName[i], warriorName[currentWarrior->kind], currentWarrior->id);
            if (oppositeWarrior->elements <= 0)// 若减至小于等于0，则敌人被杀死
            {
                citys[i==red?id+1:id-1].killHappened[1-i] = 1;// 两个相邻的武士可能同时放箭把对方射死。所以结束后收尸
                printf(" and killed %s %s %d\n", sideName[1-i], warriorName[oppositeWarrior->kind], oppositeWarrior->id);
            }
            else
            {
                printf("\n");
            }
            if (currentWarrior->weapons[arrow][weaponUsedTime] == 3)// arrow使用3次后即被耗尽，武士失去arrow。
            {
                warriorInCity[i]->weapons[arrow][weaponNumber] = 0;
                warriorInCity[i]->weapons[arrow][weaponUsedTime] = 0;
            }
        }
    }
}

void CCity::lionRunAway()
{
    for (int i = 0; i < 2; i++)
    {
        if (id == 0 && i == 1)// 在红方司令部的蓝方lion
        {
            continue;
        }
        if (id == N + 1 && i == 0)// 在蓝方司令部的红方lion
        {
            continue;
        }
        if (warriorInCity[i] != NULL)
        {
            CWarrior * currentWarrior = warriorInCity[i];
            if (currentWarrior->kind == lion && currentWarrior->loyalty == 0)
            {
                when();
                printf("%s lion %d ran away\n", sideName[i], currentWarrior->id);
                delete currentWarrior;
                warriorInCity[i] = NULL;
            }
        }
    }
}

void CCity::matchPrint()
{
    for (int i = 0; i < 2; i++)
    {
        if (matchHappened[i] == 1)
        {
            warriorInCity[i]->steps ++;
            if (warriorInCity[i]->kind == iceman && warriorInCity[i]->steps == 2)// iceman 每前进两步
            {
                if (warriorInCity[i]->elements > 9)
                {
                    warriorInCity[i]->elements -= 9;// 在第2步完成的时候，生命值会减少9
                }
                else
                {
                    warriorInCity[i]->elements = 1;// 但是若生命值减9后会小于等于0，则生命值不减9,而是变为1
                }
                warriorInCity[i]->force += 20;// 攻击力会增加20。
                warriorInCity[i]->steps = 0;
            }
            if (id != 0 && id != N + 1)// 输出不在司令部发生的
            {
                when();
                printf("%s %s %d marched to city %d with %d elements and force %d\n", sideName[i], warriorName[warriorInCity[i]->kind], warriorInCity[i]->id, id, warriorInCity[i]->elements, warriorInCity[i]->force);
            }
            else
            {
                when();
                printf("%s %s %d reached %s headquarter with %d elements and force %d\n", sideName[i], warriorName[warriorInCity[i]->kind], warriorInCity[i]->id, sideName[id==0?red:blue], warriorInCity[i]->elements, warriorInCity[i]->force);
                if (headquarters[id==0?red:blue]->warriorInHeadquarter != NULL)
                {
                    when();
                    printf("%s headquarter was taken\n",sideName[id==0?red:blue]);
                    finished = 1;
                }
                
            }
        }
    }
}

void CHeadquarter::generateWarrior()
{
    int nextGenerate = generateOrder[side][warriorGenerated % 5];// 要生产的种类
    if (elements >= warriorInitElements[nextGenerate])// 可以生产下一个
    {
        warriorGenerated ++;//现在多了1个warrior
        elements -= warriorInitElements[nextGenerate];// 消耗生命元
        CWarrior * currentWarrior = new CWarrior(warriorGenerated, nextGenerate);
        citys[(side?N+1:0)].warriorInCity[side] = currentWarrior;// new一个武士放在司令部所在的城市
        when();
        printf("%s %s %d born\n",sideName[side], warriorName[nextGenerate], warriorGenerated);
        switch (nextGenerate)
        {
        case dragon:// dragon还有“士气”这个属性，是个浮点数，其值为它降生后其司令部剩余生命元的数量除以造dragon所需的生命元数量。
            currentWarrior->morale = (double)elements/warriorInitElements[0];
            printf("Its morale is %.2f\n", currentWarrior->morale);
            break;
        case lion:// lion 有“忠诚度”这个属性，其初始值等于它降生之后其司令部剩余生命元的数目。
            currentWarrior->loyalty = elements;
            printf("Its loyalty is %d\n", currentWarrior->loyalty);
            break;
        default:
            break;
        }
    }
    // 否则等着
}

int main()
{
    int t;
    scanf("%d",&t);
    for (int count = 0; count < t; count++)
    {
        printf("Case %d:\n",count);
        
        scanf("%d %d %d %d %d",&M, &N, &R, &K, &T);
        for (int i = 0; i < 5; i++)
        {
            scanf("%d",warriorInitElements + i);
        }
        for (int i = 0; i < 5; i++)
        {
            scanf("%d",warriorInitForce + i);
        }
        for (int i = 0; i < 2; i++)
        {
            headquarters[i] = new CHeadquarter(i);
        }
        for (int i = 0; i < N + 2; i++)
        {
            citys[i].id = i;
        }
        finished = 0;
        while (finished == 0)
        {
            // 武士降生
            for (int i = 0; i < 2; i++)
            {
                headquarters[i]->generateWarrior();
            }
            mytime += 5;
            // lion逃跑
            for (int i = 0; i < N + 2; i++)
            {
                citys[i].lionRunAway();
            }
            mytime += 5;

            // 武士前进到某一城市
            headquarters[red]->warriorInHeadquarter = citys[0].warriorInCity[blue];
            headquarters[blue]->warriorInHeadquarter = citys[N + 1].warriorInCity[red];
            for (int i = N + 1; i > 0; i--)// 红方，从N+1号城市开始考虑上一城市武士是否前进
            {
                citys[i].matchHappened[red] = 0;// 清零记录区
                citys[i].warriorInCity[red] = NULL;// 本城市的武士要么没有要么走掉了
                if (citys[i - 1].warriorInCity[red] != NULL)// 上一个城市有红武士
                {
                    citys[i].matchHappened[red] = 1;// 发生了红武士转移到本城市
                    citys[i].warriorInCity[red] = citys[i - 1].warriorInCity[0];// 把上一个城市的武士移过来
                    citys[i - 1].warriorInCity[red] = NULL;// 上一个城市的红武士没了
                }
            }
            for (int i = 0; i < N + 1; i++)// 蓝方，从0号城市红方司令部开始考虑下一城市武士是否前进
            {
                citys[i].matchHappened[1] = 0;// 清零记录区
                citys[i].warriorInCity[1] = NULL;// 本城市的武士要么没有要么走掉了。
                if (citys[i + 1].warriorInCity[1] != NULL)// 下一个城市有蓝武士
                {
                    citys[i].matchHappened[1] = 1;// 发生了蓝武士转移到本城市
                    citys[i].warriorInCity[1] = citys[i + 1].warriorInCity[1];// 把下一个城市的武士移过来
                    citys[i + 1].warriorInCity[1] = NULL;// 下一个城市的红武士没了
                }
            }
            for (int i = 0; i < N + 2; i++)// 如果抵达司令部，特判
            {
                citys[i].matchPrint();
            }
            
            
            // if (citys[0].warriorInCity[blue] != NULL && headquarters[])
            // {
                
            // }
            
            // 武士抵达敌军司令部

            // 司令部被占领
            if (finished == 1)
            {
                break;
            }
            
            mytime += 10;
            // 每个城市产出10个生命元。
            for (int i = 1; i < N + 1; i++)
            {
                citys[i].addElements();
            }
            mytime += 10;
            // 只有1个武士的话，取走生命元，给司令部
            for (int i = 1; i < N + 1; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    if (citys[i].warriorInCity[j] != NULL && citys[i].warriorInCity[1-j] == NULL)
                    {
                        when();
                        printf("%s %s %d earned %d elements for his headquarter\n",sideName[j], warriorName[citys[i].warriorInCity[j]->kind],citys[i].warriorInCity[j]->id,citys[i].elements);
                        headquarters[j]->elements += citys[i].elements;
                        citys[i].elements = 0;
                    }
                }
            }
            mytime += 5;
            // 武士放箭
            for (int i = 1; i < N + 1; i++)
            {
                citys[i].lionRememberElements();
            }
            
            for (int i = 1; i < N + 1; i++)// 不能攻击对方司令部里的敌人
            {
                citys[i].shotArrows();
            }
            // for (int i = 1; i < N + 1; i++)// 收尸
            // {
            //     citys[i].clearBody();
            // }
            mytime += 3;
            // 武士使用bomb
            for (int i = 1; i < N + 1; i++)// 拥有bomb的武士评估是否应该使用bomb。
            {
                citys[i].useBomb();
            }
            mytime += 2;
            // 同一时间发生的事件，按发生地点从西向东依次输出. 武士前进的事件, 算是发生在目的地。
            // 在一次战斗中有可能发生上面的 6 至 11 号事件。这些事件都算同时发生，其时间就是战斗开始时间。一次战斗中的这些事件，序号小的应该先输出。
            for (int i = 1; i < N + 1; i++)// 拥有bomb的武士评估是否应该使用bomb。
            {
                citys[i].fight();// 6-8武士战斗
                citys[i].dragonYelled();// 9武士欢呼
            }
            for (int i = N; i > 0; i--)
            {
                citys[i].awardElements(red);
            }
            for (int i = 1; i < N + 1; i++)
            {
                citys[i].awardElements(blue);
            }
            for (int i = 1; i < N + 1; i++)
            {
                citys[i].getElements();// 10武士获取生命元
                citys[i].flagRaise();// 11旗帜升起
            }
            mytime += 10;
            // 司令部报告生命元数量
            for (int i = 0; i < 2; i++)
            {
                when();
                printf("%d elements in %s headquarter\n",headquarters[i]->elements, sideName[i]);
            }
            mytime += 5;
            // 武士报告武器情况
            for (int i = 0; i < 2; i++)// side
            {
                for (int j = 0; j < N + 2; j++)// city
                {
                    citys[j].reportWeapon(i);
                }
            }
            mytime += 5;
            // 收尾
            for (int i = 0; i < N + 2; i++)
            {
                citys[i].clearBody();
            }
        }
        for (int i = 0; i < 2; i++)
        {
            delete headquarters[i];
        }
        
    }
    
    return 0;
}

// testing data 
// echo 1 20 1 10 10 1000 20 20 30 10 20 5 5 5 5 5 |./a.out