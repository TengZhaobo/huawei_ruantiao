#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include<algorithm>
#include <functional>
#include <time.h>
using namespace std;




vector<vector<int> > Demand;
vector<vector<int> > Qos;
vector<vector<int> > D_Qos;  //延时Qos的可用不可用表
vector<vector<int> > Site_Bandwidth;
vector<vector<int> > Demand_keep;
int qos_constraint;

vector<string>Demand_Client_Name;//Demand表--> 客户的名字
vector<string>Qos_Client_Name;//Qos表--> 客户的名字
vector<string>Qos_Site_Name;//Qos表--> 结点的名字
vector<string>Bandwidth_Site_Name;//Bandwidth表 -->结点的名字

/****可用结点坐标*****/
vector<int> x;
vector<int> y;
/*********三维容器  存放客户 、 时刻 、 每个结点用掉的带宽***********/
//三维容器有问题，不能进行全局定义
//vector<vector<vector<int> > > use_bandwidth(D_Qos[1].size(), vector<vector<int> >(Demand.size(), vector<int>(D_Qos.size(), 0)));//三维容器  存放客户 、 时刻 、 每个结点用掉的带宽
vector<vector<int> > use_bandwidth_2(Demand.size(), vector<int>(D_Qos.size(), 0)); //二维容器  存放 时刻 、 每个结点用掉的带宽
vector <int> use_bandwidth_1;       //一维容器  存放每个结点用掉的带宽
vector<int>use_bandwidth_finally;   //一维容器  每个结点最终用掉的带宽值
/********************关联三个表容器*******************/


//普通遍历函数
void print01(int val)
{
	cout << val << " ";
}

