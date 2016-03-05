#pragma once

template<typename F>
struct _scopeExit {
        F f;
        _scopeExit(F f):
                f(f) {
        }
        ~_scopeExit() {
                f();
        }
};

template<typename F>
auto _scope_exit(F lambda) {
        return _scopeExit<F>(lambda);
}

#define COMBINE1(X,Y) X ## Y
#define COMBINE(X,Y) COMBINE1(X,Y)

#define UNIQUEPREFIX(PRE) COMBINE(PRE, __COUNTER__)

#define scope_exit(LAMBDA) auto UNIQUEPREFIX(SCOPEEXIT) = _scope_exit(LAMBDA)
