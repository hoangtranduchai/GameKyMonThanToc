#ifndef Queue_h
#define Queue_h
template <typename T>
class Queue {
    private:
        T *data;
        int maxsize;
        int front;
        int back;
    public:
        Queue (int s) {
            maxsize = s;
            front = -1;
            back = 0;
            data = new T [s];
        }
        ~Queue () {
            detele [] data;
        }
        bool Isfull () {
            return back == maxsize - 1;
        }
        bool Isempty () {
            return front + 1 == back;
        }
        bool Push (T x) {
            if (!Isfull) {
                data[++back] = x;
                return 1;
            }
            return 0;
        }
        bool Pop (T &x) {
            if (!Isempty) {
                x = data[back--];
                return 1;
            }
            return 0;
        }
};
#endif