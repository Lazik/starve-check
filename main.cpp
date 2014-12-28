
#include <atomic>
#include <thread>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "stress_instance.h"

// This is only to limit the number of threads to a sane number
static constexpr std::size_t max_threads = 256;

typedef std::chrono::system_clock Clock;
typedef std::chrono::time_point<Clock> TimePoint;
typedef std::chrono::duration<double> Duration;

static void error(char const* message, int exitcode = 2)
{
    std::cerr << message << std::endl;
    exit(exitcode);
}

static int run_stress(int cpucount, int timeLimit)
{
    // Sidestep all the issues with copying
    // and ensure flat memory layout
    std::unique_ptr<stress_instance[]> instances(new stress_instance[cpucount]);
    
    stress_instance* begin = &instances[0];
    stress_instance* end = &instances[cpucount];
    
    
    ///
    
    /*
    for (auto it = s.begin(); it != s.end(); it++) {
     cout << *it << endl;
    }
    
    int * coucou() {
        
        some_logic();
    }
    
    DWORD blabla;
    blabla = coucou();
    
    
    person.SayHello = new Func<string>(() => {
        return person.FirstName + " " + person.LastName + " says hello";
    });
    
    */
    
    
    // for (vector<int>::iterator it = s.begin(); it!=s.end(); it++) {
    //
    //...
    //..
    for (auto it = begin; it != end; )
    
    
    std::for_each(begin, end,[](stress_instance& instance) {
        instance.run();
    }); // Can you use auto instead of [](...& instance)?

    // Use a condition variable wait for delays
    // It looks more like we are doing work than
    // it would if we were calling sleep_for
    std::mutex dummyMutex;
    std::condition_variable dummyConditionVariable;
    std::unique_lock<std::mutex> lock(dummyMutex);
    do
    {
        dummyConditionVariable.wait_until(lock, Clock::now() + Duration(1));

        std::for_each(begin, end,
        [](stress_instance& instance)
        {
            std::cout << std::setw(5) <<
                         (instance.getCount() / 1000000) <<
                         "M ";
        });
        std::cout << std::endl;

        // Only decrement it if it is positive
        timeLimit -= (timeLimit > 0);
    }
    while (timeLimit != 0);
}


int main(int argc, char** argv)
{
    int threads = 0;
    bool force = false;
    int timeLimit = -1;

    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-t"))
        {
            if (i + 1 >= argc)
                error("Missing argument to thread count option (-t)");
            threads = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-i"))
        {
            if (i + 1 >= argc)
                error("Missing argument to iteration count option (-i)");
            timeLimit = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-f"))
        {
            force = true;
        }
        else if (!strcmp(argv[i], "--help"))
        {
            std::cerr << argv[0] << " [-t threads] [-f]" << std::endl <<
                "threads is the number of CPUs to use. Default=0. 0=All" << std::endl <<
                "Use -f to force it to use more than the number of CPUs" << std::endl;
            exit(1);
        }
        else
        {
            error((std::string("Invalid switch: ") + argv[i]).c_str());
        }
    }

    std::size_t cpuCount = std::thread::hardware_concurrency();

    if (threads <= 0)
    {
        std::cerr << "Using all " << cpuCount << " CPUs" << std::endl;
        threads = cpuCount;
    }

    if (threads > cpuCount)
    {
        if (force)
        {
            std::cerr << "Warning, forced to use more threads"
                         " than CPUs, " << threads << " threads,"
                         " but only " << cpuCount <<
                         " CPUs detected!" << std::endl;
        }
        else
        {
            std::cerr << "Capping to " << cpuCount <<
                    ". Use -f to override." << std::endl;
            threads = cpuCount;
        }
    }

    if (threads > max_threads)
    {
        std::cerr << "Capping to " << max_threads <<
                ". Recompile with higher max_threads." << std::endl;
        threads = max_threads;
    }

    return run_stress(threads, timeLimit);
}
