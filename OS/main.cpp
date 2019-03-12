//
//  main.cpp
//  OS
//
//  Created by TNT on 2018/8/10.
//  Copyright © 2018年 TNT. All rights reserved.
//
#include "Header.h"

#define MaxDisk 512*1024   //一个扇区512字节,假设有1024个扇区,模拟磁盘的大小
int Max_User=10;
int Max_Open=5;
int Max_End=0;
UFD_ptr pufd=NULL;
MFD_ptr pmfd=NULL;
AFD_ptr pafd=NULL;
char User_name[30];         //存放当前用户的用户名
char flag='n';
char Disk_Size[MaxDisk];    //最后在打印输出的时候，用RWX表示即可

typedef struct Disk_Table
{
    int Max_length;      //最大长度
    int Start;          //开始位置
}Disk_Node;
Disk_Node Disk_Head;

void Init_Disk()
{
    Disk_Head.Max_length=MaxDisk;
    Disk_Head.Start=0;
}

bool Init_MFD()
{
    pmfd=(MFD *)new MFD;       //带头节点的单向链表
    MFD *p=pmfd;
    ifstream ifs("MFD.txt");  //文件的输入流对象
    if(!ifs)
    {
        cerr<<"错误：无法打开文件"<<endl;
        p->next=NULL;
        p->Ufd_next=NULL;
        return false;
    }
    while(!ifs.eof())
    {
        p->next=(MFD *)new MFD;
        ifs>>p->next->User_name>>p->next->User_passwd>>p->next->End;
        if(p->next->End>Max_End)
            Max_End=p->next->End;
        p=p->next;
        p->Ufd_next=NULL;
        p->next=NULL;
    }
    ifs.close();
    return true;
}

void Print_UFD()
{
    UFD *p=pufd->next;
    if(!p)
    {
        cout<<"抱歉,该用户没有创建任何文件,请先创建。"<<endl;
        return ;
    }
    cout<<"文件名\t\t最大文件长度\t文件权限\t起始位置\t文件长度"<<endl;
    while(p)
    {
        cout<<p->File_name<<"\t\t"<<p->Max_File_length;
        if(p->Protect==0)
            cout<<"\t\t"<<"---";
        else if(p->Protect==1)
            cout<<"\t\t"<<"r-x";
        else if(p->Protect==2)
            cout<<"\t\t"<<"rwx";
        cout<<"\t\t"<<p->Start;
        cout<<"\t\t"<<p->File_length<<endl;
        p=p->next;
    }
}

bool Init_UFD(char *name)
{
    ifstream ifs(name);
    pufd=(UFD *)new UFD;
    UFD *p=pufd;
    char temp[MaxDisk];
    bool Index=false;
    if(!ifs)
    {
        cerr<<"错误：无法打开文件"<<endl;
        p->next=NULL;
        return false;
    }
    while(!ifs.eof())
    {
        memset(temp,'\0',sizeof(temp));
        p->next=(UFD *)new UFD;
        if(!Index)
        {
            pmfd->Ufd_next=p->next;
            Index=true;
        }
        ifs>>p->next->File_name>>p->next->Max_File_length>>p->next->Protect>>p->next->Start>>p->next->File_length;
        ifs>>temp;
        if(temp[0]=='#')
            temp[0]='\0';
        for(int i=p->next->Start,j=1;j<=p->next->Max_File_length-1;i++,j++)
        {
            Disk_Size[i]=temp[j-1];
            if(!Disk_Size[i])
                break;
        }
        Disk_Head.Max_length-=p->next->Max_File_length;
        //    Disk_Head.Start+=p->next->Max_File_length;
        p=p->next;
        p->next=NULL;
    }
    ifs.close();
    return true;
}

