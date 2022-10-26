// ConsoleApplication4.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include "winio.h"  //导入winio头文件
//#include  <minwindef.h>
//#include <stdlib.h>
//#define PCI_CONFIG_ADDRESS 0xcf8  //pci地址映射
//#define PCI_CONFIG_DATA 0xcfc  //pci数据映射
//#include INDEX_PORT 0x295;
//#include DATA_PORT 0x296;
#pragma comment(lib,"winio.lib")  //导入winio库文件

		/*   总体思路：先进PNP mode ，然后选择LDN，选择之后才能读写寄存器，最后退出  */
void EnterPnpMode1();//进入PNP mode（2E）
void EnterPnpMode();//进入PNP mode（4E）
void SelectLDN(unsigned int LDNumber);//选择逻辑设备号
void ReadLdnRegister();//读取该逻辑设备的寄存器
void WriteLdnRegister(unsigned int index, unsigned int data);//写入该逻辑设备的某个寄存器值
void ExitPnpMode();															//退出Pnp mode
unsigned int ReadEcSpace(unsigned int index);								  //读Ec空间
unsigned int WriteEcSpace(unsigned int index, unsigned int data);			//写入Ec空间
void ReadTemp();														//读取温度和风速
unsigned int ReadFan(unsigned int index, unsigned int index1, unsigned int value);
unsigned int ReadVoltage(unsigned int index, unsigned int value);		//读取电压
unsigned int FullMode();												//满转速模式	
unsigned int CPUAutoMode();												//smart fan自动模式
unsigned int CPUManualMode(unsigned int value);											//smart fan手动模式
unsigned int OnlyRead();
unsigned int OnlyRead2();

int a=0,speed;
char ch=0;
int main()
{
	/*
	int bus, dev, func,count=0;
//	int bus = 00, dev = 2, func = 0;
//	int bus1 = 0x00, dev1 = 0x1F,func1=0x04;
//	int a[20][20], reg, i, j;
	DWORD dwAddr,dwData,VID,DID;

	if (InitializeWinIo())
	{
		
		printf("VID\tDID\tBUS\tDEV\tFUNC\t\n");
		for (bus=0; bus < 8; bus++)
		{
			for (dev = 0; dev < 32; dev++)
			{
				for (func = 0; func < 8;func++)
				{
					dwAddr = 0x80000000 + (bus << 16) + (dev << 11) + (func << 8);//bus，dev，func都是我们设定的初始值然后需要移动到32位地址寄存器中
					SetPortVal(PCI_CONFIG_ADDRESS, dwAddr, 4);//I/O口写操作，第一个端口地址，16位整变量或常量；第二个为要写入端口的32位整型变量或常量  
					GetPortVal(PCI_CONFIG_DATA, &dwData, 4);//I/O读操作，第一个为端口地址，第二个为存放端口内容的32为整型变量指针
					if (dwData!=0xffffffff) //数据这里如果全“1”，表示无效
					{
//						count++;
						VID = dwData & 0xffff;
						DID = (dwData >> 16) & 0xffff;
						printf("%04x\t%04x\t%02x\t%02x\t%02x\t\n", VID, DID, bus, dev, func);  //02,04表示不足2，4位，前面补0输出
					}
				}
			}
		}
		printf("\nFind PCI Devic %d\n", count);		
		*/
	
	if (InitializeWinIo())
	{
		
		EnterPnpMode1();
		SelectLDN(0x04);
		WriteLdnRegister(0x30, 0x01);
		CPUAutoMode();
		OnlyRead();
		while (true)
		{
			if (_kbhit())
				ch = _getch();
			if (ch == 27)
			{
				ch = 0;
				printf("请选择模式：\n   1 自动模式（默认）\n   2 手动模式\n   3 满速模式\n");
				scanf_s("%d", &a);
				system("cls");
				break;
			}
			if (a == 1)
			{
				ExitPnpMode();
				EnterPnpMode1();
				SelectLDN(0x04);
				WriteLdnRegister(0x30, 0x01);
				CPUAutoMode();
				OnlyRead();
			}
			else if (a == 2)
			{
				ExitPnpMode();
				EnterPnpMode1();
				SelectLDN(0x04);
				WriteLdnRegister(0x30, 0x01);
				CPUManualMode(0x7F);
				OnlyRead();		
				OnlyRead2();
			}
			else if (a == 3)
			{
				ExitPnpMode();
				EnterPnpMode1();
				SelectLDN(0x04);
				WriteLdnRegister(0x30, 0x01);
				FullMode();
				OnlyRead();
			}
		}
		ShutdownWinIo();
	}
	else
	{
		printf("Initialize Win IO Fail\n");

	}
	return 0;
}

