#include<iostream>
#include<fstream>
using namespace std;

//���̤j��Q�B���̤j���l�B�֭p��Q

struct TX
{
	int year,month,day,exdate;
	int price[285][3];
};

int strategy1(TX *ptx)    //�����@ 
{
	int r=(ptx->price[284][2]-ptx->price[0][2])*200;    //�@�I���⦨�x���� 200 ��
	cout<<r<<endl;
	return r;    // r�O��Q 
}

int strategy2(TX *ptx,double stoploss)    //�����G
{
	int cost=ptx->price[0][2];    //���� 
	int slp=(int)(cost*(1-stoploss)+0.5);    //���l�I   +0.5���|�ˤ��J 
	int final_price;
	for(int n=1;n<285;n++)
	{
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=slp)
		{
			cout<<"���l"<<endl;
			break;    //�p�G�O�̫�@���Ʀr�Afinal_price�N���|��s�F 
		}
	}
	return (final_price-cost)*200;    //��Q 
}

int strategy3(TX*ptx,double stoploss,double takeprofit)    //�����T 
{
	int cost=ptx->price[0][2];    //���� 
	int slp=(int)(cost*(1-stoploss)+0.5);    //���l�I
	int tpp=(int)(cost*(1+takeprofit)+0.5);    //���Q�I
	int final_price;
	for(int n=1;n<285;n++)
	{
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=slp)
		{
			cout<<"���l"<<endl;
			break;    // final_price�N���|��s�F 
		}
		if(ptx->price[n][2]>=tpp)
		{
			cout<<"���Q"<<endl;
			break;
		}
	}
	return (final_price-cost)*200;
}

int strategy4(TX*ptx)    //�����| 
{
	// price[n-1]<=MA[n-1]&&price[n]>MA[n]&&MA[n-2]>MA[n-1]&&MA[n]>MA[n-1]
	double MA5[285]={0};
	double sum;
	int cost,position=0,r=0;    //position:��W���h��  r:��Q 
	for(int n=4;n<285;n++)    //�Ĥ��������@��|���������� 
	{
		sum=0;
		for(int i=n-4;i<=n;i++)
		{
			sum+=ptx->price[i][2];
		}
		MA5[n]=sum/5;
	}
	for(int n=6;n<285;n++)    //�q�ĤC�����}�l 
	{
		if(ptx->price[n-1][2]<=MA5[n-1]&&ptx->price[n][2]>MA5[n]&&MA5[n-2]>MA5[n-1]&&MA5[n]>MA5[n-1])
		{
			if(position==0)
			{
				position=1;
				cost=ptx->price[n][2];
			}
			else if(position==-1)
			{
				position=0;
				r+=(cost-ptx->price[n][2])*200;
			}
		}
		else if(ptx->price[n-1][2]>=MA5[n-1]&&ptx->price[n][2]<MA5[n]&&MA5[n-2]<MA5[n-1]&&MA5[n]<MA5[n-1])
		{
			if(position==1)
			{
				position=0;
				r+=(ptx->price[n][2]-cost)*200;
			}
			else if(position==0)
			{
				position=-1;
				cost=ptx->price[n][2];
			}
		}
	}
	if(position==1)
	{
		r+=(ptx->price[284][2]-cost)*200;
	}
	else if(position==-1)
	{
		r+=(cost-ptx->price[284][2])*200;
	}
	return r;
}

int strategy5(TX*ptx,double stoploss,double takeprofit,double downward,double upward)
{
	int org_cost=ptx->price[0][2];    //��l�@�I���� 
	int cost=ptx->price[0][2];    //�{�b�Ҫ᪺�� 
	int slp=(int)(org_cost*(1-stoploss)+0.5);    //���l�I
	int tpp=(int)(org_cost*(1+takeprofit)+0.5);    //���Q�I
	int loss=(int)(org_cost*(1-downward)+0.5);    //��X
	int add=(int)(org_cost*(1+upward)+0.5);    //�R�i
	int final_price;
	int amount=1;
	for(int n=1;n<285;n++)
	{
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=loss&&ptx->price[n][2]>slp)
		{
			cost=cost-0.5*ptx->price[n][2];
			amount=amount-0.5;
			cout<<"��X"<<" ";
		}
		if(ptx->price[n][2]>=add&&ptx->price[n][2]<tpp)
		{
			cost=cost+0.5*ptx->price[n][2];
			amount=amount+0.5;
			cout<<"�R�i"<<" ";
		}
		if(ptx->price[n][2]<=slp)
		{
			cout<<"���l"<<endl;
			break;
		}
		if(ptx->price[n][2]>=tpp)
		{
			cout<<"���Q"<<endl;
			break;
		}
	}
	return (final_price*amount-cost)*200;
}

int trading(TX*data,int day_num,int strategy_no)
{
	TX*ptx;
	int day_count;
	int R=0;
	ptx=data;
	for(day_count=0;day_count<day_num;day_count++)
	{
		switch(strategy_no)
		{
			case 1:
				R+=strategy1(ptx);
				break;
			case 2:
				R+=strategy2(ptx,0.01);
				break;
			case 3:
				R+=strategy3(ptx,0.01,0.02);
				break;
			case 4:
				R+=strategy4(ptx);
				break;
			case 5:
				R+=strategy5(ptx,0.01,0.02,0.009,0.019);
				break;
		}
		ptx++;
	}
	return R;
}

