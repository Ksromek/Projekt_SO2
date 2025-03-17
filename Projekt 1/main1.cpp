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
        log << "Philosopher " << id << " is thinking...\n";
        this_thread::sleep_for(chrono::milliseconds(1000));

        // Picking up forks
        wait(forks[left_fork]);
        wait(forks[right_fork]);

        // Eating
        log << "Philosopher " << id << " is eating...\n";
        this_thread::sleep_for(chrono::milliseconds(1000));

        // Putting down forks
        signal(forks[left_fork]);
        signal(forks[right_fork]);

        log << "Philosopher " << id << " put down forks and is thinking...\n";

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