void Date_demand()   //读取demand.csv文件   客户节点在该时刻的带宽需求
{
	int i = 0, j = 0;
	// 读文件
	//ifstream inFile( "demand.csv", ios::in);  //客户节点在该时刻的带宽需求
	ifstream inFile;
	inFile.open("/data/demand.csv");    //./data/demand.csv
	if (!inFile)
	{
		cout << "打开文件失败！" << endl;
		exit(1);
	}
	string lineStr;
	/*
	vector<string>Demand_Client_Name;//Demand表--> 客户的名字
	vector<string>Qos_Client_Name;//Qos表--> 客户的名字
	vector<string>Qos_Site_Name;//Qos表--> 结点的名字
	vector<string>Bandwidth_Site_Name;//Bandwidth表 -->结点的名字
	*/
	//	vector<vector<int> > Demand;
	while (getline(inFile, lineStr))
	{
		// 打印整行字符串
		// cout << lineStr << endl;
		// 存成二维表结构
		stringstream ss(lineStr);
		string str;
		int demand;
		vector<string> lineArray0;
		vector<int> lineDemand;
		// 按照逗号分隔
		while (getline(ss, str, ','))
		{
			lineArray0.push_back(str);
			/****************只读需求表的第一行（得到客户的名字）********************/
			if (i == 0)
			{
				Demand_Client_Name.push_back(str);  //Demand表--> 客户的名字
			}
			istringstream is(str); //构造输入字符串流，流的内容初始化为“12”的字符串					
			is >> demand;
			lineDemand.push_back(demand);
		}
		i = 1;  //  Demand_Client_Name 容器只读一次
		if (j == 0)
		{
			Demand_keep.push_back(lineDemand);
			j++;
		}
		else
		{
			lineDemand.erase(lineDemand.begin());
			Demand.push_back(lineDemand);
			Demand_keep.push_back(lineDemand);
		}
	}
	inFile.close();
	/***********************查看客户需求带宽 二维遍历*****************************/
	//cout << "-------------------" << endl;
	//for (int i = 0; i < (int)Demand.size(); i++)
	//{

	//	for (int j = 0; j <(int) Demand[1].size(); j++)
	//	{
	//		cout << Demand[i][j] << " ";

	//	}
	//	cout << endl;
	//}
	//cout << "共读取了：" << Demand[1].size() << "列" << endl;
	//cout << "共读取了：" << Demand.size() << "行" << endl;
}
void Date_qos()   //读取qos.csv文件   延长时间
{
	// 读文件
	ifstream inFile("/data/qos.csv", ios::in);   //./data/qos.csv
	if (!inFile)
	{
		cout << "打开文件失败！" << endl;
		exit(1);
	}
	string lineStr;
	int i = 0, j = 0;
	while (getline(inFile, lineStr))
	{
		// 打印整行字符串
		//	cout << lineStr << endl;      //个人看法最好是用这个语句遍历，把for循环屏蔽了
		// 存成二维表结构
		stringstream ss(lineStr);
		string str;
		int qos;
		vector<string> lineArray0;
		vector<int> lineQos;
		vector<int>vTarget; //目标容器
		// 按照逗号分隔
		/*
		vector<string>Demand_Client_Name;//Demand表--> 客户的名字
		vector<string>Qos_Client_Name;//Qos表--> 客户的名字
		vector<string>Qos_Site_Name;//Qos表--> 结点的名字
		vector<string>Bandwidth_Site_Name;//Bandwidth表 -->结点的名字
		*/
		while (getline(ss, str, ','))
		{
			lineArray0.push_back(str);
			if (i == 0)
			{
				Qos_Client_Name.push_back(str);  //客户名字
			}
			istringstream is(str); //构造输入字符串流，流的内容初始化为“is”的字符串					
			is >> qos;
			lineQos.push_back(qos);
			/********************这个地方需要注意QOS等不到于400***********************/
			if (qos < qos_constraint)   //qos  <400  
			{
				vTarget.push_back(1);
			}
			else
			{
				vTarget.push_back(0);
			}
		}
		i = 1;    //Qos_Client_Name.push_back(str);  存放客户名字
		if (j == 0)
		{
			Qos_Site_Name.push_back(lineArray0[0]);
			j++;
		}
		else
		{
			lineQos.erase(lineQos.begin());
			vTarget.erase(vTarget.begin());

			Qos_Site_Name.push_back(lineArray0[0]);
			Qos.push_back(lineQos);
			D_Qos.push_back(vTarget);
		}
	}
	inFile.close();
	/**********创建一个可用不可用表***********/
	//for (int i = 0; i < Qos.size(); i++)
	//{
	//	for (int j = 0; j < Qos[1].size(); j++)
	//	{
	//		//cout << Qos[i][j] << " ";
	//		cout << D_Qos[i][j] << " ";
	//	}
	//	cout << endl;
	//}
	for (i = 0; i < D_Qos.size(); i++)    //结点
	{
		for (int j = 0; j < D_Qos[1].size(); j++)   //客户
		{
			if (D_Qos[i][j] == 1)
			{
				x.push_back(i);  //客户交集的结点位置
				y.push_back(j);  //一个结点对应的可分配的客户
			}
		}
	}
	//for (int i = 0; i < x.size(); i++)
	//{
	//	cout << "X 轴坐标：" << x[i] << "  Y轴坐标：" << y[i] << endl;
	//	//cout << "(" << x[i] << " , " << y[i] << ")";
	//}
	//cout << endl;

}
void Date_site_bandwidth()   //读取site_bandwidth文件   每个边缘节点的最大带宽
{
	// 读文件
	ifstream inFile("/data/site_bandwidth.csv", ios::in);    //./data/site_bandwidth.csv

	if (!inFile)
	{
		cout << "打开文件失败！" << endl;
		exit(1);
	}
	string lineStr;


	int i = 0, j = 0;
	while (getline(inFile, lineStr))
	{
		// 打印整行字符串
		//	cout << lineStr << endl;
		// 存成二维表结构
		stringstream ss(lineStr);
		string str;
		int site_bandwidth;
		vector<string> lineArray0;
		vector<int> line_Site_Bandwidth;
		// 按照逗号分隔
		while (getline(ss, str, ','))
		{
			lineArray0.push_back(str);
			istringstream is(str); //构造输入字符串流，流的内容初始化为“is”的字符串					
			is >> site_bandwidth;
			line_Site_Bandwidth.push_back(site_bandwidth);
		}
		if (j == 0)
		{
			Bandwidth_Site_Name.push_back(lineArray0[0]);
			j++;
		}
		else
		{
			line_Site_Bandwidth.erase(line_Site_Bandwidth.begin());
			Bandwidth_Site_Name.push_back(lineArray0[0]);
			Site_Bandwidth.push_back(line_Site_Bandwidth);
		}
	}
	inFile.close();

}
/*
Demand_Client_Sort[][];
Bandwidth_Site_Sort[];
*/
vector<vector<int> > Demand_Client_Sort;
vector<int>Bandwidth_Site_Sort;
vector <int> My_Demand_Client_Sort;  //需求表调用序列的顺序
vector <int> My_Bandwidth_Site_Sort;  //最大结点带宽表 调用序列的顺序
		/*
	vector<string>Demand_Client_Name;//Demand表--> 客户的名字
	vector<string>Qos_Client_Name;//Qos表--> 客户的名字
	vector<string>Qos_Site_Name;//Qos表--> 结点的名字
	vector<string>Bandwidth_Site_Name;//Bandwidth表 -->结点的名字
	*/
