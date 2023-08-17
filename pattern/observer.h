#pragma once

#include <bits/stdc++.h>

//
// 观察者模式
//
class Observer;

// 抽象被观察者
class Subject {
public:
    Subject() : m_nState(0) {}
    virtual ~Subject() = default;
    virtual void Attach(const std::shared_ptr<Observer> pObserver) = 0;
    virtual void Detach(const std::shared_ptr<Observer> pObserver) = 0;
    virtual void Notify() = 0;
    virtual int GetState() { return m_nState; }
    void SetState(int state) {
        std::cout << "Subject updated!" << std::endl;
        m_nState = state;
    }

protected:
    std::list<std::shared_ptr<Observer>> m_pObserver_list;
    int m_nState;
};

// 抽象观察者
class Observer {
public:
    virtual ~Observer() = default;

    Observer(const std::shared_ptr<Subject> pSubject, const std::string& name = "unknown")
        : m_pSubject(pSubject), m_strName(name) {}
    virtual void Update() = 0;
    virtual const std::string& name() { return m_strName; }

protected:
    std::shared_ptr<Subject> m_pSubject;
    std::string m_strName;
};

// 具体被观察者
class ConcreteSubject : public Subject {
public:
    void Attach(const std::shared_ptr<Observer> pObserver) override;
    void Detach(const std::shared_ptr<Observer> pObserver) override;
    // 循环通知所有观察者
    void Notify() override;
};

//具体观察者1
class Observer1 : public Observer {
public:
    Observer1(const std::shared_ptr<Subject> pSubject, const std::string& name = "unknown")
        : Observer(pSubject, name) {}
    void Update() override;
};

//具体观察者2
class Observer2 : public Observer {
public:
    Observer2(const std::shared_ptr<Subject> pSubject, const std::string& name = "unknown")
        : Observer(pSubject, name) {}
    void Update() override;
};
