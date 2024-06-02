//2-1

//#include <iostream>
//#include <vector>
//#include <mutex>
//#include <stdexcept>
//
//using namespace std;
//
//enum ProcessType { FG, BG };
//
//struct Process {
//    int pid;
//    ProcessType type;
//    bool promoted = false;
//};
//
//struct Node {
//    vector<Process> processes;
//    Node* next;
//    Node() : next(nullptr) {}  // next 멤버를 nullptr로 초기화
//};
//
//class DynamicQueue {
//private:
//    Node* top;
//    int threshold;
//    int processCount = 0;
//    Node* promotePointer;
//    mutex mtx;
//
//public:
//    DynamicQueue(int threshold) : threshold(threshold) {
//        top = new Node();
//        promotePointer = top;
//    }
//
//    void enqueue(Process p) {
//        lock_guard<mutex> lock(mtx);
//        if (p.type == FG) {
//            top->processes.push_back(p);
//        }
//        else {
//            Node* current = top;
//            while (current->next != nullptr) {
//                current = current->next;
//            }
//            current->processes.push_back(p);
//        }
//        processCount++;
//        split_n_merge();
//    }
//
//    Process dequeue() {
//        lock_guard<mutex> lock(mtx);
//        if (top->processes.empty()) {
//            throw runtime_error("Queue is empty");
//        }
//        Process p = top->processes.front();
//        top->processes.erase(top->processes.begin());
//        processCount--;
//        if (top->processes.empty() && top->next != nullptr) {
//            Node* oldTop = top;
//            top = top->next;
//            delete oldTop;
//        }
//        return p;
//    }
//
//    void promote() {
//        lock_guard<mutex> lock(mtx);
//        if (promotePointer->next == nullptr) {
//            promotePointer = top;
//        }
//        if (!promotePointer->processes.empty()) {
//            Process p = promotePointer->processes.front();
//            promotePointer->processes.erase(promotePointer->processes.begin());
//            p.promoted = true;
//            enqueue(p);
//        }
//        if (promotePointer->processes.empty()) {
//            Node* oldPointer = promotePointer;
//            promotePointer = top;
//            while (promotePointer != oldPointer && promotePointer->processes.empty()) {
//                promotePointer = promotePointer->next;
//            }
//        }
//    }
//
//    void split_n_merge() {
//        Node* current = top;
//        while (current != nullptr) {
//            if (current->processes.size() > threshold) {
//                Node* newNode = new Node();
//                auto middle = current->processes.begin() + current->processes.size() / 2;
//                newNode->processes.insert(newNode->processes.begin(), middle, current->processes.end());
//                current->processes.erase(middle, current->processes.end());
//                newNode->next = current->next;
//                current->next = newNode;
//            }
//            current = current->next;
//        }
//    }
//
//    void print() {
//        lock_guard<mutex> lock(mtx);
//        Node* current = top;
//        cout << "---------------------------" << endl;
//        while (current != nullptr) {
//            cout << "[";
//            for (auto& p : current->processes) {
//                cout << p.pid << (p.type == FG ? "F" : "B") << (p.promoted ? "*" : "") << " ";
//            }
//            cout << "] ";
//            current = current->next;
//        }
//        cout << endl;
//    }
//};
//
//int main() {
//    try {
//        DynamicQueue dq(5);
//
//        // Test the Dynamic Queue
//        for (int i = 0; i < 10; ++i) {
//            Process p = { i, (i % 2 == 0) ? FG : BG };
//            dq.enqueue(p);
//        }
//
//        dq.print();
//
//        dq.promote();
//        dq.print();
//
//        for (int i = 0; i < 5; ++i) {
//            dq.dequeue();
//            dq.print();
//        }
//    }
//    catch (const exception& e) {
//        cerr << "Exception: " << e.what() << endl;
//    }
//
//    return 0;
//}





//2-2

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>

using namespace std;
using namespace chrono;

struct Process {
    int pid;
    string type; // "FG" or "BG"
    bool promoted = false;
};

mutex mtx;
condition_variable cv;

queue<Process> WQ;

void monitor(queue<Process>& dq) {
    while (true) {
        this_thread::sleep_for(seconds(3));
        lock_guard<mutex> lock(mtx);
        cout << "Running: " << (dq.empty() ? "[]" : "[" + dq.front().type + "]") << endl;
        cout << "---------------------------" << endl;
        cout << "DQ: ";
        if (!dq.empty()) {
            cout << "[" << dq.front().pid << dq.front().type << "] ";
        }
        cout << endl;
        cout << "---------------------------" << endl;
        cout << "WQ: ";
        if (!WQ.empty()) {
            for (auto& p : WQ) {
                cout << "[" << p.pid << p.type << "] ";
            }
        }
        cout << endl;
    }
}

int main() {
    queue<Process> dq;

    // Test the Alarm Clock
    for (int i = 0; i < 5; ++i) {
        Process p = { i, (i % 2 == 0) ? "FG" : "BG" };
        dq.push(p);
    }

    thread monitorThread(monitor, ref(dq));
    monitorThread.detach();

    this_thread::sleep_for(seconds(15)); // Allow the monitor to run for a while

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
