
<h1 align="center">
  Pack-TL
</h1>

<h4 align="center">A C++ library for operating with Template Parameter Packs</h4>

<p align="center">
<img alt="Language" src="https://img.shields.io/badge/LANG-C%2B%2B-blue?&style=for-the-badge&logo=c%2B%2B&logoColor=blue">
<img alt="GitHub" src="https://img.shields.io/github/license/castle055/packtl?style=for-the-badge">
<img alt="GitHub tag (latest SemVer)" src="https://img.shields.io/github/v/tag/castle055/packtl?color=%23fcae1e&label=latest&sort=semver&style=for-the-badge">
</p>

<p align="center">
  <a href="#overview">Overview</a> •
  <a href="#how-to-build">How To Build</a> •
  <a href="#usage">Usage</a> •
  <a href="#documentation">Documentation</a> •
  <a href="#license">License</a>
</p>

## Overview

C++ allows for templates to be defined with variadic parameters, that is, the number of parameters can be unknown. These can be used for encoding arbitrary static information into types. Here is an example:

```cpp
// Template struct to hold static information
template <typename ...Args>
struct pack {};

// Example use - Static reflection

template <typename T, std::size_t Offset>
struct field {
  using type = T;
  static constexpr std::size_t offset = Offset;
};

struct SomeObject {
  int         some_int    {};
  double      some_double {};
  std::string some_string {};

  using field_types = pack<
    field<int, 0>,
    field<double, 8>,
    field<std::string, 16>
  >;
};
```

In this case, the type alias `SomeObject::field_types` encodes a list of types corresponding to the fields of the struct `SomeObject`, as well as their offsets. The type `pack` is merely a compile-time construct, used only for holding static information. Note that none of the reflection information makes it to runtime, thus not affecting memory usage nor performance.

This library provides operations for accessing and mutating constructs like this. Some accessor examples are:

```cpp
using size   = packtl::get_size<SomeObject::field_types>::value; // 3
using first  = packtl::get_first<SomeObject::field_types>::type; // field<int, 0>
using last   = packtl::get_last<SomeObject::field_types>::type;  // field<std::string, 16>
using second = packtl::get<2, SomeObject::field_types>::type;    // field<double, 8>


bool has_int = packtl::has_item<field<int, 0>, SomeObject::field_types>::value; // true
```

Meanwhile, some of the mutations that this library can perform are:

```cpp
using appended = packtl::append<field<char, 24>>::to<SomeObject::field_types>::type;

using prepended = packtl::prepend<field<char, 24>>::to<SomeObject::field_types>::type;

using sliced = packtl::slice<1, 2, SomeObject::field_types>::type; // Returns a pack containing the 2nd and 3rd fields

```

## How to Build

After cloning this repository, or otherwise getting a copy of this directory, configure the library by running the following CMake command in this directory:

```sh
#        Built type (Debug/Release)    Build dir  Generator
#     /------------------------------\/--------\/-------\
cmake -DCMAKE_BUILD_TYPE:STRING=Debug -B./build -G Ninja
```

### Build PackTL

Once the project is configured, the library can be build with the following command:

```sh
#      Build dir   Built type     Build target
#     /--------\/-------------\/--------------\
cmake -B./build --config Debug --target packtl
```

### Build Documentation

Doxygen documentation can be build with the following command:

```sh
#      Build dir   Built type       Build target
#     /--------\/-------------\/-------------------\
cmake -B./build --config Debug --target packtl_docs
```

### Build Unit Tests

Tests can be built either all at once or separately with the following commands:

#### Build all tests

```sh
#      Build dir   Built type          Build target
#     /--------\/-------------\/-------------------------\
cmake -B./build --config Debug --target TEST_SUITE_packtl
```

#### Build only specified test file

```sh
#      Build dir   Built type          Build target
#     /--------\/-------------\/------------------------\
cmake -B./build --config Debug --target TEST_<test_name>
```

## Usage

### Adding to CMake Project

Integrating this library into a CMake project is as simple as adding the following to the `CmakeLists.txt` file:

```cmake
include(FetchContent)

FetchContent_Declare(packtl
        GIT_REPOSITORY https://github.com/castle055/packtl.git
        GIT_TAG main
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(packtl)
```

### Importing and Usage

Everything in this library can be imported from the module `packtl`, where all operations are placed withing the `packtl` namespace. Carrying on with the static reflection example, here is a very simple serializer:

```cpp
export module serializer;
export import std;
export import packtl;

// Access any field from any object, given its type and offset
template <typename T, std::size_t Offset>
struct field_accessor {
  char offset_padding[Offset];
  T value;
}

// Serialize a specific field within an object
template <typename T, typename Field>
std::string serialize_field(const T& obj) {
  using field_type = typename Field::type;
  constexpr std::size_t field_offset = Field::offset;

  field_type& field_data = static_cast<field_accessor<field_type, field_offset>*>(&obj)->value;

  return std::format("{}", field_data);
}

// Serialize all fields within an object
export template <typename T, std::size_t ...I>
std::string serialize(const T& obj, std::index_sequence<I...>) {
  using fields = typename T::field_types;

  std::string str = "{ ";

  // Serialize each field and append it to `str`
  (str.append(serialize_field<packtl::get<I, fields>::type>(obj).append("; ")), ...);
  
  str.substr(0, str.size()-2); // remove last semicolon
  str.append("}");

  return str;
}

// Serialize any object that contains a `field_types` pack
export template <typename T>
requires requires {
  typename T::field_types;
}
std::string serialize(const T& obj) {
  using fields = typename T::field_types;

  return serialize_all_fields(obj, std::make_index_sequence<packtl::get_size<fields>::value>());
}
```

And this would be the result:

```cpp
import serializer;

int main(int argc, char* argv[]) {
  SomeObject obj {
    .some_int = 1,
    .some_double = 3.14,
    .some_string = "Hello, World!",
  };

  std::cout << serialize(obj) << std::endl;

  return  0;
}
```

OUTPUT:
```
{ 1; 3.14; Hello, world!}
```

## Documentation

Documentation can be found [here]().

## License

GPL 3.0 &nbsp;&middot;&nbsp; [LICENSE.md](LICENSE.md)

---

> GitHub [@castle055](https://github.com/castle055) &nbsp;&middot;&nbsp;

