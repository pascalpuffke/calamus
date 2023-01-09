#pragma once

#include <script/config.h>

namespace calamus::script {

struct SourceLocation {
    using size_type = u32;
    size_type start;
    size_type end;
};

}