void EnterPnpMode()
{
	SetPortVal(0x4E, 0x87, 1);
	SetPortVal(0x4E, 0x01, 1);
	SetPortVal(0x4E, 0xAA, 1);   //有一个显示是0x55
	SetPortVal(0x4E, 0xAA, 1);
	return;
}

void EnterPnpMode1()
{
	SetPortVal(0x2E, 0x87, 1);
	SetPortVal(0x2E, 0x01, 1);
	SetPortVal(0x2E, 0x55, 1);
	SetPortVal(0x2E, 0x55, 1);
	return;
}

void SelectLDN(unsigned int LDNumber)
{
	SetPortVal(0x2E, 0x07, 1);
	SetPortVal(0x2F, LDNumber, 1);
	return;
}

void ReadLdnRegister()
{

}

void WriteLdnRegister(unsigned int index,unsigned int data)
{
	SetPortVal(0x2E, index, 1);
	SetPortVal(0x2F, data, 1);
}

void ExitPnpMode()
{
	WriteLdnRegister(0x02, 0xE2);
}

unsigned int ReadEcSpace(unsigned int index)
{
	DWORD data;
	SetPortVal(0x0A45, index, 1);
	GetPortVal(0x0A46, &data, 1);
	return data;
}

unsigned int WriteEcSpace(unsigned int index, unsigned int data)
{
	SetPortVal(0x0A45, index, 1);
	SetPortVal(0x0A46, data, 1);
	return 0;
}

/*思路：温度通过温度读取寄存器即2B-29h
		风扇转速也可以通过风扇转速读取寄存器获得即0D-0F*/
void ReadTemp()
{
	unsigned int temp0, temp1, temp2;

	temp0 = ReadEcSpace(0x29);			//可能是cpu
	temp1 = ReadEcSpace(0x2A);			//可能是系统
//	temp2 = ReadEcSpace(0x2B);
	printf("CPU-Temp=%d\n", temp0);
	printf("SYS-Temp=%d\t\n", temp1);
//	printf("Temperature2=%d\n", temp2);
}

/*0D-0F为风扇1-3的低位，高位为18-1A
  而CPU风扇处于fan1，
  fan4-5的低位为80、82，高位为81、83*/
unsigned int ReadFan(unsigned int index, unsigned int index1, unsigned int value)
{
	if (value == 1)
	{
		unsigned int fan;
		unsigned int fanMSB,fanLSB;

		fanLSB = ReadEcSpace(index);
		fanMSB = ReadEcSpace(index1);
		fan = (fanMSB << 8 & 0xff00) | fanLSB;
		//fan = (fanLSB << 8 & 0xff00) | fanMSB;
//		printf("fanMSB: %x or %d\n", fanMSB, fanMSB);
//		printf("fanLSB: %x or %d\n", fanLSB, fanLSB);
//		printf("fan: %x\n", fan);
		printf("CPU-fan:%d\n", (135 * 10000) / (fan * 2));
	}
	else
	{
		unsigned int fan;
		unsigned int fanMSB, fanLSB;

		fanMSB = ReadEcSpace(index);
		fanLSB = ReadEcSpace(index1);
		fan = (fanMSB << 8 & 0xff00) | fanLSB;
//		printf("fanMSB: %x or %d\n", fanMSB, fanMSB);
//		printf("fanLSB: %x or %d\n", fanLSB, fanLSB);
//		printf("fan: %x or %d\n", fan, fan);
		printf("fan: %x\n", fan);
		printf("fan%d:%d\n", value,(135 * 10000) / (fan * 2));
	}
	return 0;
}

/*电压对于的位点为20-24*/
unsigned int ReadVoltage(unsigned int index, unsigned int value)
{
	unsigned int vin1,vin;
	float	voltage1, voltage2;
	vin1 = ReadEcSpace(index);
	if (value==1||value==2)
	{
//		vin=hextoDec(vin1);
//		printf("%d\n", vin);
		voltage1 = (float)vin1 * 12 / 1000;
		printf("%02x\n", vin1);
		printf("voltage%d:    %f", value, voltage1);
		printf("\n");
	}
	else if (value==3)
	{
		voltage1 = (float)vin1 * 12 / 1000;
		voltage1 = voltage1*(10 + 2) / 2;
		printf("%02x\n", vin1);
		printf("voltage%d:    %f", value, voltage1);
		printf("\n");
	}
	else if (value==4)
	{
		voltage1 = (float)vin1 * 12 / 1000;
		voltage1 = voltage1*(15 +10) / 10;
		printf("%02x\n", vin1);
		printf("voltage%d:    %f", value, voltage1);
		printf("\n");
	}
	else if (value == 5)
	{
		voltage1 = (float)vin1 * 12 / 1000;
		voltage1 = voltage1*(6.49 + 10) / 10;
		printf("%02x\n", vin1);
		printf("voltage%d:    %f", value, voltage1);
		printf("\n");
	}
	
//	printf("%x\n", vin1);
//	printf("voltage%d:    %d\n", value, voltage1);
//	printf("voltage%d:    %d.%d\n", value, voltage1, voltage2);
	return 0;
}

