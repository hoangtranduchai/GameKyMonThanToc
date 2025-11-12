#ifndef Stack_h
#define Stack_h
template <typename T>
class Stack {
    private:
        T *data;
        int maxsize;
        int top;
    public:
        Stack(int s) {
            maxsize = s;
            top = -1;
            data = new T [s];
        }
        ~Stack(){
            delete [] data;
        }
        bool Push(T x){
            if (!isFull()) {
                data[++top] = x;
                return 1;
            }
            return 0;
        }
        bool Pop(T &x){
            if (!isEmpty()) {
                x = data[top--];
                return 1;
            }
            return 0;
        }
        bool isFull(){
            return top == maxsize-1;
        }
        bool isEmpty(){
            return top == -1;
        }
};
#endif