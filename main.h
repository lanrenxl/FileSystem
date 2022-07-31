//#pragma comment (lib, "os_design.lib")
#pragma once
#include <stdio.h>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <string>
#include <vector>
#include <map>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "transform.h"

extern "C" {
#include "osCode/dir.h"
#include "osCode/tools.h"
}

using namespace std;
//新建文件或目录时填写的信息
dir *current;
vector<string> currentPath;
u16 currentUserNum = 1;
bool filesChanged = 1;
bool dirsChanged = 1;
bool showSameNameWindow = 0;
bool showErrorPathWindow = 0;
bool showUserNumOutRageWindow = 0;
bool showUserEnableErrorWindow = 0;
bool showRwErrorWindow = 0;
bool showTempNULLWindow = 0;
bool showCopyErrorWindow = 0;


void userNumOutRageWindow()
{
    ImGui::Begin(u8"错误");
    ImGui::Text(u8"\t\t编号超出范围\n\t\t必须小于16");
    ImGui::Text(u8"\t\t"); ImGui::SameLine();
    if (ImGui::Button(u8"更改编号"))
    {
        showUserNumOutRageWindow = 0;
    }
    ImGui::End();
}
void sameNameWindow()
{
    ImGui::Begin(u8"错误");
    ImGui::Text(u8"\t\t发生重名错误");
    ImGui::Text(u8"\t\t"); ImGui::SameLine();
    if (ImGui::Button(u8"更改名字"))
    {
        showSameNameWindow = 0;
    }
    ImGui::End();
}
void errorPathWindow()
{
    ImGui::Begin(u8"路径错误");
    ImGui::Text(u8"\t\t发生路径错误");
    ImGui::Text(u8"\t\t"); ImGui::SameLine();
    if (ImGui::Button(u8"更改路径"))
    {
        showErrorPathWindow = 0;
    }
    ImGui::End();
}
void userEnableErrorWindow()
{
    ImGui::Begin("error");
    ImGui::Text(u8"\t\t用户权限不够");
    ImGui::Text(u8"\t\t"); ImGui::SameLine();
    if (ImGui::Button(u8"更改编号"))
    {
        showUserEnableErrorWindow = 0;
    }
    ImGui::End();
}
void rwError()
{
    ImGui::Begin("error");
    ImGui::Text(u8"\t\t读写权限不够");
    ImGui::Text(u8"\t\t"); ImGui::SameLine();
    if (ImGui::Button(u8"更改编号"))
    {
        showRwErrorWindow = 0;
    }
    ImGui::End();
}
void tempErrorWindow()
{
    ImGui::Begin("error");
    ImGui::Text(u8"\t\t剪切板为空");
    ImGui::Text(u8"\t\t"); ImGui::SameLine();
    if (ImGui::Button(u8"更改编号"))
    {
        showTempNULLWindow = 0;
    }
    ImGui::End();
}
void CopyErrorWindow()
{
    ImGui::Begin(u8"error");
    ImGui::Text(u8"\t\tcopy no completed");
    ImGui::Text(u8"\t\t"); ImGui::SameLine();
    if (ImGui::Button(u8"quit"))
    {
        showCopyErrorWindow = 0;
    }
    ImGui::End();
}

//--------------------------------------------------------------------------------
struct Funcs
{
    static int MyResizeCallback(ImGuiInputTextCallbackData* data)
    {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            ImVector<char>* my_str = (ImVector<char>*)data->UserData;
            IM_ASSERT(my_str->begin() == data->Buf);
            my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
            data->Buf = my_str->begin();
        }
        return 0;
    }
    // Note: Because ImGui:: is a namespace you would typically add your own function into the namespace.
    // For example, you code may declare a function 'ImGui::InputText(const char* label, MyString* my_str)'
    static bool MyInputTextMultiline(const char* label, ImVector<char>* my_str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0)
    {
        IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
        return ImGui::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, Funcs::MyResizeCallback, (void*)my_str);
    }
};
//-------------------------------------------------------------------------------

string space(int len)
{
    string s = "";
    for (int i = 0; i < len; i++)
    {
        s += " ";
    }
    return s;
}

