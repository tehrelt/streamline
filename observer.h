#pragma once
#include <vector>
#include <functional>

template <typename T>
class Subject {
private:
    std::vector<std::function<void(T)>> fns;

public:
    void attach(std::function<void(T)> fn) {
        fns.push_back(fn);
    }

    void clear() {
        fns.clear();
    }

    void notify(T data) {
        for (std::function<void(T)> fn : fns) {
            fn(data);
        }
    }
};





