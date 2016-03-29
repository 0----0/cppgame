#pragma once

#include <luajit-2.0/lua.hpp>
#include <string>
#include <functional>

// struct lua_State;
template<typename T>
int destroyObj(lua_State* L) {
        T* obj = static_cast<T*>(lua_touserdata(L, 1));
        obj->~T();
        return 0;
}

template<typename T>
inline T loadLuaValue(lua_State* L, int idx);

template<>
inline int loadLuaValue(lua_State* L, int idx) {
        return lua_tonumber(L, idx);
}

template<>
inline unsigned int loadLuaValue(lua_State* L, int idx) {
        return lua_tonumber(L, idx);
}

template<>
inline float loadLuaValue(lua_State* L, int idx) {
        return lua_tonumber(L, idx);
}

template<typename T>
inline void pushLuaValue(lua_State* L, T value);

template<>
inline void pushLuaValue(lua_State* L, int val) {
        lua_pushnumber(L, val);
}

template<>
inline void pushLuaValue(lua_State* L, unsigned val) {
        lua_pushnumber(L, val);
}

template<>
inline void pushLuaValue(lua_State* L, float val) {
        lua_pushnumber(L, val);
}

template<typename... Args>
struct loadArgs;

template<typename Arg>
struct loadArgs<Arg> {
        static std::tuple<Arg> f(lua_State* L, int start) {
                return std::make_tuple(loadLuaValue<Arg>(L, start));
        }
};

template<typename Arg, typename... Args>
struct loadArgs<Arg, Args...> {
        static std::tuple<Arg, Args...> f(lua_State* L, int start) {
                return std::tuple_cat(std::make_tuple(loadLuaValue<Arg>(L, start), loadArgs<Args...>::f(L, start+1)));
        }
};

template<typename R, typename... Args>
struct callFn2 {
        using argtuple = std::tuple<Args...>;

        template<int currArg, typename... ArgsSoFar>
        static
        std::enable_if_t<currArg != std::tuple_size<argtuple>::value, R>
        f(std::function<R(Args...)>* fn, lua_State* L, int startIdx, ArgsSoFar... args) {
                using currArgType = std::tuple_element_t<currArg, argtuple>;
                return callFn2::template f<currArg + 1>(fn, L, startIdx, std::forward(args)..., loadLuaValue<currArgType>(L, startIdx + currArg));
        }

        template<int currArg, typename... ArgsSoFar>
        static
        std::enable_if_t<currArg == std::tuple_size<argtuple>::value, R>
        f(std::function<R(Args...)>* fn, lua_State* L, int startIdx, ArgsSoFar... args) {
                return fn->operator()(std::forward<Args...>(args...));
        }
};

template<typename R, typename... Args>
std::enable_if_t<std::is_same<R, void>::value,
int> callFn(lua_State* L) {
        using fntype = std::function<R(Args...)>;
        fntype* obj = static_cast<fntype*>(lua_touserdata(L, lua_upvalueindex(1)));
        callFn2<R, Args...>::template f<0>(obj, L, 1);
        return 0;
}

template<typename R, typename... Args>
std::enable_if_t<!std::is_same<R, void>::value,
int> callFn(lua_State* L) {
        using fntype = std::function<R(Args...)>;
        fntype* obj = static_cast<fntype*>(lua_touserdata(L, lua_upvalueindex(1)));
        pushLuaValue(L, callFn2<R, Args...>::template f<0>(obj, L, 1));
        return 1;
}

class LuaInterface {
private:
        lua_State *L;

        template<typename T>
        void pushObject(T&& obj) {
                void* lspace = lua_newuserdata(L, sizeof(T));
                // T* lobj = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
                T* lobj = new(lspace) T(std::forward<T>(obj));
                lua_newtable(L);
                lua_pushcfunction(L, destroyObj<T>);
                lua_setfield(L, -2, "__gc");
                lua_setmetatable(L, -2);
        }

        template<typename R, typename... Args>
        void pushClosure(const std::function<R(Args...)>& fn) {
                pushObject(std::move(fn));
                lua_pushcclosure(L, callFn<R, Args...>, 1);
        }

        template<typename R, typename... Args>
        void pushClosure(std::function<R(Args...)>&& fn) {
                pushObject(std::move(fn));
                lua_pushcclosure(L, callFn<R, Args...>, 1);
        }
public:
        void init();
        void loadFile(const std::string& filename);

        template<typename F>
        void addGlobalFn(const char* name, std::function<F> fn) {
                pushClosure(std::move(fn));
                lua_setglobal(L, name);
        }

        template<typename T>
        void setObj(const char* name, T&& obj) {
                pushObject(std::forward<T>(obj));
                lua_setglobal(L, name);
        }

        LuaInterface();
        ~LuaInterface();
};
