#pragma once

#include <raylib.h>
#include <util/formatter.h>

FORMATTER(Vector2, "Vector2({}, {})", value.x, value.y)
FORMATTER(Rectangle, "Rectangle({}, {}, {}, {})", value.x, value.y, value.width, value.height)