void Rename_File()
{
    string File_name;
    UFD *temp=pufd;
    char Index;
SD:
    cout<<"请输入文件的名字："<<endl;
    cin>>File_name;
    while(temp)
    {
        if(temp->File_name==File_name)
        {
            cout<<"请输入新的文件名："<<endl;
            cin>>File_name;
            temp->File_name=File_name;
            break;
        }
        temp=temp->next;
    }
    if(!temp)
    {
        cout<<"抱歉,输入的文件不存在,无法完成重命名文件操作,是否继续操作<y/n>?"<<endl;
        cin>>Index;
        if(Index=='y')
            goto SD;
        else
            return ;
    }
    else
        cout<<"修改成功。"<<endl;
}
bool User_Check()
{
    string User_passwd;
    int Count=0;
    while(1)
    {
        cout<<"请输入用户名：";
        cin>>User_name;
    INIT:
        cout<<"请输入密码：";
        cin>>User_passwd;
        MFD *p=pmfd->next;
        char temp[30];
        memset(temp,'\0',sizeof(temp));
        strcpy(temp,User_name);
        Count++;
        while(p)
        {
            if(User_name==p->User_name)
            {
                if(User_passwd==p->User_passwd)
                    cout<<"登入成功."<<endl;
                else
                {
                    cout<<"抱歉,密码错误,登陆失败,请重新输入。"<<endl;
                    if(Count==3)
                    {
                        cout<<"密码错误次数过多,系统运行结束。"<<endl;
                        return false;
                    }
                    goto INIT;
                }
                strcat(temp,".txt");
                Init_UFD(temp);
                //            Print_UFD();
                Disk_Head.Start=Max_End;
                return true;
            }
            p=p->next;
        }
        cout<<"用户名不存在,请重新输入。"<<endl;
    }
    //    return false;
}
void Init_AFD()
{
    pafd=(AFD *)new AFD;
    pafd->next=NULL;
}

bool Create_MFD()//创建文件命令
{
    string User_name;
    char User_passwd[30];
    MFD *p = pmfd;
    cout<<"请输入要创建的用户名: ";
    cin>>User_name;
    cout<<"请输入"<<User_name<<"用户的密码：(默认为:admin)";
    getchar();
    cin.getline(User_passwd,30);
    if(User_passwd[0]=='\0')
        strcpy(User_passwd,"admin");
    while(p)
    {
        if(User_name==p->User_name)
        {
            cout<<"此用户名已存在。"<<endl;
            return false;
        }
        if(!p->next)
            break;
        p= p->next;
    }
    p->next = (MFD *)new MFD;  //尾插法
    p=p->next;
    p->User_name=User_name;
    p->User_passwd=User_passwd;
    p->End=0;
    p->next = NULL;
    p->Ufd_next = NULL;
    Max_User--;
    return true;
}

bool Create_UFD()//创建文件命令
{
    string File_name;
    UFD *p = pufd;
    unsigned int Protect;
    int Max_File_length;

    cout<<"请输入要创建的文件名: ";
    cin>>File_name;
    cout<<"请输入要创建的文件保护类型（0/1/2）：";
    cin>>Protect;
    cout<<"请输入文件的最大容量："<<endl;
    cin>>Max_File_length;
    while(p)
    {
        if(File_name==p->File_name)
        {
            cout<<"此文件名已存在。"<<endl;
            return false;
        }
        if(!p->next)
            break;
        p= p->next;
    }
    p->next = (UFD *)new UFD;  //尾插法
    p=p->next;
    p->File_name=File_name;
    p->Max_File_length=Max_File_length;
    p->Start=Disk_Head.Start;
    p->Protect = Protect;
    p->File_length = 0;     //刚创建文件时，文件是空的
    Disk_Head.Start+=Max_File_length;
    p->next = NULL;
    return true;
}

