import qbs

Product {
    name: "GTest"
    targetName: "gtest"

    type: "staticlibrary"

    Depends { name: "cpp" }

    cpp.cxxFlags: [
        "-ggdb3",
        "-Wall",
        "-Wextra",
        "-Wno-sign-compare",
        "-Wno-unused-parameter",
    ]

    property var includePaths: [
        "gtest-1.11.0/googlemock/include",
        "gtest-1.11.0/googlemock",
        "gtest-1.11.0/googletest/include",
        "gtest-1.11.0/googletest",
    ]
    cpp.includePaths: includePaths;
    cpp.cxxLanguageVersion: "c++17"

    files: [
        "gtest-1.11.0/googletest/include/gtest/**/*.h",
        "gtest-1.11.0/googletest/src/*.cc",
        "gtest-1.11.0/googletest/src/*.h",
        "gtest-1.11.0/googlemock/include/gmock/**/*.h",
        "gtest-1.11.0/googlemock/src/*.cc",
    ]
    Export {
        Depends { name: "cpp" }
        cpp.includePaths: exportingProduct.includePaths
    }
}
