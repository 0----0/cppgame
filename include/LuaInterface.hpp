#pragma once

#include <luajit-2.0/lua.hpp>
#include <string>
#include <functional>
#include <memory>

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

namespace detail {

template<typename ObjT, typename MType, MType M>
struct method_info;

template<typename ObjT, typename R, typename... Args, R (ObjT::*M)(Args...)>
struct method_info<ObjT, R (ObjT::*)(Args...), M> {
        using return_type = R;
        using argtuple = std::tuple<Args...>;

        static R callMethod(ObjT& obj, Args... args) {
                return (obj.*M)(std::forward<Args>(args)...);
        }

        template<int startIdx, int currArg, typename... ArgsSoFar>
        static
        std::enable_if_t<currArg != std::tuple_size<argtuple>::value,
        R> accumulateArgsLua(ObjT& obj, lua_State* L, ArgsSoFar... args) {
                using currArgT = std::tuple_element_t<currArg, argtuple>;
                return method_info::template accumulateArgsLua<startIdx, currArg + 1>(
                        obj, L, std::forward<ArgsSoFar>(args)...,
                        loadLuaValue<currArgT>(L, startIdx + currArg)
                );
        }

        template<int startIdx, int currArg, typename... ArgsSoFar>
        static
        std::enable_if_t<currArg == std::tuple_size<argtuple>::value,
        R> accumulateArgsLua(ObjT& obj, lua_State* L, ArgsSoFar... args) {
                using currArgT = std::tuple_element_t<currArg, argtuple>;
                return callMethod(obj, std::forward<Args>(args)...);
        }
};

template<typename ObjT, typename R, typename... Args, R (ObjT::*M)(Args...) const>
struct method_info<ObjT, R (ObjT::*)(Args...) const, M> {
        using return_type = R;
        using argtuple = std::tuple<Args...>;

        static R callMethod(const ObjT& obj, Args... args) {
                (obj.*M)(std::forward<Args>(args)...);
        }

        template<int startIdx, int currArg, typename... ArgsSoFar>
        static
        std::enable_if_t<currArg != std::tuple_size<argtuple>::value,
        R> accumulateArgsLua(const ObjT& obj, lua_State* L, ArgsSoFar... args) {
                using currArgT = std::tuple_element_t<currArg, argtuple>;
                return method_info::template accumulateArgsLua<startIdx, currArg + 1>(
                        obj, L, std::forward<ArgsSoFar>(args)...,
                        loadLuaValue<currArgT>(L, startIdx + currArg)
                );
        }

        template<int startIdx, int currArg, typename... ArgsSoFar>
        static
        std::enable_if_t<currArg == std::tuple_size<argtuple>::value,
        R> accumulateArgsLua(const ObjT& obj, lua_State* L, ArgsSoFar... args) {
                return callMethod(obj, std::forward<Args>(args)...);
        }
};

template<typename ObjT, typename MType, MType M>
std::enable_if_t<std::is_same<typename method_info<ObjT, MType, M>::return_type, void>::value,
int> callMethodLua(lua_State* L) {
        const ObjT* obj = static_cast<ObjT*>(lua_touserdata(L, lua_upvalueindex(1)));
        method_info<ObjT, MType, M>::template accumulateArgsLua<1, 0>(*obj, L);
        return 0;
}

template<typename ObjT, typename MType, MType M>
std::enable_if_t<!std::is_same<typename method_info<ObjT, MType, M>::return_type, void>::value,
int> callMethodLua(lua_State* L) {
        using R = typename method_info<ObjT, MType, M>::return_type;
        const ObjT* obj = static_cast<ObjT*>(lua_touserdata(L, lua_upvalueindex(1)));
        R ret = method_info<ObjT, MType, M>::template accumulateArgsLua<1, 0>(*obj, L);
        pushLuaValue(ret);
        return 1;
}

};

class FSWatcher;

class LuaInterface {
private:
        lua_State *L;
        std::unique_ptr<FSWatcher> fsWatch;

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

        template<typename F>
        void pushClosure(F&& fn) {
                pushObject(std::forward<F>(fn));
                lua_pushcclosure(L, detail::callMethodLua<F, decltype(&F::operator()), &F::operator()>, 1);
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
