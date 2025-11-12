#ifndef Vecto_h
#define Vecto_h
template <typename T>
class Vecto {
    private:
        T *data;
        int maxsize;
        int currentsize;
    public:
        Vecto (int s) {
            maxsize = s;
            currentsize = 0;
            data = new T [s];
        }
        ~Vecto () {
            delete [] data;
        }
        bool push_back (T x) {
            if (currentsize < maxsize) {
                data[currentsize++] = x;
                return 1;
            }
            return 0;
        }
        bool pop_back (T &x) {
            if (currentsize > 0) {
                x = data[--currentsize];
                return 1;
            }
            return 0;
        }
};
#endif