#include <cstring>
#include <script_legacy/memory.h>
#include <script_legacy/table.h>

namespace calamus::script {

void Table::reset() {
    m_entry_map.clear();
    // If we don't explicitly reserve some storage, we will immediately crash on SIGFPE.
    // ... Actually, there's always SIGFPE risk as soon as you allocate more than a couple
    // of objects.
    // This is all horribly scuffed, but I can't stand looking at scripting code anymore.
    m_entry_map.reserve(32);
}

void Table::free() {
    reset();
}

std::optional<Value> Table::get(ObjString* key) {
    auto iterator = m_entry_map.find(key);
    if (iterator == m_entry_map.end())
        return std::nullopt;
    return iterator->second;
}

bool Table::set(ObjString* key, Value value) {
    auto is_new_key = !m_entry_map.contains(key);
    m_entry_map[key] = value;
    return is_new_key;
}

void Table::remove(ObjString* key) {
    // This can't be right
    m_entry_map[key] = BOOL_VAL(true);
}

ObjString* Table::find_string(const char* chars, i32 length, u32 hash) {
    for (auto& [key, _] : m_entry_map) {
        if (key->length == length && key->hash == hash && std::memcmp(key->chars, chars, static_cast<size_t>(length)) == 0)
            return key;
    }
    return nullptr;
}

void Table::remove_white() {
    for (auto& [key, _] : m_entry_map) {
        if (!key)
            continue;
        if (!key->obj.is_marked)
            remove(key);
    }
}

void Table::mark() {
    for (auto& [key, value] : m_entry_map) {
        mark_object(reinterpret_cast<Obj*>(key));
        mark_value(value);
    }
}

}