bool Delete_UFD()  //关于主目录的用户文件夹的文件删除，如果文件已经打开了，不能删除
{
    string File_name;
    cout<<"请输入要删除的文件名：";
    cin>>File_name;
    UFD *p=pufd,*temp;
    AFD *pa=pafd;
    while(pa->next)
    {
        if(File_name==pa->next->File_name)
        {
            cout<<"抱歉,该文件已被打开,请先关闭,再进行删除操作。"<<endl;
            return false;
        }
    }
    while(p->next)
    {
        if(File_name==p->next->File_name)
        {
            temp=p->next;
            p->next=p->next->next;  //如果说要回收模拟的磁盘空间的话，应该是回收最大长度
            delete temp;
            cout<<"文件删除成功!!!"<<endl;
            return true;
        }
        p=p->next;
    }
    cout<<"抱歉,要删除的文件不存在。"<<endl;
    return false;
}
bool Open()
{
    string File_name;
    unsigned int Protect;
    cout<<"请输入要打开的文件名：";
    cin>>File_name;
    UFD *p=pufd->next;
    AFD *pa=pafd->next;
    while(pa)
    {
        if(File_name==pa->File_name)
        {
            cout<<"文件"<<File_name<<"已经打开。"<<endl;
            return true;
        }
        if(!pa->next)
            break;
        pa=pa->next;
    }
    if(!pa)     //如果找不到，或者打开的文件目录链表为空表
        pa=pafd;
    while(p)
    {
        if(File_name==p->File_name)
        {
            if(!Max_Open)
            {
                cout<<"抱歉，最多只能打开5个文件,请先关闭其他打开的文件再操作。"<<endl;
                return false;
            }
            if(p->Protect==0)
            {
                cout<<"抱歉，此文件不可执行任何操作。"<<endl;
                return false;
            }
            cout<<"请选择以什么样的方式打开文件<1-只读,2-可读可写>：";
            cin>>Protect;
            pa->next=(AFD *)new AFD;
            pa=pa->next;
            pa->File_name=p->File_name;
            pa->Start=p->Start;
            pa->File_length=p->File_length;
            pa->Max_File_length=p->Max_File_length;
            pa->Protect=Protect;
            if(Protect==1)
                pa->Pointer=0;
            else
                pa->Pointer=p->File_length;
            pa->next=NULL;
            Max_Open--;
            cout<<"文件"<<File_name<<"已经打开。"<<endl;
            return true;
        }
        p=p->next;
    }
    cout<<"抱歉,要打开的文件不存在。"<<endl;
    return false;
}
void Close()
{
    string file_name;
    UFD *pu=pufd->next;
    cout<<"请输入要关闭的文件名：";
    cin>>file_name;
    AFD *p=pafd,*temp;
    while(p&&p->next)
    {
        if(file_name==p->next->File_name)
        {
            temp=p->next;
            p->next=p->next->next;
            if(temp->Protect==2)
            {
                while(pu)
                {
                    if(temp->File_name==pu->File_name)
                    {
                        pu->File_length=temp->Pointer;
                        break;
                    }
                    pu=pu->next;
                }
            }
            delete temp;
            cout<<"文件"<<file_name<<"关闭成功!!!"<<endl;
            return ;
        }
        p=p->next;
    }
    cout<<"抱歉,要关闭的文件没有被打开!!!"<<endl;
}
bool Read_File()  //读写都是通过修改运行文件目录的Pointer去模拟
{
    string File_name;
    unsigned int length;
    AFD *p=pafd->next;
    cout<<"请输入要读的文件名：";
    cin>>File_name;
    cout<<"请输入要读取的长度：";
    cin>>length;
    while(p)
    {
        if(File_name==p->File_name)
        {
            for(int i=p->Start,j=1;j<=length;i++,j++)
                cout<<Disk_Size[i];
            cout<<endl;
            cout<<"文件"<<File_name<<"读取成功"<<endl;
            return true;
        }
        p=p->next;
    }
    cout<<"读取失败,文件没有打开过"<<endl;
    return false;
}
bool Write_File()
{
    string File_name;
    unsigned int length;
    AFD *p=pafd->next;
    char temp[MaxDisk]={'\0'};
    cout<<"请输入要写的文件名：";
    cin>>File_name;
    while(p)
    {
        if(File_name==p->File_name)
        {
            if(p->Protect!=2)
            {
                cout<<"文件"<<File_name<<"不可写"<<endl;
                return false;
            }
            cout<<"请输入要写入的长度："<<endl;
            cin>>length;
            if(p->Pointer+length<=p->Max_File_length)
            {
                cout<<"请写入指定长度的内容：<以回车作为结束符>"<<endl;
                getchar();
                gets(temp);
                for(int i=p->Start+p->File_length,j=1;j<=strlen(temp);i++,j++)
                    Disk_Size[i]=temp[j-1];
                p->Pointer += length;
                cout<<"文件"<<File_name<<"写入成功!!!"<<endl;
            }
            else
            {
                cout<<"欲写入的数据大小发生溢出,已超过文件的分配容量,写入失败。"<<endl;
                return false;
            }
            return true;
        }
        p=p->next;
    }
    cout<<"写入失败,文件没有打开过。"<<endl;
    return false;
}
void Destroy_Space()
{
    MFD *pm=pmfd;
    while(pm)
    {
        pmfd=pmfd->next;
        delete pm;
        pm=pmfd;
    }

    AFD *pa=pafd;
    while(pa)
    {
        pafd=pafd->next;
        delete pa;
        pa=pafd;
    }
    UFD *pu=pufd;
    while(pu)
    {
        pufd=pufd->next;
        delete pu;
        pu=pufd;
    }
}

void Save_MFD()
{
    ofstream ofs;        //文件的输出流对象
    ofs.open("MFD.txt");
    if(!ofs.is_open())
    {
        cout<<"The MFD can't open!!!"<<endl;
        if(flag=='y')
            cout<<"正在保存主目录用户名信息..."<<endl;
        else
            ofs.close();
    }
    MFD *p=pmfd->next;
    while(p)
    {
        if(p->User_name==User_name)
            p->End=Disk_Head.Start;
        ofs<<p->User_name<<" "<<p->User_passwd<<" "<<p->End;
        if(p->next)
            ofs<<"\n";
        p=p->next;
    }
    ofs.close();
}

