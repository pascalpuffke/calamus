#pragma once

#include <raylib.h>
#include <util/formatter.h>

FORMATTER(Vector2, "({}, {})", value.x, value.y)
FORMATTER(Rectangle, "({}, {}, {}, {})", value.x, value.y, value.width, value.height)
FORMATTER(Texture, "Texture(id={}, width={}, height={}, mipmaps={}, format={})", value.id, value.width, value.height, value.mipmaps, value.format)