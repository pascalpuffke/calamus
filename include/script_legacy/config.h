#pragma once

#include <fmt/format.h>

#define NAN_BOXING

namespace calamus::script {

constexpr static auto DEBUG_MEASURE_PERF = true;
constexpr static auto DEBUG_PRINT_CODE = false;
constexpr static auto DEBUG_TRACE_EXECUTION = false;
constexpr static auto DEBUG_STRESS_GC = false;
constexpr static auto DEBUG_LOG_GC = false;

}
