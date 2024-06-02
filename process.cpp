//2-1







//2-2

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <condition_variable>
#include <algorithm>

using namespace std;
using namespace chrono;

struct Process {
    int pid;
    string type; // "FG" or "BG"
    bool promoted = false;
    int remaining_time = 0; // Wait Queue에 들어간 프로세스의 남은 시간 표시
};

mutex mtx;
condition_variable cv;

queue<Process> DQ; // Dynamic Queue
queue<Process> WQ; // Wait Queue

void printQueue(const queue<Process>& q) {
    auto copy = q;
    while (!copy.empty()) {
        Process p = copy.front();
        copy.pop();
        cout << "[" << p.pid << p.type << (p.promoted ? "*" : "") << (p.remaining_time > 0 ? ":" + to_string(p.remaining_time) : "") << "] ";
    }
    cout << endl;
}

void monitor() {
    while (true) {
        this_thread::sleep_for(seconds(3));
        lock_guard<mutex> lock(mtx);
        cout << "Running: " << (DQ.empty() ? "[]" : "[" + to_string(DQ.front().pid) + DQ.front().type + "]") << endl;
        cout << "---------------------------" << endl;
        cout << "DQ: ";
        printQueue(DQ);
        cout << "---------------------------" << endl;
        cout << "WQ: ";
        printQueue(WQ);
        cout << endl;
    }
}

vector<string> parse(const string& command) {
    vector<string> tokens;
    istringstream stream(command);
    string token;
    while (getline(stream, token, ' ')) {
        tokens.push_back(token);
    }
    return tokens;
}

void exec(const vector<string>& args) {
    if (args.empty()) return;

    string cmd = args[0];
    if (cmd == "echo" && args.size() > 1) {
        cout << args[1] << endl;
    }
    else if (cmd == "dummy") {
        // Do nothing
    }
    else if (cmd == "gcd" && args.size() > 2) {
        int x = stoi(args[1]);
        int y = stoi(args[2]);
        while (y != 0) {
            int temp = y;
            y = x % y;
            x = temp;
        }
        cout << "GCD: " << x << endl;
    }
    else if (cmd == "prime" && args.size() > 1) {
        int limit = stoi(args[1]);
        vector<bool> is_prime(limit + 1, true);
        is_prime[0] = is_prime[1] = false;
        for (int i = 2; i * i <= limit; ++i) {
            if (is_prime[i]) {
                for (int j = i * i; j <= limit; j += i) {
                    is_prime[j] = false;
                }
            }
        }
        int count = count_if(is_prime.begin(), is_prime.end(), [](bool p) { return p; });
        cout << "Prime count: " << count << endl;
    }
    else if (cmd == "sum" && args.size() > 1) {
        int limit = stoi(args[1]);
        int sum = (limit * (limit + 1)) / 2;
        cout << "Sum: " << sum % 1000000 << endl;
    }
}

void shell() {
    ifstream commands("commands.txt");
    string line;
    int pidCounter = 0;
    while (getline(commands, line)) { 
        istringstream ss(line);
        string token;
        while (getline(ss, token, ';')) {
            Process p;
            p.pid = pidCounter++;
            p.type = (token[0] == '&') ? "B" : "F";
            string command = (token[0] == '&') ? token.substr(1) : token;
            auto args = parse(command);
            exec(args);
            {
                lock_guard<mutex> lock(mtx);
                DQ.push(p);
            }
        }
        this_thread::sleep_for(seconds(1)); // Y초 동안 sleep
    }
}

int main() {
    thread monitorThread(monitor);
    monitorThread.detach();

    thread shellThread(shell);
    shellThread.join();

    return 0;
}





//2-3

//#include <iostream>
//#include <thread>
//#include <vector>
//#include <string>
//#include <fstream>
//#include <sstream>
//#include <chrono>
//#include <queue>
//#include <mutex>
//
//using namespace std;
//using namespace chrono;
//
//mutex mtx;
//
//struct Process {
//    int pid;
//    string command;
//    vector<string> args;
//    bool promoted = false;
//};
//
//queue<Process> DQ;
//
//void shell() {
//    ifstream commands("commands.txt");
//    string line;
//    int pidCounter = 0;
//    while (getline(commands, line)) {
//        istringstream ss(line);
//        string token;
//        while (getline(ss, token, ';')) {
//            Process p;
//            p.pid = pidCounter++;
//            p.command = token;
//            DQ.push(p);
//        }
//        this_thread::sleep_for(seconds(1));
//    }
//}
//
//void execute() {
//    while (true) {
//        this_thread::sleep_for(seconds(1));
//        lock_guard<mutex> lock(mtx);
//        if (!DQ.empty()) {
//            Process p = DQ.front();
//            DQ.pop();
//            cout << "Executing: " << p.command << endl;
//        }
//    }
//}

//int main() {
//    thread shellThread(shell);
//    thread executeThread(execute);
//
//    shellThread.join();
//    executeThread.join();
//    return 0;
//}