class dirGUI
{
private:
    dir* dp;        //文件控制块指针
    int index;      //dirs中的索引
    bool dirOpen;   //打开操作绑定
    bool showChangeWindow;  //修改操作绑定变量
    bool showConfirmWindow; //确认操作绑定变量
    bool showMoveWindow;    //移动操作绑定变量
    bool choose;            //选项窗口绑定变量
public:
    dirGUI(dir* _dp, int _index):
        dp(_dp), index(_index), dirOpen(0), showChangeWindow(0), showConfirmWindow(0), showMoveWindow(0)
    {}
    void changeWindow()
    {
        static char name[11] = "new dir\0";
        static int nameSize = 8;
        string title = this->dp->name + string(" change window");
        ImGui::SetNextWindowSize(ImVec2(350, 200));
        ImGui::Begin(title.c_str());

        //更改文件属性
        ImGui::InputText("name", name, 11);
        ImGui::InputInt("size", &nameSize);
        ImGui::Dummy(ImVec2(10, 20));
        ImGui::Text("\t"); ImGui::SameLine();
        if (ImGui::Button(u8"确认更改"))
        {
            if (findDir(current, name, nameSize) != NULL)
                showSameNameWindow = 1;
            else
            {
                dirRename(dp, name, nameSize);
                dirsChanged = 1;
                this->showChangeWindow = false;
            }
            
        }ImGui::SameLine();
        ImGui::Text(space(4).c_str()); ImGui::SameLine();
        if (ImGui::Button(u8"取消"))
        {
            this->showChangeWindow = false;
        }
        ImGui::End();
    }
    void confirmWindow()
    {
        string title = this->dp->name + string(" confirm window");
        ImGui::SetNextWindowSize(ImVec2(220, 100));
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::Begin(title.c_str());
        ImGui::Dummy(ImVec2(10, 10));
        if (ImGui::Button(u8"确认"))
        {
            this->showConfirmWindow = false;
            dirDelete(dp);
            dirsChanged = 1;
        }ImGui::SameLine();
        ImGui::Text(space(4).c_str()); ImGui::SameLine();
        if (ImGui::Button(u8"取消"))
        {
            this->showConfirmWindow = false;
        }
        ImGui::End();
    }
    void chooseWindow()
    {
        ImGui::Begin("d");
        string lopen = (u8"目录打开" + to_string(index) + "       ");
        if (ImGui::Button(lopen.c_str()))  //文件可以打开修改 目录则进入该目录
        {
            this->dirOpen = true;
        }
        string lchange = (u8"目录修改" + to_string(index) + "       ");
        if (ImGui::Button(lchange.c_str()))  //展示属性并可以修改
        {
            this->showChangeWindow = true;
        }
        string lDel = (u8"目录删除" + to_string(index) + "       ");
        if (ImGui::Button(lDel.c_str()))  //确认窗口
        {
            this->showConfirmWindow = true;
        }
        string lMov = (u8"目录剪切" + to_string(index) + "       ");
        if (ImGui::Button(lMov.c_str()))  //移动窗口
        {
            if (dirMovePart1(dp))
                dirsChanged = 1;
            else
                showCopyErrorWindow = 1;
        }
        string lcreate = (u8"目录快捷方式" + to_string(index));
        if (ImGui::Button(lcreate.c_str()))
        {
            char lastName[5] = "sc\0";
            if (findFile(current, dp->name, lastName) == NULL)
            {
                fcb* p = fileCreate(current, dp->name, dp->nameLen, lastName, 3, 1, 0, 1);
                char* data = new char[p->FileSize];
                fileReadAll(p, data);
                string sPath = "";
                for (int i = 0; i < currentPath.size(); i++)
                {
                    sPath += currentPath[i];
                }
                sPath += "\0";
                char* wData = new char[sPath.size()];
                for (int i = 0; i < sPath.size(); i++)
                {
                    wData[i] = sPath[i];
                }
                fileWriteAll(p, wData, sPath.size());   //刷写回磁盘
                filesChanged = 1;
                delete data;
                delete wData;
            }
            else
            {
                showSameNameWindow = 1;
            }
        }
        if (ImGui::Button(u8"取消                "))
        {
            choose = 0;
        }
        ImGui::End();
    }
    void show()
    {
        ImGui::Spacing();
        string time = space(8) + to_string(dp->createTime_year+1900) + "/" + to_string(dp->createTime[0]+1) + "/" + to_string(dp->createTime[1]) + " " + to_string(dp->createTime[2]) + ":" + to_string(dp->createTime[3]) + space(8);
        ImGui::Separator();
        ImGui::Dummy(ImVec2(10, 5));
        ImGui::Text(u8"目录"); ImGui::SameLine();
        int spaceW = 0;
        spaceW = (8 - this->dp->nameLen) * 1.5 + 38;    //计算空格长度
        if (spaceW < 0) spaceW = 38;
        ImGui::Text((space(12)+ dp->name +space(spaceW)).c_str());ImGui::SameLine();
        ImGui::Text(time.c_str()); ImGui::SameLine();
        if (ImGui::Button((u8"D选项" + to_string(index)).c_str()))
        {
            ImGui::SetNextWindowPos(ImGui::GetMousePos());
            choose = 1;
        }
        ImGui::Dummy(ImVec2(10, 5));
        if (this->choose)
        {
            chooseWindow();
        }
        if (this->dirOpen)
        {
            //打开目录操作
            openDir(dp);
            currentPath.push_back(string(dp->name) + "/");
            current = dp;
            dirOpen = 0;
            //dirs更改
            dirsChanged = 1;
            filesChanged = 1;
        }
        if (this->showChangeWindow) //更改窗口
        {
            changeWindow();
        }
        if (this->showConfirmWindow)    //确认窗口
        {
            confirmWindow();
        }
    }
};

