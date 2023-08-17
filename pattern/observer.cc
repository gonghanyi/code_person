#include "observer.h"

void ConcreteSubject::Attach(const std::shared_ptr<Observer> pObserver) {
    auto iter = std::find(m_pObserver_list.begin(), m_pObserver_list.end(), pObserver);
    if (iter == m_pObserver_list.end()) {
        std::cout << "Attach observer" << pObserver->name() << std::endl;
        m_pObserver_list.emplace_back(pObserver);
    }
}

void ConcreteSubject::Detach(const std::shared_ptr<Observer> pObserver) {
    std::cout << "Detach observer" << pObserver->name() << std::endl;
    m_pObserver_list.remove(pObserver);
}

// 循环通知所有观察者
void ConcreteSubject::Notify() {
    auto iter = m_pObserver_list.begin();
    while (iter != m_pObserver_list.end()) {
        (*iter++)->Update();
    }
}

void Observer1::Update() {
    std::cout << "Observer1(" << m_strName << ") get the update.New state is: "
              << m_pSubject->GetState() << std::endl;
}

void Observer2::Update() {
    std::cout << "Observer2(" << m_strName << ") get the update.New state is: "
              << m_pSubject->GetState() << std::endl;
}
