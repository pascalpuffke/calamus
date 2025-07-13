#pragma once

#include <ui/structs.h>
#include <unordered_map>
#include <vector>

namespace calamus {

class Level final {
public:
    using TextureId = usize;
    using TextureMap = std::unordered_map<TextureId, std::string>;

    constexpr Level() = default;
    Level(
        std::string&& name,
        u16 version,
        Size<u16> size,
        TextureMap&& texture_map,
        std::vector<TextureId>&& level_data
    );

    ~Level() = default;

    constexpr Level(const Level&) = delete;
    constexpr Level& operator=(const Level&) = delete;
    constexpr Level(Level&&) noexcept = delete;
    constexpr Level& operator=(Level&&) noexcept = delete;

    [[nodiscard]] constexpr auto name() const noexcept -> std::string_view { return m_name; }
    [[nodiscard]] constexpr auto version() const noexcept { return m_version; }
    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
    [[nodiscard]] constexpr auto width() const noexcept { return m_size.width; }
    [[nodiscard]] constexpr auto height() const noexcept { return m_size.height; }
    [[nodiscard]] constexpr auto texture_map() const noexcept -> const TextureMap& { return m_texture_map; }
    [[nodiscard]] constexpr auto level_data() const noexcept -> const std::vector<TextureId>& { return m_level_data; }

private:
    std::string m_name {};
    u16 m_version { 0 };
    Size<u16> m_size { static_cast<unsigned short>(-1) };
    TextureMap m_texture_map {};
    std::vector<TextureId> m_level_data {};
};

}