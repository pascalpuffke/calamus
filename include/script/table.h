#pragma once

#include <optional>
#include <script/config.h>
#include <script/value.h>
#include <unordered_map>

namespace calamus::script {

struct Entry {
    ObjString* key;
    Value value;
};

class Table {
public:
    void reset();
    void free();

    [[nodiscard]] auto size() const noexcept { return m_entry_map.size(); }
    [[nodiscard]] const auto& entries() const noexcept { return m_entry_map; }

    [[nodiscard]] std::optional<Value> get(ObjString* key);
    [[nodiscard]] ObjString* find_string(const char* chars, i32 length, u32 hash);
    bool set(ObjString* key, Value value);
    void remove(ObjString* key);

    // Memory related stuff. I don't think these should be members of Table.
    void remove_white();
    void mark();

private:
    std::unordered_map<ObjString*, Value> m_entry_map {};
};

}