void  Correlation_Bandwidth_Table()
{
	/******************关联QOS表和结点带宽表的结点对应*******************/
	for (int i = 1; i < Qos_Site_Name.size(); i++)
	{
		for (int j = 1; j < Bandwidth_Site_Name.size(); j++)
		{
			//cout << i << "   Qos_Site_Name[i]    " << Qos_Site_Name[i] << endl;
			//cout << j << "    Bandwidth_Site_Name[j]  " << Bandwidth_Site_Name[j] << endl;
			if (Qos_Site_Name[i] == Bandwidth_Site_Name[j])
			{
				//cout << i << "   Qos_Site_Name[i]    " << Qos_Site_Name[i] << endl;
				//cout << j << "    Bandwidth_Site_Name[j]  " << Bandwidth_Site_Name[j] << endl;
				My_Bandwidth_Site_Sort.push_back(j - 1);
				//break;
			}
		}
	}
	/*****************将带宽结点表的名称按照QOS表的来*******************/
	for (int i = 0; i < Qos_Site_Name.size(); i++)
	{
		Bandwidth_Site_Name.push_back(Qos_Site_Name[i]);
	}
	/************结点带宽表调用序列的顺序************/
	//for (int i = 0; i < My_Bandwidth_Site_Sort.size(); i++)
	//{
	//	cout << My_Bandwidth_Site_Sort[i] << " ";
	//}
	//cout << endl;
	/****************查看调整顺序后的结点带宽表*********************/
	for (int i = 0; i < Site_Bandwidth.size(); i++)     //需要注意的是遍历结点带宽表时，将 Site_Bandwidth[i][0] 第二个值置零即可
	{
		Bandwidth_Site_Sort.push_back(Site_Bandwidth[My_Bandwidth_Site_Sort[i]][0]);
		//cout << Site_Bandwidth[i][0] << "   ";
		//cout << Site_Bandwidth[i][0] << "   " << Site_Bandwidth[My_Bandwidth_Site_Sort[i]][0] << "  " << Bandwidth_Site_Sort[i];
		//Site_Bandwidth[My_Bandwidth_Site_Sort[i]][j]; //按照QOS表的客户顺序，调用需求表
		//cout << endl;
	}
}

void  Correlation_Demand_Table()     //
{
	/******************关联QOS表和需求表的客户对应*******************/
	for (int i = 1; i < Qos_Client_Name.size(); i++)
	{
		for (int j = 1; j < Demand_Client_Name.size(); j++)
		{
			if (Qos_Client_Name[i] == Demand_Client_Name[j])
			{
				//cout << "Qos_Client_Name[i]  " << Qos_Client_Name[i] << endl;
				//cout << "Demand_Client_Name[j]  " << Demand_Client_Name[j] << endl;
				My_Demand_Client_Sort.push_back(j - 1);
			}
		}
	}
	/*****************将需求表客户的名称按照QOS表的来*******************/
	for (int i = 0; i < Qos_Client_Name.size(); i++)
	{
		Demand_Client_Name.push_back(Qos_Client_Name[i]);
	}
	/************需求表调用序列的顺序************/
	//for (int i = 0; i < My_Demand_Client_Sort.size(); i++)
	//{
	//	cout << My_Demand_Client_Sort[i] << " ";
	//}
	//cout << endl;
	/****************查看调整顺序后的需求表*********************/
	for (int i = 0; i < Demand.size(); i++)
	{
		vector<int>Demand_Client_Sort_1;
		for (int j = 0; j < Demand[0].size(); j++)
		{
			Demand_Client_Sort_1.push_back(Demand[i][My_Demand_Client_Sort[j]]);    //存放关联后的一维的客户需求表
			//cout << Demand[i][j] << "  ";
			Demand[i][My_Demand_Client_Sort[j]]; //按照QOS表的客户顺序，调用需求表
			//cout << Demand[i][My_Demand_Client_Sort[j]] << " ";
		}
		//cout << endl;
		Demand_Client_Sort.push_back(Demand_Client_Sort_1);
	}
	/*************查看关联后的需求表************/
	//cout << "______________________________________" << endl;
	//for (int i = 0; i < Demand.size(); i++)
	//{
	//	for (int j = 0; j < Demand[0].size(); j++)
	//	{
	//		cout << Demand_Client_Sort[i][j] << "  ";
	//	}
	//	cout << endl;
	//}
	//cout << "______________________________________" << endl;
		/*
	vector<string>Demand_Client_Name;//Demand表--> 客户的名字
	vector<string>Qos_Client_Name;//Qos表--> 客户的名字
	vector<string>Qos_Site_Name;//Qos表--> 结点的名字
	vector<string>Bandwidth_Site_Name;//Bandwidth表 -->结点的名字
	*/
	//for (int i = 0; i < Demand_Client_Name.size(); i++)
	//{
	//	cout << "Demand表-->客户名字：" << Demand_Client_Name[i] << endl;
	//}
	//for (int i = 0; i < Qos_Site_Name.size(); i++)
	//{
	//	cout << "Qos表字符串_结点名字：" << Qos_Site_Name[i] << endl;
	//}
	//for (int i = 0; i < Qos_Client_Name.size(); i++)
	//{
	//	cout << "Qos表字符串_客户名字：" << Qos_Client_Name[i] << endl;
	//}
	//for (int i = 0; i < Bandwidth_Site_Name.size(); i++)
	//{
	//	cout << "最大带宽结点名字：" << Bandwidth_Site_Name[i] << endl;
	//}

}

