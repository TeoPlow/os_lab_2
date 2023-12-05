#include <iostream>
#include <cmath>
#include <pthread.h>
#include <vector>
#include <cstdint>
#include <chrono>

struct Point {
    int x, y, z;
};

struct TriangleData {
    float max_S;
    unsigned int pt_count;
    unsigned int thread_count;
    int** array;
};

/* Division with rounding up */
int div_up(int x, int y) {
    return (x + y - 1) / y;
}

/* Get triangle area */
void square(float& S, Point p[3]) {
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
    S = 0.5 * pow(abs(pow(opr_matr1, 2) + pow(opr_matr2, 2) + pow(opr_matr3, 2)), 0.5);
}

/* Function for working with threads */
void* perebor(void* a) {
    TriangleData* t_data = (TriangleData*)a;
    float S = 0;
    float max_S = 0;
    Point p[3];

    int pt_c = t_data->pt_count;
    int thr_c = t_data->thread_count;
    int thr_num = t_data->max_S;

    int x = (div_up(pt_c, thr_c)) * (thr_num + 1);
    if (x >= pt_c) {
        x = pt_c - 1;
    }

    for (unsigned int i = 0; i < pt_c; ++i) {
        p[0].x = t_data->array[i][0];
        p[0].y = t_data->array[i][1];
        p[0].z = t_data->array[i][2];

        for (unsigned int j = 0; j < pt_c; ++j) {
            p[1].x = t_data->array[j][0];
            p[1].y = t_data->array[j][1];
            p[1].z = t_data->array[j][2];

            for (unsigned int k = (div_up(pt_c, thr_c)) * thr_num; k < x; ++k) {
                p[2].x = t_data->array[k][0];
                p[2].y = t_data->array[k][1];
                p[2].z = t_data->array[k][2];

                square(S, p);
                if (S > max_S) {
                    max_S = S;
                }
            }
        }
    }

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

    unsigned int zhaba = pt_count;

    int** array = new int*[pt_count];
    for (unsigned int i = 0; i < pt_count; ++i) {
        array[i] = new int[3];
    }

    for (unsigned int i = 0; i < pt_count; i++) {
        for (unsigned int j = 0; j < 3; j++) {
            std::cin >> array[i][j];
        }
    }

    std::cout << "Start!" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now(); // Start the timer

    /* Work with threads */
    pthread_t thread_id[thread_count];
    for (int n = 0; n < thread_count; n++) {
        TriangleData t_data{0, pt_count, thread_count, array};
        t_data.max_S = n;
        pthread_create(&thread_id[n + 1], nullptr, perebor, &t_data);
    }

    for (int n = 0; n < thread_count; n++) { // Correct loop bounds
        pthread_join(thread_id[n + 1], reinterpret_cast<void**>(&max_S));
        if (*max_S >= maximum) {
            maximum = *max_S;
        }
        delete max_S; // Release memory
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // Stop the timer
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Итоговое время: " << duration.count() << " миллисекунд" << std::endl;

    std::cout << "Итоговый максимум: " << maximum << std::endl;

    /* Delete array */
    for (unsigned int i = 0; i < pt_count; ++i) {
        delete[] array[i];
    }
    delete[] array;

    return 0;
}