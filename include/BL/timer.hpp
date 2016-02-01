#include <chrono>
#include <thread>
#include <iostream>

namespace BL {

class Timer {
private:
        using Clock = std::chrono::steady_clock;

        const Clock::time_point start;
public:
        Timer(): start(Clock::now()) {}
        void roundTo(Clock::duration duration) {
                auto end = Clock::now();
                auto wait = start - end + duration;
                if(wait.count() > 0) {
                        std::this_thread::sleep_for(wait);
                }
        }
        int elapsedMS() {
                return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
        }
        int elapsedUS() {
                return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start).count();
        }
};

} //namespace BL
