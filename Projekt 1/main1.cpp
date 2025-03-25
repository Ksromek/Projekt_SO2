// Colors for better console readability
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

using namespace std;

int num_philosophers;
vector<int> forks;

void wait(int& semaphore) {
    while (semaphore == 1) {
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    semaphore = 1;
}

void signal(int& semaphore) {
    semaphore = 0;
}

void philosopher(int id) {
    int left_fork = id;
    int right_fork = (id + 1) % num_philosophers;

    if (id == num_philosophers - 1) {
        swap(left_fork, right_fork);
    }

    while (true) {
        stringstream log;

        // Thinking
        log <<CYAN<< "Philosopher " << id << " is thinking...\n"<<RESET;
        this_thread::sleep_for(chrono::milliseconds(1000));

        // Picking up forks
        wait(forks[left_fork]);
        wait(forks[right_fork]);

        // Eating
        log <<GREEN<< "Philosopher " << id << " is eating...\n"<<RESET;
        this_thread::sleep_for(chrono::milliseconds(1000));

        // Putting down forks
        signal(forks[left_fork]);
        signal(forks[right_fork]);

        log <<RED<< "Philosopher " << id << " put down forks and is thinking...\n"<<RESET;

        cout << log.str();
    }
}

int main() {
    cout << "Enter number of philosophers: ";
    cin >> num_philosophers;

    if (num_philosophers < 2) {
        cout << "Invalid number of philosophers! Must be at least 2.\n";
        return 1;
    }

    forks.resize(num_philosophers, 0);

    vector<thread> philosophers;

    for (int i = 0; i < num_philosophers; i++) {
        philosophers.push_back(thread(philosopher, i));
    }

    for (auto& ph : philosophers) {
        ph.join();
    }

    return 0;
}