#pragma once

template<class T>
class Win{
public:
    virtual ~Win() {}
    virtual void initialize(T* thePlayer, int theNumberOfPlayer) = 0; // 生成赢者树
    virtual int winner() const = 0; // 返回赢者
    virtual void play(int thePlayerId, int thePlayer) = 0; // 改变thePlayerId的成绩之后重赛
};