void Save_UFD(char *name)
{
    ofstream ofs;        //文件的输出流对象
    char temp[MaxDisk]={'\0'};
    ofs.open(name);
    if(!ofs.is_open())
    {
        cout<<"The "<<name<<"文件 can't open!!!"<<endl;
        ofs.close();
    }
    UFD *p=pufd->next;
    while(p)
    {
        memset(temp,'\0',sizeof(temp));
        for(int i=p->Start,j=1;j<=p->Max_File_length;i++,j++)
        {
            if(Disk_Size[i])
                temp[j-1]=Disk_Size[i];
            else
                break;
        }
        if(!temp[0])        //如果文件没有数据的话，就用空格表示此文件没有任何内容
            temp[0]='#';
        ofs<<p->File_name<<" "<<p->Max_File_length<<" "<<p->Protect<<" "<<p->Start<<" "<<p->File_length<<" "<<temp;
        if(p->next)
            ofs<<"\n";
        p=p->next;
    }
    ofs.close();
}

void Quit_System()
{
    AFD *pa=pafd->next;
    UFD *pu=pufd->next;
    while(pa)
    {
        if(pa->Protect==2)  //2表示可写   //1表示只读  //0表示不可操作
        {
            while(pu)
            {
                if(pa->File_name==pu->File_name)
                {
                    pu->File_length=pa->Pointer;
                    break;
                }
                pu=pu->next;
            }
        }
        pa=pa->next;
    }
    char temp[30];
    strcpy(temp,User_name);
    strcat(temp,".txt");
    Save_MFD();
    Save_UFD(temp);
    Print_UFD();
    Destroy_Space();
}

void Print_Help()
{
    system("color 0b");
    cout<<"************************二级文件系统演示*******************"<<endl;
    cout<<"*\t\t 命令                说明                            *"<<endl;
    cout<<"*\t\tlogin              登录系统                         *"<<endl;
    cout<<"*\t\tcreate             创建文件                         *"<<endl;
    cout<<"*\t\tdelete             删除文件                         *"<<endl;
    cout<<"*\t\topen               打开文件                         *"<<endl;
    cout<<"*\t\tclose              关闭文件                         *"<<endl;
    cout<<"*\t\tread               读取文件                         *"<<endl;
    cout<<"*\t\twrite              写入文件                         *"<<endl;
    cout<<"*\t\tls                 显示目录                         *"<<endl;
    cout<<"*\t\trename             重命文件                         *"<<endl;
    cout<<"*\t\thelp               帮助菜单                         *"<<endl;
    cout<<"*\t\tcls                清除屏幕                         *"<<endl;
    cout<<"*\t\tlogout             切换用户                         *"<<endl;
    cout<<"*\t\tquit               退出系统                         *"<<endl;
    cout<<"********************************************************"<<endl;
}
void System_Init()
{
Start:
    Print_Help();
    Init_Disk();
    if(!Init_MFD())
    {
        int num;
        cout<<"主目录还未创建，是否创建<y/n>"<<endl;
        cin>>flag;
        cout<<"请输入欲创建的用户文件夹的个数(1-10)："<<endl;
        cin>>num;
        if(flag=='y')
        {
            while(num--)
            {
                Create_MFD();
                if(!Max_User)
                {
                    cout<<"本系统,最多支持10个用户,创建多余用户失败,请删除其他用户,再做尝试"<<endl;
                    return ;
                }
            }
            Save_MFD();
            cout<<"主目录已经创建完成,用户各自目录已得到保存,是否继续<y/n>"<<endl;
            cin>>flag;
            if(flag=='y')
                goto Start;
        }
    }
    return ;
}


void File_System()
{
    while(1)
    {
        string Command;
        cout<<"请输入命令：";
        cin>>Command;
        if(Command=="create")
            Create_UFD();
        else if(Command=="delete")
            Delete_UFD();
        else if(Command=="open")
            Open();
        else if(Command=="close")
            Close();
        else if(Command=="read")
            Read_File();
        else if(Command=="write")
            Write_File();
        else if(Command=="quit")
        {
            Quit_System();
            break;
        }
        else if(Command=="ls")
            Print_UFD();
        else if(Command=="cls")
            system("cls");
        else if(Command=="login")
        {
            if(!User_Check())
                break;
            Init_AFD();
        }
        else if(Command=="rename")
            Rename_File();
        else if(Command=="logout")
        {
            cout<<"用户登出成功!!!"<<endl;
            Quit_System();
            System_Init();
            File_System();
        }
        else
            Print_Help();
    }
}

int main(int argc, const char * argv[]) {
    System_Init();
    File_System();
    return 0;
}