unsigned int FullMode()
{
	WriteEcSpace(0x13, 0x70);
	WriteEcSpace(0x14, 0x97);
	return 0;
}

unsigned int CPUAutoMode()
{
	WriteEcSpace(0x13, 0x71);		//使能并使fan1使用smart模式
	WriteEcSpace(0x14, 0x90);
	WriteEcSpace(0x15, 0x80);		//选择smart的automatic模式，温度由TMPIN1（cpu）提供
	WriteEcSpace(0x60, 0x2D);		//温度设为45(34)
	WriteEcSpace(0x61, 0x37);		//温度为55(40)
	WriteEcSpace(0x62, 0x41);		//温度65(47)
	WriteEcSpace(0x63, 0x7F);		//初始转速
	WriteEcSpace(0x64, 0x75);
	return 0;
}

/*计算0x63的值的方式：想要的转速/满转速度*256（类似于占空比），上式结果变为十六进制即可*/
unsigned int CPUManualMode(unsigned int value)
{
	WriteEcSpace(0x13, 0x71);		//使能并使fan1使用smart模式
	WriteEcSpace(0x14, 0x90);
	WriteEcSpace(0x15, 0x00);		//选择手动模式
	WriteEcSpace(0x63, value);		//初始速度大概1700-1800
	return 0;
}

int hextoDec(int hex){
	int sum = 0, mul = 1;
	int i, r;
	int count = 0;
	do{
		r = hex % 16;
		for (i = 0; i<count; i++)
			mul *= 16;
		mul *= r;
		sum += mul;
		mul = 1;
		count++;
	} while (hex /= 16);
	return sum;

}

unsigned int OnlyRead()
{
	while (true)
	{
		ReadTemp();
		printf("\n");
		ReadFan(0x0D, 0x18, 1);
		printf("\n");
		ReadVoltage(0x20, 1);
		ReadVoltage(0x21, 2);
		ReadVoltage(0x22, 3);
		ReadVoltage(0x23, 4);
		ReadVoltage(0x24, 5);
		Sleep(300);
		system("cls");
		if (_kbhit())
			ch = _getch();
		if (ch == 13)
		{
			ch = 0;
//			system("pause");
			printf("请选择模式：\n   1 自动模式（默认）\n   2 手动模式\n   3 满速模式\n");
			scanf_s("%d", &a);
			system("cls");
			break;
		}
		
		if (a == 2)
		{
			system("cls");
			break;
		}
	}
	return a;
}

unsigned int OnlyRead2()
{
	while (true)
	{
		ch = 0;
		static int speed=1800;
		unsigned int value;
		char str[2];
		int m = 0;
		ReadTemp();
		printf("\n");
		ReadFan(0x0D, 0x18, 1);
		printf("\n");
		ReadVoltage(0x20, 1);
		ReadVoltage(0x21, 2);
		ReadVoltage(0x22, 3);
		ReadVoltage(0x23, 4);
		ReadVoltage(0x24, 5);
		printf("转速(0-3300)：   %d\n", speed);
		if (_kbhit())
			ch = _getch();
		if (ch == 13)
		{
			ch = 0;
			Sleep(300);
			if (_kbhit())
				ch = _getch();
			if (ch == 13)
			{
				ch = 0;
				system("cls");
				printf("请选择模式：\n   1 自动模式（默认）\n   2 手动模式\n   3 满速模式\n");
				scanf_s("%d", &a);
				system("cls");
				break;
			}
			scanf_s("%d", &speed);
			if (speed<3400)
			{
				value = speed * 0x100 / 3650;
				CPUManualMode(value);
			}
			else
			{
				printf("请正确输入转速！\n");
			}			
		}
		
		/*
		else
		{

			ch = 0;
			printf("请选择模式：\n   1 自动模式（默认）\n   2 手动模式\n   3 满速模式\n");
			scanf_s("%d", &a);
			system("cls");
			break;
		}*/	
		Sleep(300);
		system("cls");
	}
	return 0;
}