class fileGUI
{
private:
    fcb* fp;
    int index;
    bool showFileEditWindow;
    bool showChangeWindow;
    bool showConfirmWindow;
    bool showMoveWindow;
    bool choose;
    bool chooseS;
    //fcb指针一个
public:
    fileGUI(fcb* _fp, int _index) :
        fp(_fp), index(_index), showFileEditWindow(0), showChangeWindow(0), showConfirmWindow(0), showMoveWindow(0)
    {
    }
    void show()
    {
        if (fp->isShortCut)
        {
            ImGui::Spacing();
            string time = space(27) + to_string(fp->createTime_year + 1900) + "/" + to_string(fp->createTime[0] + 1) + "/" + to_string(fp->createTime[1]) + " " + to_string(fp->createTime[2]) + ":" + to_string(fp->createTime[3]) + space(8);
            int num = log2(fp->UserEnable);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(10, 5));
            ImGui::Text(u8"快捷方式"); ImGui::SameLine();
            int spaceW = 6;
            spaceW = (9 - this->fp->nameLen) * 2 + 6;
            if (spaceW < 0) spaceW = 6;
            ImGui::Text((space(5) + fp->name + space(spaceW)).c_str()); ImGui::SameLine();
            ImGui::Text((space(4) + fp->last + space(4)).c_str()); ImGui::SameLine();
            ImGui::Text(time.c_str()); ImGui::SameLine();

            if (ImGui::Button((u8"选项s" + to_string(index)).c_str()))
            {
                ImGui::SetNextWindowPos(ImGui::GetMousePos());
                chooseS = 1;
            }
            ImGui::Dummy(ImVec2(10, 5));
            if (this->chooseS)
            {
                chooseSWindow();
            }
            if (this->showConfirmWindow)
            {
                confirmWindow();
            }
        }
        else
        {
            ImGui::Spacing();
            string time = space(8) + to_string(fp->createTime_year + 1900) + "/" + to_string(fp->createTime[0] + 1) + "/" + to_string(fp->createTime[1]) + " " + to_string(fp->createTime[2]) + ":" + to_string(fp->createTime[3]) + space(8);
            int num = log2(fp->UserEnable);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(10, 5));
            ImGui::Text(u8"文件"); ImGui::SameLine();
            int spaceW = 6;
            spaceW = (9 - this->fp->nameLen) * 2 + 6;
            if (spaceW < 0) spaceW = 6;
            ImGui::Text((space(12) + fp->name + space(spaceW)).c_str()); ImGui::SameLine();
            ImGui::Text((space(4) + fp->last + space(4)).c_str()); ImGui::SameLine();
            ImGui::Text((u8"用户编号" + to_string(num)).c_str()); ImGui::SameLine();
            ImGui::Text(time.c_str()); ImGui::SameLine();
            if (ImGui::Button((u8"选项s" + to_string(index)).c_str()))
            {
                ImGui::SetNextWindowPos(ImGui::GetMousePos());
                choose = 1;
            }
            ImGui::Dummy(ImVec2(10, 5));
            if (this->choose)
            {
                chooseWindow();
            }
            if (this->showFileEditWindow)
            {
                editWindow();
            }
            if (this->showChangeWindow)
            {
                changeWindow();
            }
            if (this->showConfirmWindow)
            {
                confirmWindow();
            }
        }
    }
    void chooseSWindow()
    {
        ImGui::Begin("s");
        string lopen = (u8"快捷方式打开" + to_string(index));
        if (ImGui::Button(lopen.c_str()))
        {
            chooseS = 1;
            dir* tmp;
            int len = fp->FileSize;
            char* data = new char[len];
            fileReadAll(fp, data);
            fileWriteAll(fp, data, len);

            return2Root(current);
            currentPath.clear();
            currentPath.push_back("/root/");
            current = root;
            tmp = goToDir(data, len, current);
            while (tmp != NULL)
            {
                current = tmp;
                currentPath.push_back(string(current->name) + "/");
                tmp = goToDir(data, len, current);
            }
            filesChanged = 1;
            dirsChanged = 1;
            free(data);
        }
        string lDel = (u8"快捷方式删除" + to_string(index));
        if (ImGui::Button(lDel.c_str()))  //确认窗口
        {
            chooseS = 1;
            this->showConfirmWindow = true;
        }
        string lmov = (u8"快捷方式剪切" + to_string(index));
        if (ImGui::Button(lmov.c_str()))  //确认窗口
        {
            chooseS = 1;
            //未粘贴完成
            if (fileMovePart1(fp))
                filesChanged = 1;
            else
                showCopyErrorWindow = 1;
        }
        if (ImGui::Button(u8"取消                 "))
        {
            chooseS = 0;
        }
        ImGui::End();
    }
    void chooseWindow()
    {
        ImGui::Begin("f");
        string lopen = (u8"文件打开" + to_string(index) + "       ");
        if (ImGui::Button(lopen.c_str()))  //文件可以打开修改 目录则进入该目录
        {
            choose = 0;
            if (fp->UserEnable != currentUserNum)
            {
                showUserEnableErrorWindow = 1;
            }
            else if (fp->rw == 0)
            {
                showRwErrorWindow = 1;
            }
            else
            {
                this->showFileEditWindow = true;
            }
        }
        string lchange = (u8"文件修改" + to_string(index) + "       ");
        if (ImGui::Button(lchange.c_str()))  //展示属性并可以修改
        {
            choose = 0;
            if (fp->UserEnable != currentUserNum)
            {
                showUserEnableErrorWindow = 1;
            }
            else
            {
                this->showChangeWindow = true;
            }
        }
        string lDel = (u8"文件删除" + to_string(index) + "       ");
        if (ImGui::Button(lDel.c_str()))  //确认窗口
        {
            choose = 0;
            if (fp->UserEnable != currentUserNum)
            {
                showUserEnableErrorWindow = 1;
            }
            else
            {
                this->showConfirmWindow = true;
            }
        }
        string lMov = (u8"文件剪切" + to_string(index) + "       ");
        if (ImGui::Button(lMov.c_str()))  //移动窗口
        {
            choose = 0;
            if (fp->UserEnable != currentUserNum)
            {
                showUserEnableErrorWindow = 1;
            }
            else
            {
                //未粘贴完成
                if (fileMovePart1(fp))
                    filesChanged = 1;
                else
                    showCopyErrorWindow = 1;
            }
        }
        string lCreate = (u8"创建快捷方式" + to_string(index));
        if (ImGui::Button(lCreate.c_str()))
        {
            choose = 0;
            char lastName[5] = "sc\0";
            //防止重命名
            if (findFile(current, fp->name, lastName) == NULL)
            {
                fcb* p = fileCreate(current, fp->name, fp->nameLen, lastName, 3, fp->rw, fp->UserEnable, 1);
                char* data = new char[p->FileSize];
                fileReadAll(p, data);
                string sPath = "/";
                for (int i = 1; i < currentPath.size(); i++)
                {
                    sPath += currentPath[i];
                }
                sPath += '\0';
                char* wData = new char[sPath.size()];
                for (int i = 0; i < sPath.size(); i++)
                {
                    wData[i] = sPath[i];
                }
                fileWriteAll(p, wData, sPath.size());   //刷写回磁盘
                filesChanged = 1;
                delete data;
                delete wData;
            }
            else
            {
                showSameNameWindow = 1;
            }
        }
        if (ImGui::Button(u8"取消                 "))
        {
            choose = 0;
        }
        ImGui::End();
    }
    void editWindow()
    {
        static ImVector<char> my_str;
        static bool flag = 0;
        static char* text;
        int RealSize = fp->FileSize;
        if (flag == 0)  //新进的一个文件
        {
            my_str.clear();
            RealSize = fp->FileSize;
            text = new char[RealSize];
            fileReadAll(fp, text);
            for (int i = 0; i < RealSize; i++)
            {
                my_str.push_back(text[i]);
            }
            flag = 1;
        }
        string title = this->fp->name + string(" edit window");
        ImGui::SetNextWindowSize(ImVec2(450, 450));
        ImGui::Begin(title.c_str());
        if (my_str.empty())
        {
            my_str.push_back(0);
        }
        Funcs::MyInputTextMultiline("##MyStr", &my_str, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
        //ImGui::Text("Data: %p Size: %d Capacity: %d", (void*)my_str.begin(), my_str.size(), my_str.capacity());
        if (ImGui::Button(u8"确认保存"))
        {
            this->showFileEditWindow = false;               //终止渲染
            fileWriteAll(fp, my_str.Data, my_str.size());   //刷写回磁盘
            flag = 0;   //文件已退出标志
        }ImGui::SameLine();
        ImGui::Text("\t"); ImGui::SameLine();
        if (ImGui::Button(u8"取消"))
        {
            this->showFileEditWindow = false;   //终止渲染
            fileWriteAll(fp, text, RealSize);   //关闭文件 释放所占内存
            flag = 0;   //文件已退出标志
        }
        ImGui::End();
    }
    void changeWindow()
    {
        static char name[11] = "";
        static int nameLen = 0;
        static char lastName[5] = "";
        static int lastLen = 0;
        static int rw = 1;
        static int userEnable = 1;
        
        string title = this->fp->name + string(" change window");
        ImGui::SetNextWindowSize(ImVec2(350, 300));
        ImGui::Begin(title.c_str());
        
        //更改文件属性
        ImGui::InputText("name", name, 11);
        ImGui::InputInt("nameLen", &nameLen);
        ImGui::InputText("lastName", lastName, 5);
        ImGui::InputInt("lastLen", &lastLen);
        ImGui::InputInt("rw", &rw);
        ImGui::InputInt("userEnable", &userEnable);
        ImGui::Dummy(ImVec2(10, 20));
        ImGui::Text("\t"); ImGui::SameLine();
        if (ImGui::Button(u8"确认更改"))
        {
            //文件重命名
            if (userEnable > 15)
            {
                showUserEnableErrorWindow = 1;
            }
            else
            {
                if(findFile(current, name, lastName) != NULL)
                    showSameNameWindow = 1;
                else
                {
                    userEnable = 1 << userEnable;
                    fileRename(fp, name, nameLen, lastName, lastLen);
                    fileReset(fp, rw, userEnable);
                    filesChanged = 1;   //文件更改 重新渲染列表
                    this->showChangeWindow = false; //终止渲染
                }
            }
            
        }ImGui::SameLine();
        if (showUserEnableErrorWindow)
        {
            userEnableErrorWindow();
        }
        ImGui::Text(space(4).c_str()); ImGui::SameLine();
        if (ImGui::Button(u8"取消"))
        {
            this->showChangeWindow = false;
        }
        ImGui::End();
    }
    void confirmWindow()    //删除文件确认
    {
        string title = this->fp->name + string(" confirm window");
        ImGui::SetNextWindowSize(ImVec2(220, 100));
        ImGui::SetNextWindowPos(ImVec2(850, 450));
        ImGui::Begin(title.c_str());
        ImGui::Dummy(ImVec2(10, 10));
        if (ImGui::Button(u8"确认"))
        {
            //终止渲染
            this->showConfirmWindow = false;
            //删除文件
            fileDelete(fp);
            //文件块发生更改 需要重新渲染
            filesChanged = 1;
        }ImGui::SameLine();
        ImGui::Text(space(4).c_str()); ImGui::SameLine();
        if (ImGui::Button(u8"取消"))
        {
            this->showConfirmWindow = false;
        }
        ImGui::End();
    }
};

