#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include<vector>
#include <mutex>
#include <queue>
#include <condition_variable>



std::queue<std::string> FileQueue;
std::mutex qm;
std::condition_variable cv;

long long sumofvalues = 0;


void reader()
{
    while (true) {
        std::string filename;

        {
            std::unique_lock<std::mutex> lock(qm);
            cv.wait(lock, [] {return !FileQueue.empty(); });

            if (FileQueue.empty()) {
                break; 
            }

            filename = FileQueue.front();
            FileQueue.pop();
        }


        std::ifstream file(filename);
        if (!file.is_open()) {
            continue;
        }

        std::string line;
        while (std::getline(file, line)) {
            sumofvalues += stoi(line);
        }

        file.close();
    }
    
}


int main()
{

    int numCores = std::thread::hardware_concurrency();

    std::vector<std::string >filenames;

    for (int i = 0; i < 100; i++) {
        FileQueue.push("filename" + std::to_string(i) + ".txt");
    }

    std::vector<std::thread> reader_threads;

    for (int i = 0; i < numCores/2; i++) {
        reader_threads.emplace_back(reader);
    }

    cv.notify_all(); 

    for (auto& thread : reader_threads) {
        thread.join();
    }

    return 0;
}