int maximum(TX*data,int day_num,int strategy_no)
{
	TX*ptx;
	int day_count;
	int M=0;
	ptx=data;
	for(day_count=0;day_count<day_num;day_count++)
	{
		switch(strategy_no)
		{
			case 1:
				if(strategy1(ptx)>=M)
				{
					M=strategy1(ptx);
				}
				break;
			case 2:
				if(strategy2(ptx,0.01)>=M)
				{
					M=strategy2(ptx,0.01);
				}
				break;
			case 3:
				if(strategy3(ptx,0.01,0.02)>=M)
				{
					M=strategy3(ptx,0.01,0.02);
				}
				break;
			case 4:
				if(strategy4(ptx)>=M)
				{
					M=strategy4(ptx);
				}
				break;
			case 5:
				if(strategy5(ptx,0.01,0.02,0.009,0.019)>=M)
				{
					M=strategy5(ptx,0.01,0.02,0.009,0.019);
				}
				break;
		}
		ptx++;
	}
	return M;
}

int minimum(TX*data,int day_num,int strategy_no)
{
	TX*ptx;
	int day_count;
	int min=0;
	ptx=data;
	for(day_count=0;day_count<day_num;day_count++)
	{
		switch(strategy_no)
		{
			case 1:
				if(strategy1(ptx)<=min)
				{
					min=strategy1(ptx);
				}
				break;
			case 2:
				if(strategy2(ptx,0.01)<=min)
				{
					min=strategy2(ptx,0.01);
				}
				break;
			case 3:
				if(strategy3(ptx,0.01,0.02)<=min)
				{
					min=strategy3(ptx,0.01,0.02);
				}
				break;
			case 4:
				if(strategy4(ptx)<=min)
				{
					min=strategy4(ptx);
				}
				break;
			case 5:
				if(strategy5(ptx,0.01,0.02,0.009,0.019)<=min)
				{
					min=strategy5(ptx,0.01,0.02,0.009,0.019);
				}
				break;
		}
		ptx++;
	}
	return min;
}

int main()
{
	int day_num,count=0,m;    // m���ĴX���� 
	char c,str[101];    //�׽u��c�N�� 
	int year,month,day,hour,minute,exdate,price;
	TX *data,*ptx;
	ifstream fin;    //�H��J�覡�}���ɮ� 
	fin.open("TXF_minute_2021_285.csv");
	fin.getline(str,100);
	while(fin>>year)
	{
		fin>>c;
		fin>>month>>c;
		fin>>day>>c;
		fin>>hour>>c;
		fin>>minute>>c;
		fin>>exdate>>c;
		fin>>price;
		if(hour==13&&minute==29)
		{
			cout<<year<<"/"<<month<<"/"<<day<<endl;
			count++;
		}
	}
	fin.close();    //�����ɮ�
	day_num=count;    //�@���X�� 
	cout<<day_num<<endl;
	data=new TX[day_num];
	ptx=data;
	fin.open("TXF_minute_2021_285.csv");
	fin.getline(str,100);
	m=0;
	while(fin>>year)
	{
		fin>>c;
		fin>>month>>c;
		fin>>day>>c;
		fin>>hour>>c;
		fin>>minute>>c;
		fin>>exdate>>c;
		fin>>price;
		if(m==0)
		{
			ptx->year=year;
			ptx->month=month;
			ptx->day=day;
			ptx->exdate=exdate;
		}
		ptx->price[m][0]=hour;
		ptx->price[m][1]=minute;
		ptx->price[m][2]=price;
		m++;
		if(hour==13&&minute==29)
		{
			m=0;    //�k�s
			ptx++;
		}
	}
	fin.close();    //�����ɮ�
	// data[2].price[100][2]
	cout<<"�����@(buy and hold)�i��Q: "<<trading(data,day_num,1)<<" ���̤j��Q: "<<maximum(data,day_num,1)<<" ���̤j���l: "<<minimum(data,day_num,1)<<endl;
	cout<<"�����G(B&H+���l)�i��Q: "<<trading(data,day_num,2)<<" ���̤j��Q: "<<maximum(data,day_num,2)<<" ���̤j���l: "<<minimum(data,day_num,2)<<endl;
	cout<<"�����T(B&H+���l���Q)�i��Q: "<<trading(data,day_num,3)<<" ���̤j��Q: "<<maximum(data,day_num,3)<<" ���̤j���l: "<<minimum(data,day_num,3)<<endl;
	cout<<"�����|(²�ƪ������ѥ���k�h)�i��Q: "<<trading(data,day_num,4)<<" ���̤j��Q: "<<maximum(data,day_num,4)<<" ���̤j���l: "<<minimum(data,day_num,4)<<endl;
	cout<<"�������i��Q: "<<trading(data,day_num,5)<<" ���̤j��Q: "<<maximum(data,day_num,5)<<" ���̤j���l: "<<minimum(data,day_num,5)<<endl;
	return 0;
} 
