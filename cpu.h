#pragma once
#include <array>
using namespace std;

typedef unsigned int uint;

const uint registerNum=4;
const uint dataSize=100;
const uint instructionSize=100;
const uint maxInstruNum=2;

struct instruction
{
    uint op;
    array<int,maxInstruNum> addressCode;
};

class VM
{
private:
    array<instruction,instructionSize> instructionCache;
    uint nowInstruNum;
    array<int,dataSize> dataArea; //存储器数据区，数据类型这里设为int
    int generalRegister[registerNum]; //通用寄存器（CPU存储器）
    //非通用寄存器
    instruction IR; //目前执行instructionCache到的对象
    uint PC; //目前执行instructionCache到的位置（下标）
    uint AR; //地址寄存器（存下标）
    int DR; //数据寄存器（存值）

    //通用操作
    void decoding()
    {
        if(IR.op==0) //译码
            this->mov(); //执行周期
        else if(IR.op==1)
            this->lad();
    }

    //所有指令函数
    void mov() //由源寄存器向目标寄存器传送字
    {
        //0:目标寄存器 1:源寄存器
        this->DR=this->generalRegister[IR.addressCode[1]];
        this->generalRegister[IR.addressCode[0]]=this->DR;
    }

    void lad() //由存储器向CPU传送字
    {
        //0:存储器地址码 1:目标寄存器
        this->AR=IR.addressCode[0];
        this->DR=this->dataArea[this->AR];
        this->generalRegister[IR.addressCode[1]]=this->DR;
    }

    void add() //计算两个操作数的和
    {
        //0:寄存器1 1:寄存器2
        this->DR=this->generalRegister[IR.addressCode[0]]+this->generalRegister[IR.addressCode[1]];
        this->generalRegister[IR.addressCode[1]]=this->DR;
    }

    void sto() //由CPU向存储器传输字
    {
        //0:存储存储器地址码的寄存器 1:源寄存器
        this->AR=this->generalRegister[IR.addressCode[0]]; //warn:若模拟总线结构，到AR和DR的传输都是通过DBUS
        this->DR=this->generalRegister[IR.addressCode[1]];
        this->dataArea[this->AR]=this->DR;
    }

    void jmp() //以指定地址装入PC
    {
        //0:寄存器
        //warn:教材上写的是直接PC<-IR，但这显然不可能
        this->AR=this->generalRegister[IR.addressCode[0]]; //应该是取寄存器里面内容放IR里，因为IR在模拟时是指令对象，所以这里放AR里
        this->PC=this->AR;
    }

public:
    VM(array<instruction,instructionSize> instructionCache, uint nowInstruNum=0)
    {
        this->setInstructionCache(instructionCache, nowInstruNum);
    }

    void setInstructionCache(array<instruction,instructionSize> instructionCache, uint nowInstruNum)
    {
        this->nowInstruNum=nowInstruNum;
        this->instructionCache=instructionCache;
    }

    void setDataArea(array<int,dataSize> dataArea) { this->dataArea=dataArea; }

    void nextInstru()
    {
        if(this->PC==instructionSize)
            return;
        else
        {
            //取指周期
            this->IR=this->instructionCache[PC];
            this->PC++;
            this->decoding();
        }
    }
};
