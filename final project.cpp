#include<iostream>
#include<fstream>
using namespace std;

//單日最大獲利、單日最大虧損、累計獲利

struct TX
{
	int year,month,day,exdate;
	int price[285][3];
};

int strategy1(TX *ptx)    //策略一 
{
	int r=(ptx->price[284][2]-ptx->price[0][2])*200;    //一點換算成台幣為 200 元
	cout<<r<<endl;
	return r;    // r是獲利 
}

int strategy2(TX *ptx,double stoploss)    //策略二
{
	int cost=ptx->price[0][2];    //成本 
	int slp=(int)(cost*(1-stoploss)+0.5);    //停損點   +0.5為四捨五入 
	int final_price;
	for(int n=1;n<285;n++)
	{
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=slp)
		{
			cout<<"停損"<<endl;
			break;    //如果是最後一筆數字，final_price就不會更新了 
		}
	}
	return (final_price-cost)*200;    //獲利 
}

int strategy3(TX*ptx,double stoploss,double takeprofit)    //策略三 
{
	int cost=ptx->price[0][2];    //成本 
	int slp=(int)(cost*(1-stoploss)+0.5);    //停損點
	int tpp=(int)(cost*(1+takeprofit)+0.5);    //停利點
	int final_price;
	for(int n=1;n<285;n++)
	{
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=slp)
		{
			cout<<"停損"<<endl;
			break;    // final_price就不會更新了 
		}
		if(ptx->price[n][2]>=tpp)
		{
			cout<<"停利"<<endl;
			break;
		}
	}
	return (final_price-cost)*200;
}

int strategy4(TX*ptx)    //策略四 
{
	// price[n-1]<=MA[n-1]&&price[n]>MA[n]&&MA[n-2]>MA[n-1]&&MA[n]>MA[n-1]
	double MA5[285]={0};
	double sum;
	int cost,position=0,r=0;    //position:手上有多少  r:獲利 
	for(int n=4;n<285;n++)    //第五分鐘為一到四分鐘的平均 
	{
		sum=0;
		for(int i=n-4;i<=n;i++)
		{
			sum+=ptx->price[i][2];
		}
		MA5[n]=sum/5;
	}
	for(int n=6;n<285;n++)    //從第七分鐘開始 
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
	int org_cost=ptx->price[0][2];    //原始一點價格 
	int cost=ptx->price[0][2];    //現在所花的錢 
	int slp=(int)(org_cost*(1-stoploss)+0.5);    //停損點
	int tpp=(int)(org_cost*(1+takeprofit)+0.5);    //停利點
	int loss=(int)(org_cost*(1-downward)+0.5);    //賣出
	int add=(int)(org_cost*(1+upward)+0.5);    //買進
	int final_price;
	int amount=1;
	for(int n=1;n<285;n++)
	{
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=loss&&ptx->price[n][2]>slp)
		{
			cost=cost-0.5*ptx->price[n][2];
			amount=amount-0.5;
			cout<<"賣出"<<" ";
		}
		if(ptx->price[n][2]>=add&&ptx->price[n][2]<tpp)
		{
			cost=cost+0.5*ptx->price[n][2];
			amount=amount+0.5;
			cout<<"買進"<<" ";
		}
		if(ptx->price[n][2]<=slp)
		{
			cout<<"停損"<<endl;
			break;
		}
		if(ptx->price[n][2]>=tpp)
		{
			cout<<"停利"<<endl;
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
	int day_num,count=0,m;    // m為第幾分鐘 
	char c,str[101];    //斜線用c代掉 
	int year,month,day,hour,minute,exdate,price;
	TX *data,*ptx;
	ifstream fin;    //以輸入方式開啟檔案 
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
	fin.close();    //關閉檔案
	day_num=count;    //共有幾天 
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
			m=0;    //歸零
			ptx++;
		}
	}
	fin.close();    //關閉檔案
	// data[2].price[100][2]
	cout<<"策略一(buy and hold)可獲利: "<<trading(data,day_num,1)<<" 單日最大獲利: "<<maximum(data,day_num,1)<<" 單日最大虧損: "<<minimum(data,day_num,1)<<endl;
	cout<<"策略二(B&H+停損)可獲利: "<<trading(data,day_num,2)<<" 單日最大獲利: "<<maximum(data,day_num,2)<<" 單日最大虧損: "<<minimum(data,day_num,2)<<endl;
	cout<<"策略三(B&H+停損停利)可獲利: "<<trading(data,day_num,3)<<" 單日最大獲利: "<<maximum(data,day_num,3)<<" 單日最大虧損: "<<minimum(data,day_num,3)<<endl;
	cout<<"策略四(簡化版葛蘭碧交易法則)可獲利: "<<trading(data,day_num,4)<<" 單日最大獲利: "<<maximum(data,day_num,4)<<" 單日最大虧損: "<<minimum(data,day_num,4)<<endl;
	cout<<"策略五可獲利: "<<trading(data,day_num,5)<<" 單日最大獲利: "<<maximum(data,day_num,5)<<" 單日最大虧損: "<<minimum(data,day_num,5)<<endl;
	return 0;
} 
