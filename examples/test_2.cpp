/* Expected: Runtime error! */

#include <thread>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

int main() {
    this_thread::sleep_for(seconds(1));
    int a = 0, b = 0;
    while (cin >> a >> b)
        cout << a + b << endl;
    return 0;
}