vector<fileGUI> files = {};
vector<dirGUI> dirs = {};

class menu
{
private:
    bool isOpen;
    bool showNewFile;
    bool showNewDir;
    bool showChangeUser;
public:
    menu():isOpen(false), showNewDir(false), showNewFile(false), showChangeUser(false)
    {}
    void newFileWindow()    //完成
    {
        static char name[11] = "new file\0";
        static int nameSize = 9;
        static char lastName[5] = "txt\0";
        static int lastSize = 4;
        ImGui::SetNextWindowSize(ImVec2(350, 250));
        ImGui::Begin(u8"创建新文件");
        ImGui::InputText("name", name, 11);
        ImGui::InputInt("size", &nameSize);
        ImGui::InputText("last", lastName, 5);
        ImGui::InputInt("lastSize", &lastSize);
        ImGui::Dummy(ImVec2(10, 20));
        ImGui::Text("\t"); ImGui::SameLine();
        if (ImGui::Button(u8"确认文件"))
        {
            //防止重命名
            if (findFile(current, name, lastName) == NULL)
            {   
                fileCreate(current, name, nameSize, lastName, lastSize, 1, currentUserNum, 0);
                filesChanged = 1;
                dirsChanged = 1;
                this->showNewFile = false;
            }
            else
            {
                showSameNameWindow = 1;
            }
        }ImGui::SameLine();
        ImGui::Text(space(4).c_str()); ImGui::SameLine();
        if (ImGui::Button(u8"取消"))
        {
            this->showNewFile = false;
        }
        ImGui::End();
    }
    void newDirWindow() //完成
    {
        static char dirName[11] = "new dir\0";
        static int dirSize = 8;
        int i = 0;
        ImGui::SetNextWindowSize(ImVec2(350, 200));
        ImGui::Begin(u8"创建新目录");

        ImGui::InputText("name", dirName, 11);
        ImGui::InputInt("size", &dirSize);
        ImGui::Dummy(ImVec2(10, 20));
        ImGui::Text("\t"); ImGui::SameLine();
        if (ImGui::Button(u8"确认目录"))
        {
            //找到\0即字符串大小
            for (i = 0; i < 11; i++)
            {
                if (dirName[i] == '\0')
                    break;
            }
            //防止重命名
            if (findDir(current, dirName, i + 1) == NULL)
            {
                dirCreate(current, dirName, dirSize);
                dirsChanged = 1;
                filesChanged = 1;
                this->showNewDir = false;
            }
            else
            {
                showSameNameWindow = 1;
            }
        }ImGui::SameLine();
        if (showSameNameWindow == 1)
        {
            sameNameWindow();
        }
        ImGui::Text(space(4).c_str()); ImGui::SameLine();
        if (ImGui::Button(u8"取消"))
        {
            this->showNewDir = false;
        }
        ImGui::End();
    }
    void changeUser()
    {
        static int userNum = 1;
        ImGui::SetNextWindowSize(ImVec2(350, 200));
        ImGui::Begin(u8"更改用户");
        ImGui::InputInt("size", &userNum);
        ImGui::Dummy(ImVec2(10, 20));
        ImGui::Text("\t"); ImGui::SameLine();
        if (ImGui::Button(u8"确认编号"))
        {
            if (userNum > 15)
            {
                showUserNumOutRageWindow = 1;
            }
            else
            {
                currentUserNum = (1 << userNum);
                this->showChangeUser = false;
            }
        }ImGui::SameLine();
        if (showUserNumOutRageWindow)
        {
            userNumOutRageWindow();
        }
        ImGui::Text(space(4).c_str()); ImGui::SameLine();
        if (ImGui::Button(u8"取消更改"))
        {
            this->showChangeUser = false;
        }
        ImGui::End();
    }
    void show()
    {
        if (filesChanged)   //当filesChanged时, 刷新files数组的内容
        {
            files.clear();
            fcb* f = current->fcbHead;
            while (f != NULL)
            {
                fileGUI fl(f, files.size());    //创建新的对象
                files.push_back(fl);
                f = f->nextFcb;
            }
            filesChanged = 0;   //下次不再刷新
        }
        if (dirsChanged)    //同上刷新dirs的内容
        {
            dirs = {};
            dir* d = current->dirHead;
            while (d != NULL)
            {
                dirGUI dt(d, dirs.size());
                dirs.push_back(dt);
                d = d->nextDir;
            }
            dirsChanged = 0;
        }
        ImGui::SetNextWindowPos(ImVec2(200, 100));
        ImGui::Begin("file system");
        if (ImGui::Button(u8"返回"))
        {
            closeDir(current);
            if (current == root)
            {
                exit(0);
            }
            current = current->parent;
            currentPath.pop_back();
            dirsChanged = 1;
            filesChanged = 1;
        }ImGui::SameLine();
        string sPath = space(2);
        for (int i = 0; i < currentPath.size(); i++)
        {
            sPath += currentPath[i];
        }
        sPath += space(2);
        ImGui::Text(sPath.c_str()); ImGui::SameLine();
        int num = log2(currentUserNum);
        ImGui::Text(("\t用户编号"+to_string(num)+"\t").c_str()); ImGui::SameLine();
        ImGui::Text("\t\t\t\t\t\t\t"); ImGui::SameLine();
        if (ImGui::Button(u8"新建文件"))
        {
            this->showNewFile = 1;  //更新绑定变量
        }ImGui::SameLine();
        if (ImGui::Button(u8"新建目录"))
        {
            this->showNewDir = 1;   //更新绑定变量
        }ImGui::SameLine();
        if (ImGui::Button(u8"切换用户"))
        {
            this->showChangeUser = 1;   //更新绑定变量
        }ImGui::SameLine();
        if (ImGui::Button(u8"粘贴到目录"))
        {
            if (tmpFcb != NULL)
            {
                if(fileMovePart2(current) == 1)
                    filesChanged = 1;   //需要刷新队列
            }
            else if (tmpDir != NULL)
            {
                if (dirMovePart2(current) == 1)
                    dirsChanged = 1;    //刷新队列变量
            }
            else
            {
                showTempNULLWindow = 1; //剪切板为空
            }
        }
        //根据绑定的变量进行指定的操作
        if (this->showNewDir)
        {
            newDirWindow();
        }
        if (this->showNewFile)
        {
            newFileWindow();
        }
        if (this->showChangeUser)
        {
            changeUser();
        }
        if (showUserEnableErrorWindow)
        {
            userEnableErrorWindow();
        }
        if (showRwErrorWindow)
        {
            rwError();
        }
        if (showTempNULLWindow)
        {
            tempErrorWindow();
        }
        if (showCopyErrorWindow)
        {
            CopyErrorWindow();
        }
        if (showSameNameWindow == 1)
        {
            sameNameWindow();
        }
        //磁盘使用量的显示
        float progress = (float)usedBlockNum/10000.0f, progress_dir = 1.0f;
        // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
        // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text(u8"磁盘使用率");
        float progress_saturated = ((progress) < (0.0f) ? (0.0f) : (progress) > (1.0f) ? (1.0f) : (progress));
        char buf[32];

        string s = to_string((int)(progress_saturated * 10000)) + "/" + "10000";
        for (int i = 0; i < s.size(); i++)
        {
            buf[i] = s[i];
            buf[i + 1] = '\0';
        }
        ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text(u8"磁盘使用量");

        //渲染fileGUI和dirGUI
        for (int i = 0; i < files.size(); i++)
        {
            files[i].show();
        }
        for (int i = 0; i < dirs.size(); i++)
        {
            dirs[i].show();
        }
        ImGui::End();
    }
};