void Date_config()   //读取config.ini文件    最大延迟时间
{
	ifstream inFile("/data/config.ini", ios::in);    //./data/config.ini
	if (!inFile)
	{
		cout << "打开文件失败！" << endl;
		exit(1);
	}
	int i = 0, j = 0;
	string line;
	string field;
	vector<string> demand1;
	vector<string> demand2;
	while (getline(inFile, line))//getline(inFile, line)表示按行读取CSV文件中的数据
	{
		string field;
		istringstream sin(line); //将整行字符串line读入到字符串流sin中
		while (getline(sin, field, ','))
		{
			demand1.push_back(field);  //将数据存到容器中
			cout << field << " ";
		}
		cout << endl;
	}
	/* ---------------------- 读取config.ini文件 提取配置参数 ----------------  */
	int pos_1 = demand1[1].find("=");
	int pos_2 = demand1[1].size();
	string  constraint = demand1[1].substr(pos_1 + 1, pos_2);


	istringstream is(constraint); //构造输入字符串流，流的内容初始化为“12”的字符串   
	is >> qos_constraint; //从is流中读入一个int整数存入i中
	inFile.close();

	cout << "读取数据完成" << endl;
}


/*************----------索引结构体-----------*************/
typedef struct
{
	int index;
	int value;
}sort_st;
bool compare(sort_st a, sort_st b)
{
	return a.value < b.value; //升序排列，如果改为return a.value<b.value，则为降序
}
typedef struct
{
	int index;
	int value;
}time_st;
bool compare_1(time_st  a, time_st b)
{
	return a.value > b.value; //降序排列
}
bool compare_2(int a, int b)
{
	return a > b; //降序排列
}
//  Site_Bandwidth[My_Bandwidth_Site_Sort[i]][0]   //结点带宽值按QOS表的结点顺序排列
//	Demand[i][My_Demand_Client_Sort[j]]       //需求表的客户按照QOS表的客户顺序排列
void Bandwidth_Distribute()
{
	
}
int main()
{
	clock_t start;
	clock_t finish_1, finish_2, finish_3, finish_4, finish_5, finish;
	start = clock();

	Date_config();
	finish_1 = clock();
	int duration_1 = finish_1 - start;
	cout << "读取最大延时文件总共用时为：" << duration_1 << "毫秒" << endl;


	Date_demand();             //Demand[i][j]
	finish_2 = clock();
	int duration_2 = finish_2 - finish_1;
	cout << "读取客户需求总共用时为：" << duration_2 << "毫秒" << endl;


	Date_qos();                //Qos[i][j]     D_Qos[i][j]
	finish_3 = clock();
	int duration_3 = finish_3 - finish_2;
	cout << "读取边沿结点总共用时为：" << duration_3 << "毫秒" << endl;


	Date_site_bandwidth();       // Site_Bandwidth[i]
	Correlation_Demand_Table();       //关联QOS表、需求表
	Correlation_Bandwidth_Table();    //关联QOS表、结点带宽表

	finish_4 = clock();
	int duration_4 = finish_4 - finish_3;
	cout << "读取边沿结点带宽值总共用时为：" << duration_4 << "毫秒" << endl;

	//Customer_Bandwidth();
	Bandwidth_Distribute();
	finish_5 = clock();
	int duration_5 = finish_5 - finish_4;
	cout << "读取带宽分配总共用时为：" << duration_5 << "毫秒" << endl;
	finish = clock();
	int duration = finish - start;
	double durationS = (double)duration / CLOCKS_PER_SEC;//如果需要换算成秒
	cout << "总共用时为：" << durationS << "秒" << endl;
	system("pause");
	return 0;

}
