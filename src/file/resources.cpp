#include <file/resources.h>
#include <toml++/toml.h>
#include <util/logging.h>
#include <utility>

namespace calamus {

ResourceLoader::ResourceLoader(std::filesystem::path root)
    : m_root(std::move(root)) {
}

ResourceLoader::~ResourceLoader() = default;

#define CHECK_EXIST_DIR(path, desc)                                          \
    if (!exists(path))                                                       \
        return Error::formatted(desc " directory does not exist: {}", path); \
    if (!is_directory(path))                                                 \
        return Error::formatted(desc " is not a directory: {}", path);

Result<std::vector<TextureResource>> ResourceLoader::find_textures() {
    CHECK_EXIST_DIR(m_root, "Resource root")

    const auto texture_root = m_root / "textures";
    CHECK_EXIST_DIR(texture_root, "Texture root")

    auto textures = std::vector<TextureResource> {};
    for (const auto& it : std::filesystem::recursive_directory_iterator(texture_root)) {
        const auto& path = it.path().relative_path();
        const auto extension = path.extension();

        if (extension == ".toml")
            continue;
        if (extension != ".png") {
            LOG_WARNING("This file likely should not be here: {}", path)
            continue;
        }

        const auto toml_path = std::filesystem::path { path }.replace_extension(".toml");
        const auto resource_name = path.filename().replace_extension().string();
        if (!exists(toml_path)) {
            LOG_WARNING("Skipping texture without a toml description file: {}", resource_name)
            continue;
        }

        auto resource_toml = toml::table {};
        try {
            resource_toml = toml::parse_file(toml_path.string());
        } catch (const toml::parse_error& e) {
            LOG_WARNING("Error parsing toml for texture '{}': {}", resource_name, e.what())
            continue;
        }

        auto resource = TextureResource {};
        resource.path = path;
#define CHECK_TYPE(field, type)                                                                            \
    if (!resource_toml["texture"][field].is_##type()) {                                                    \
        LOG_WARNING("Missing or invalid '{}' field in description for texture '{}'", field, resource_name) \
        continue;                                                                                          \
    }

        CHECK_TYPE("name", string)
        resource.name = resource_toml["texture"]["name"].as_string()->get();
        CHECK_TYPE("width", number)
        resource.size.width = static_cast<i32>(resource_toml["texture"]["width"].as_integer()->get());
        CHECK_TYPE("height", number)
        resource.size.height = static_cast<i32>(resource_toml["texture"]["height"].as_integer()->get());
#undef CHECK_TYPE

        if (const auto& scaling = resource_toml["texture"]["scaling"]; scaling) {
            if (!scaling.is_string()) {
                LOG_WARNING("Invalid 'scaling' field in description for texture '{}'", resource_name);
                continue;
            }

            const auto scaling_string = std::string_view { scaling.as_string()->value_or("stretch") };
            if (scaling_string != "stretch" && scaling_string != "fill") {
                LOG_WARNING("'scaling' field must be one of ['stretch', 'fill'] in description for texture '{}'", resource_name);
                continue;
            }

            const auto scaling_mode = scaling_string == "stretch" ? Texture::Scaling::Stretch : Texture::Scaling::Fill;
            resource.scaling = scaling_mode;
        }

        if (const auto& tiled = resource_toml["texture"]["tiled"]; tiled) {
            if (!tiled.is_boolean()) {
                LOG_WARNING("Invalid 'tiled' field in description for texture '{}'", resource_name);
                continue;
            }

            resource.tiled = tiled.as_boolean()->get();
        } else {
            // If no 'tiled' field is present, there's no need to go any further.
            textures.emplace_back(std::move(resource));
            continue;
        }
        // Same applies if 'tiled' is present, but false.
        if (!resource.tiled) {
            textures.emplace_back(std::move(resource));
            continue;
        }

        if (const auto& tile_width = resource_toml["texture"]["tile_width"]; tile_width) {
            if (!tile_width.is_number()) {
                LOG_WARNING("Invalid 'tile_width' field in description for texture '{}'", resource_name);
                continue;
            }

            resource.tile_size.width = static_cast<i32>(tile_width.as_integer()->get());
        }

        if (const auto& tile_height = resource_toml["texture"]["tile_height"]; tile_height) {
            if (!tile_height.is_number()) {
                LOG_WARNING("Invalid 'tile_height' field in description for texture '{}'", resource_name);
                continue;
            }

            resource.tile_size.height = static_cast<i32>(tile_height.as_integer()->get());
        }

        if (const auto& tile_names = resource_toml["texture"]["tile_names"]; tile_names) {
            if (!tile_names.is_array()) {
                LOG_WARNING("Invalid 'tile_names' field in description for texture '{}'", resource_name);
                continue;
            }

            const auto* array = VERIFY(tile_names.as_array());
            const auto max_size = static_cast<size_t>((resource.size.width / resource.tile_size.width) * (resource.size.height / resource.tile_size.height));

            if (array->size() > max_size) {
                LOG_WARNING("'tile_names' field in description for texture '{}' contains more entries than there are tiles ({}, max {})", resource_name, array->size(), max_size)
            }

            auto tile_names_vector = std::vector<std::string> {};
            tile_names_vector.reserve(max_size);

            if (array->empty()) {
                LOG_WARNING("'tile_names' field in description for texture '{}' is empty, using default names", resource_name)
                for (size_t i = 0; i < max_size; i++) {
                    auto generated_name = fmt::format("{}_{}", resource.name, i);

                    tile_names_vector.emplace_back(std::move(generated_name));
                }
            } else {
                auto tile_name_count = size_t { 0 };

                for (const auto& value : *array) {
                    if (!value.is_string()) {
                        LOG_WARNING("'tile_names' field in description for texture '{}' contains entries which are not strings", resource_name)
                        break;
                    }

                    if (++tile_name_count == max_size)
                        break;
                    tile_names_vector.emplace_back(value.as_string()->get());
                }
            }

            resource.tile_names = std::move(tile_names_vector);
        } else {
            LOG_WARNING("no 'tile_names' field in description for texture '{}'", resource_name)
            continue;
        }

        textures.emplace_back(std::move(resource));
    }

    LOG_INFO("Found {} texture resource(s)", textures.size())
    return textures;
}

Result<std::unordered_map<Resources::FontType, FontResource>> ResourceLoader::find_fonts() {
    CHECK_EXIST_DIR(m_root, "Resource root")

    const auto fonts_root = m_root / "fonts";
    CHECK_EXIST_DIR(fonts_root, "Fonts root")

    const auto fonts_path = fonts_root / "fonts.toml";
    if (!exists(fonts_path))
        return Error::formatted("Font configuration file does not exist: {}", fonts_path);
    if (!is_regular_file(fonts_path))
        return Error::formatted("Not a regular file: {}", fonts_path);

    auto resource_toml = toml::table {};
    try {
        resource_toml = toml::parse_file(fonts_path.string());
    } catch (const toml::parse_error& e) {
        return Error::formatted("Error parsing font configuration file: {}", e.what());
    }

    const auto find_font = [&](std::string_view font_type) -> Result<FontResource> {
        const auto& font = resource_toml[font_type];
        if (!font)
            return Error::formatted("No font specified for type '{}'", font_type);

        // It'd be stupid if this wasn't the case. If the user breaks the config this badly, crashing is justified.
        // To be honest I just don't feel like typing out another error message, and instead I'm wasting my time
        // typing out these ridiculous comments. No one will ever read this.
        VERIFY(font.is_table());
        auto path = font["path"];
        if (!path.is_string())
            return Error::formatted("Invalid path field in font configuration for type '{}'", font_type);

        const auto& path_string = path.as_string()->get();
        auto font_path = fonts_root / path_string;

        if (!exists(font_path))
            return Error::formatted("Font does not exist: '{}'", font_path);
        if (!is_regular_file(font_path))
            return Error::formatted("Not a regular file: {}", font_path);

        const auto filter_string = std::string_view { font["filtering"].value_or("point") };
        auto filter = FontResource::Filter::_Error;
        if (filter_string == "point")
            filter = FontResource::Filter::Point;
        if (filter_string == "bilinear")
            filter = FontResource::Filter::Bilinear;
        if (filter_string == "trilinear")
            filter = FontResource::Filter::Trilinear;
        if (filter_string == "anisotropic")
            filter = FontResource::Filter::Anisotropic;

        if (filter == FontResource::Filter::_Error)
            return Error::formatted("'filtering' field must be one of ['point', 'bilinear', 'trilinear', 'anisotropic'] in font configuration for type '{}'", font_type);

        return FontResource {
            font_path,
            filter,
        };
    };

    auto fonts = std::unordered_map<Resources::FontType, FontResource> {};
    fonts[Resources::FontType::Regular] = TRY(find_font("regular"));
    fonts[Resources::FontType::Monospace] = TRY(find_font("monospace"));

    for (const auto& [type, resource] : fonts) {
        LOG_INFO("Font type {}: {} with filter {}", std::to_underlying(type), resource.path, std::to_underlying(resource.filter))
    }
    return fonts;
}

#undef CHECK_EXIST_DIR

}