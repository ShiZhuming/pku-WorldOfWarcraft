#include<iostream>
#include<cmath>
#include<cstdio>
#include<stdio.h>
#include<string.h>
#include<string>
#include<algorithm>
#include<stdlib.h>
#include<cstdlib>
#include<iomanip>
#include<sstream>
#include<memory.h>
#include<fstream>
using namespace std;

class MAN {
public:
	string name;
	int life;
};

MAN maninf[5];
string nameofman[5] = { "dragon","ninja","iceman","lion","wolf" };
string color[2] = { "red","blue" };
int order[2][5] = { 2,3,4,1,0,3,0,1,2,4 };
string weapon_name[3]={"sword","bomb","arrow"};

int main()
{
	// freopen("test.txt","w",stdout);
	int turn;
	cin >> turn;

// cout<<"test"<<endl;

	for (int TURN = 0; TURN < turn; TURN++) {
		// cout<<"test head"<<endl;
		int leftlife[2];
		cin >> leftlife[1];leftlife[0] = leftlife[1];
		int time = 0;
		int minn = 10001;

// cout<<"test head"<<endl;

		for (int i = 0; i < 5; i++) {
			maninf[i].name = nameofman[i];
			cin >> maninf[i].life;
			minn = maninf[i].life <= minn ? maninf[i].life : minn;
		}

		// cout<<"test middle"<<endl;

		int id[2] = { 0,0 };
		int nextcreat[2] = { 0,0 };
		int totalnumber[2][5] = { 0,0,0,0,0,0,0,0,0,0 };

		bool flag[2] = { false,false };


		// cout<<"test only"<<endl;

		cout << "Case:" << TURN+1 << endl;

		while (!flag[0]||!flag[1]) {
			for (int i = 0; i < 2; i++) {
				if (leftlife[i] < minn) {
					if (flag[i])continue;
					flag[i] = true;

					/*003 red headquarter stops making warriors*/

					cout<< setw(3) << setfill('0') << time;
					cout << " " << color[i];
					cout << " headquarter stops making warriors" << endl;
					continue;
				}
				while (1) {
					if (nextcreat[i] == 5)nextcreat[i] = 0;
					if (maninf[order[i][nextcreat[i]]].life <= leftlife[i]) {
						leftlife[i] -= maninf[order[i][nextcreat[i]]].life;
						totalnumber[i][order[i][nextcreat[i]]]++;
						id[i]++;
						/*001 red lion 2 born with strength 6,1 lion in red headquarter*/
						cout << setw(3) << setfill('0') << time;
						cout << " " << color[i];
						cout << " " << maninf[order[i][nextcreat[i]]].name;
						cout << " " << id[i];
						cout << " born with strength ";
						cout << maninf[order[i][nextcreat[i]]].life;
						cout << ",";
						cout << totalnumber[i][order[i][nextcreat[i]]];
						cout << " " << maninf[order[i][nextcreat[i]]].name;
						cout << " in ";
						cout << color[i];
						cout << " headquarter" << endl;

						switch(order[i][nextcreat[i]]){
							case 0:cout<<"It has a "<<weapon_name[id[i]%3]<<
							",and it's morale is ";
							cout<<fixed<<setprecision(2)<<(double)leftlife[i]/(double)maninf[order[i][nextcreat[i]]].life<<endl;
							break;
							case 1:cout<<"It has a "<<weapon_name[id[i]%3]<<" and a "<<weapon_name[(id[i]+1)%3]<<endl;
							break;
							case 2:cout<<"It has a "<<weapon_name[id[i]%3]<<endl;
							break;
							case 3:cout<<"It's loyalty is "<<leftlife[i]<<endl;
							break;
							case 4:break;
							default:cout<<"error"<<endl;
						}


						nextcreat[i]++;
						break;
					}
					nextcreat[i]++;
				}
				//cout << "reach here i = " << i << endl;
			}
			time++;
		}
		// cout<<"test only"<<endl;
		//cout << "reach here TRUN = " << TURN << endl;
	}
	return 0;
}