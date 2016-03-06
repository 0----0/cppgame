#include <unordered_map>

class Keymap {
private:
        using BindMap = std::unordered_map<std::string, int>;
        BindMap keyBinds;
public:
        // template<typename ...Args>
        // Keymap(Args&& ...args):
        //         keyBinds( std::forward<Args...>(args...) )
        // {
        // }
        Keymap(BindMap&& binds):
                keyBinds( binds )
        {
        }

        int getBind(const std::string& name) const {
                auto iter = keyBinds.find(name);
                if (iter == keyBinds.end()) {
                        return -1;
                }
                return iter->second;
        }
};
