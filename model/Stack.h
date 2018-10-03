//
// Created by kuusri on 30/09/2018.
//

#ifndef UNTITLED_STACK_H
#define UNTITLED_STACK_H


template <typename T>
class Stack {

    short top = -1;
    int maxSize;
    T *stack;

public:

    explicit Stack(int maxSize);

    int size();
    void push(T t);
    T pop();

    T get(int i);
};

template<typename T>
Stack<T>::Stack(int maxSize):maxSize(maxSize) {
    this->stack = new T[maxSize];
}

template<typename T>
int Stack<T>::size() {
    return this->top + 1;
}

template<typename T>
void Stack<T>::push(T t) {
    this->stack[++top] = t;
}

template<typename T>
T Stack<T>::pop() {
    return this->stack[top--];
}

template<typename T>
T Stack<T>::get(int i) {
    return stack[i];
}



#endif //UNTITLED_STACK_H
