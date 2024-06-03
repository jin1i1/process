//2-1 O
//2-2 O
//2-3 O



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

//#include <iostream>
//#include <thread>
//#include <vector>
//#include <queue>
//#include <mutex>
//#include <chrono>
//#include <fstream>
//#include <sstream>
//#include <condition_variable>
//#include <algorithm>
//
//using namespace std;
//using namespace chrono;
//
//struct Process {
//    int pid;
//    string type; // "FG" or "BG"
//    bool promoted = false;
//    int remaining_time = 0; // Wait Queue에 들어간 프로세스의 남은 시간 표시
//};
//
//mutex mtx;
//condition_variable cv;
//
//queue<Process> DQ; // Dynamic Queue
//queue<Process> WQ; // Wait Queue
//
//void printQueue(const queue<Process>& q) {
//    auto copy = q;
//    while (!copy.empty()) {
//        Process p = copy.front();
//        copy.pop();
//        cout << "[" << p.pid << p.type << (p.promoted ? "*" : "") << (p.remaining_time > 0 ? ":" + to_string(p.remaining_time) : "") << "] ";
//    }
//    cout << endl;
//}
//
//void monitor() {
//    while (true) {
//        this_thread::sleep_for(seconds(3));
//        lock_guard<mutex> lock(mtx);
//        cout << "Running: " << (DQ.empty() ? "[]" : "[" + to_string(DQ.front().pid) + DQ.front().type + "]") << endl;
//        cout << "---------------------------" << endl;
//        cout << "DQ: ";
//        printQueue(DQ);
//        cout << "---------------------------" << endl;
//        cout << "WQ: ";
//        printQueue(WQ);
//        cout << endl;
//    }
//}
//
//vector<string> parse(const string& command) {
//    vector<string> tokens;
//    istringstream stream(command);
//    string token;
//    while (getline(stream, token, ' ')) {
//        tokens.push_back(token);
//    }
//    return tokens;
//}
//
//void exec(const vector<string>& args) {
//    if (args.empty()) return;
//
//    string cmd = args[0];
//    if (cmd == "echo" && args.size() > 1) {
//        cout << args[1] << endl;
//    }
//    else if (cmd == "dummy") {
//        // Do nothing
//    }
//    else if (cmd == "gcd" && args.size() > 2) {
//        int x = stoi(args[1]);
//        int y = stoi(args[2]);
//        while (y != 0) {
//            int temp = y;
//            y = x % y;
//            x = temp;
//        }
//        cout << "GCD: " << x << endl;
//    }
//    else if (cmd == "prime" && args.size() > 1) {
//        int limit = stoi(args[1]);
//        vector<bool> is_prime(limit + 1, true);
//        is_prime[0] = is_prime[1] = false;
//        for (int i = 2; i * i <= limit; ++i) {
//            if (is_prime[i]) {
//                for (int j = i * i; j <= limit; j += i) {
//                    is_prime[j] = false;
//                }
//            }
//        }
//        int count = count_if(is_prime.begin(), is_prime.end(), [](bool p) { return p; });
//        cout << "Prime count: " << count << endl;
//    }
//    else if (cmd == "sum" && args.size() > 1) {
//        int limit = stoi(args[1]);
//        int sum = (limit * (limit + 1)) / 2;
//        cout << "Sum: " << sum % 1000000 << endl;
//    }
//}
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
//            p.type = (token[0] == '&') ? "B" : "F";
//            string command = (token[0] == '&') ? token.substr(1) : token;
//            auto args = parse(command);
//            exec(args);
//            {
//                lock_guard<mutex> lock(mtx);
//                DQ.push(p);
//            }
//        }
//        this_thread::sleep_for(seconds(1)); // Y초 동안 sleep
//    }
//}
//
//int main() {
//    thread monitorThread(monitor);
//    monitorThread.detach();
//
//    thread shellThread(shell);
//    shellThread.join();
//
//    return 0;
//}





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
//bool shell_done = false; // shell 함수가 종료되었는지 여부를 나타내는 플래그
//
//struct Process {
//    int pid;
//    string command;
//    vector<string> args;
//    bool promoted = false;
//    bool is_background = false; // 백그라운드 명령어 여부
//};
//
//queue<Process> DQ;
//queue<Process> BQ; // 백그라운드 큐
//
//vector<string> parse(const string& command) {
//    vector<string> tokens;
//    istringstream stream(command);
//    string token;
//    while (getline(stream, token, ' ')) {
//        tokens.push_back(token);
//    }
//    return tokens;
//}
//
//void exec(const Process& p) {
//    if (p.command == "dummy") {
//        // Do nothing
//    }
//    else if (p.command.rfind("echo", 0) == 0) {
//        cout << p.command.substr(5) << endl;
//    }
//    else if (p.command.rfind("gcd", 0) == 0) {
//        istringstream iss(p.command.substr(4));
//        int x, y;
//        iss >> x >> y;
//        while (y != 0) {
//            int temp = y;
//            y = x % y;
//            x = temp;
//        }
//        cout << "GCD: " << x << endl;
//    }
//    else if (p.command.rfind("prime", 0) == 0) {
//        int limit = stoi(p.command.substr(6));
//        vector<bool> is_prime(limit + 1, true);
//        is_prime[0] = is_prime[1] = false;
//        for (int i = 2; i * i <= limit; ++i) {
//            if (is_prime[i]) {
//                for (int j = i * i; j <= limit; j += i) {
//                    is_prime[j] = false;
//                }
//            }
//        }
//        int count = count_if(is_prime.begin(), is_prime.end(), [](bool p) { return p; });
//        cout << "Prime count: " << count << endl;
//    }
//    else if (p.command.rfind("sum", 0) == 0) {
//        int limit = stoi(p.command.substr(4));
//        int sum = (limit * (limit + 1)) / 2;
//        cout << "Sum: " << sum % 1000000 << endl;
//    }
//}
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
//            if (token[0] == '&') {
//                p.is_background = true;
//                p.command = token.substr(1);
//            }
//            lock_guard<mutex> lock(mtx);
//            if (p.is_background) {
//                BQ.push(p);
//            }
//            else {
//                DQ.push(p);
//            }
//        }
//        this_thread::sleep_for(seconds(1));
//    }
//    lock_guard<mutex> lock(mtx);
//    shell_done = true; // shell 함수가 종료됨을 표시
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
//            exec(p);
//        }
//        if (!BQ.empty()) {
//            Process p = BQ.front();
//            BQ.pop();
//            thread t(exec, p);
//            t.detach();
//            cout << "Executing in background: " << p.command << endl;
//        }
//        if (DQ.empty() && BQ.empty() && shell_done) {
//            break; // 큐가 모두 비어있고 shell이 종료된 경우 루프 종료
//        }
//    }
//}
//
//int main() {
//    thread shellThread(shell);
//    thread executeThread(execute);
//
//    shellThread.join();
//    executeThread.join();
//    return 0;
//}