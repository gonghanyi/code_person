#pragma once

#include <cmath>
#include "winner.h"
#include "iostream"

template<typename T>
class WinnerTree : public Win<T> {
public:
    WinnerTree(T* thePlayer, int theNumberOfPlayers) {
        tree_ = nullptr;
        initialize(thePlayer, theNumberOfPlayers);
    }

    ~WinnerTree() {
        std::cout << "~WinnerTree, tree_ = " << tree_ << std::endl;
        delete [] tree_;
    }
    void initialize(T* player, int playerNum);
    T winner() const {    // 最终的胜利者
        return tree_[1];
    }
    T winner(int i) const {     // 第 i 场比赛的胜利者
        return (i < number_of_players) ? tree_[i] : 0; 
    }
    void play(int thePlayerId, int thePlayer);
    void output(std::ostream& out) const;
protected:
    int tree_size_;
    T* tree_;                       // 赢者树
    int number_of_players;          // 参赛人数
    T* palyer;                      // 比赛原始数组
    void rePlay(int compete, int left, int right);
};

template <typename T>
void WinnerTree<T>::rePlay(int compete, int left, int right) {
    for (int i = compete; i > 0; i /= 2) {
        tree_[i] = tree_[2*i] > tree_[2*i+1] ? tree_[2 * i] : tree_[2*i+1];
    }
}