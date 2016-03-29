#include "LuaInterface.hpp"

#include "AssetManager.hpp"

#include <iostream>

LuaInterface::LuaInterface()
{
        L = lua_open();
        luaL_openlibs(L);
}

LuaInterface::~LuaInterface() {
        lua_close(L);
}

void LuaInterface::init() {

}

void LuaInterface::loadFile(const std::string& filename) {
        auto script = AssetManager::get().getScript(filename);
        int error = luaL_loadbuffer(L, script->data(), script->size(), filename.data());
        if (error) {
                std::cerr << lua_tostring(L, -1);
                lua_pop(L, 1);
        }
        error = lua_pcall(L, 0, 0, 0);
        if (error) {
                std::cerr << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1);
        }
}