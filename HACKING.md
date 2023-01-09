# Things to do:

- Use `auto`.
- Properly format code - use the `.clang-format` file.
- Go nuts with assertions. `<util/assert.h>`
- Use type aliases. `<util/types.h>`
  - `i32` instead of `int`.
  - `u64` instead of `unsigned long`.
  - ...
  - Exception: Use `const char*` for raw C strings, but `u8` for arbitrary bytes.

# Things to avoid:

- Exceptions; use `Result<T, E>` type instead.
- Raw pointers; use references or smart pointers instead. For nullable types, wrap them in `std::optional`.
- `#define`s for constant values. Use `static constexpr` instead.
- Any additional macros if possible, this is a mess already.
- Exposing raylib APIs. Use `<util/raylib/raylib_extensions.h>` wrapper utilities in implementation files.
Their global-namespaced types already cause issues, so let's keep `raylib.h` away from as many places as possible.