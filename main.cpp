#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>
#include <mutex>

using namespace std;

mutex mute;

void minimum (const vector<int>& vec, int left, int right, int& min) {
    min = vec[left];
    for (int i = left + 1; i < right; ++i) {
        if (min > vec[i]) {
            min = vec[i];
        }
    }
}

int main() {
    cout << "Enter count of threads on your PC :";
    unsigned int thread_count = 1, duration, best_duration = 100000000, best_threads;
    cin >> thread_count;
    srand(time(nullptr)); // псевдорандом от времени
    vector<int> vec, vec_min(thread_count);
    vector<thread*> threads(thread_count);
    for (int i = 0; i < 100000000; ++i) {
        vec.push_back(rand());  //вбиваем сто миллионов псевдорандомных значений, хотя логично, что минимум в каждой
                                //части будет нулем, так как значений очень много
    }
    for (int t = 1; t <= thread_count; ++t) {
        cout << "Using " << t << " threads\n";
        auto begin_time = std::chrono::steady_clock::now();  //стартуем счетчик времени
        int step = vec.size() / t;
        int start = 0;
        for (int i = 0; i < t; ++i) {
            threads[i] = new thread(minimum, ref(vec), start, start + step, ref(vec_min[i]));
            start += step;
        }
        for (int i = 0; i < t; ++i) {
            threads[i]->join();  //запускаем каждый поток с защитой ресурсов
            mute.lock();  //ставим блокировку
            cout << "Minimum for " << i + 1 << "_st thread is " << vec_min[i] << endl;
            mute.unlock();  //убираем блокировку
            delete threads[i];  //чистим память
        }
        int main_min = 0;
        minimum(vec_min, 0, t, main_min);  //используем главный поток
        cout << "Minimum for main thread is " << main_min << endl;
        auto end_time = std::chrono::steady_clock::now();
        duration = chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time).count();
        if (best_duration > duration) {
            best_duration = duration;
            best_threads = t;
        }
        cout << "Time for " << t  << " threads is " << duration << endl;
        cout << "\n\n";
    }
    cout << "The best time is " << best_duration << " for " << best_threads << " threads" << endl;
    return 0;
}
