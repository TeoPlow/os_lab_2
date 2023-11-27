#include <iostream>
#include <cmath>
#include <pthread.h>
#include <vector>
#include <cstdint>
#include <chrono>

struct Point {
    int x, y, z;
};

class Triangle {
private:
    float max_S = 0;
    unsigned int pt_count;
    unsigned int thread_count;
    int** array;
    Point p[3];
public:
    unsigned int thread_num = 0;

    Triangle (unsigned int pt_count, int thread_count) {
        this->pt_count = pt_count;
        this->thread_count = thread_count;

        /* Add array */
        array = new int* [pt_count];
        for (unsigned int i = 0; i < pt_count; ++i) {
            array[i] = new int [3];
        }

        for (unsigned int i = 0; i < pt_count; i++) {
            for (unsigned int j = 0; j < 3; j++) { 
                std::cin >> array[i][j]; // Исправить 
            }
        }
    }

    /* Get triangle area */
    void square(float& S) {
        int opr_matr1 = (1 * p[1].x * p[2].y) + 
            (p[1].y * p[0].x * 1) +
            (p[2].x * p[0].y * 1) -
            (p[0].y * p[1].x * 1) - 
            (p[1].y * p[2].x * 1) - 
            (p[2].y * p[0].x * 1);
            
        int opr_matr2 = (1 * p[1].x * p[2].z) + 
            (p[1].z * p[0].x * 1) +
            (p[2].x * p[0].z * 1) -
            (p[0].z * p[1].x * 1) - 
            (p[1].z * p[2].x * 1) - 
            (p[2].z * p[0].x * 1);

        int opr_matr3 = (1 * p[1].y * p[2].z) + 
            (p[1].z * p[0].y * 1) +
            (p[2].y * p[0].z * 1) -
            (p[0].z * p[1].y * 1) - 
            (p[1].z * p[2].y * 1) - 
            (p[2].z * p[0].y * 1);
        S = 0.5 * pow(abs(pow(opr_matr1,2) + pow(opr_matr2,2) + pow(opr_matr3,2)), 0.5);
    }

    void add_point(int i) {
        p[i%3].x = array[i][0];
        p[i%3].y = array[i][1];
        p[i%3].z = array[i][2];
    }

    ~Triangle () {
        /* Delete array */
        for (unsigned int i = 0; i < pt_count; ++i) {
            delete[] array[i];
        }
        delete[] array;
    }

    friend void * perebor(void *a);
};

/* Division with rounding up */
int div_up(int x, int y)
{
    return (x + y - 1) / y;
}

/* Function for working with threads */
void * perebor(void *a) {
    float S = 0;
    float max_S = 0;
    Triangle* t = (Triangle*)a;
    int pt_c = t->pt_count;
    int thr_c = t->thread_count;
    int thr_num = t->thread_num;
    int x = (div_up(pt_c, thr_c)) * (thr_num + 1);
    if (x >= pt_c) {
        x = pt_c - 1;
    }
    //std::cout << "Номер потока: " << thr_num << std::endl;
    //std::cout << "Кол-во точек: " << pt_c << std::endl;
    //std::cout << "Кол-во потоков: " << thr_c << std::endl;
    for (unsigned int i = 0; i < pt_c; ++i) {
        t->add_point(i);
        for (unsigned int j = 0; j < pt_c; ++j) {
            t->add_point(j);
            for (unsigned int k = (div_up(pt_c, thr_c)) * thr_num; k < x; ++k) {
                t->add_point(k);
                t->square(S);
                if (S > max_S) {
                    max_S = S;
                }
            }
        }
    }
    //std::cout << "Максимум у этого потока: " << max_S << std::endl;
    //std::cout << std::endl;

    return reinterpret_cast<void*>(new float(max_S));
}

int main() {
    unsigned int pt_count, thread_count;
    float* max_S = nullptr;
    float maximum = 0;
    
    std::cout << "Count threads: ";
    std::cin >> thread_count;
    std::cout << "Count points: ";
    std::cin >> pt_count;

    pthread_t thread_id[thread_count];
    class Triangle t(pt_count, thread_count);

    std::cout << "Start!" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now(); // Start the timer

    /* Work with threads */
    for (int n = 0; n < thread_count; n++) {
        t.thread_num = n;
        pthread_create(&thread_id[n+1], nullptr, perebor, &t);
    }
    for (int n = 0; n < thread_count; n++) {  // Correct loop bounds
        pthread_join(thread_id[n+1], reinterpret_cast<void**>(&max_S));
        if (*max_S >= maximum) {
            maximum = *max_S;
        }
        delete max_S;  // Release memory
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // Stop the timer
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Итоговое время: " << duration.count() << " миллисекунд" << std::endl;

    std::cout << "Итоговый максимум: " << maximum << std::endl;
    return 0;
}