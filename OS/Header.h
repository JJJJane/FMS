//
//  Header.h
//  OS
//
//  Created by TNT on 2018/8/10.
//  Copyright © 2018年 TNT. All rights reserved.
//

#ifndef Header_h
#define Header_h

#include<cmath>
#include<ctime>
#include<string>
//#include<conio.h>
#include<fstream>
#include<cstdlib>
#include<iostream>
//#include<windows.h>
using namespace std;

typedef struct UFD
{
    string File_name;         //文件名
    int Start;                //文件在磁盘存储空间的起始地址
    int Protect;              //文件的属性
    int File_length;           //文件的长度
    int Max_File_length;         //文件的最大长度
    struct UFD *next;
}UFD,*UFD_ptr;

typedef struct MFD
{
    string User_name;
    string User_passwd;
    UFD *Ufd_next;
    int End;
    struct MFD *next;
}MFD,*MFD_ptr;

typedef struct AFD
{
    string File_name;
    int File_length;
    int Start;
    int Protect;
    int Pointer;    //根据这个指针的值去完成用户的要求  读写指针
    int Max_File_length;
    struct AFD *next;
}AFD,*AFD_ptr;

#endif /* Header_h */
