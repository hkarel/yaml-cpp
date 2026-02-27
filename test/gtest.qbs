import qbs
import qbs.FileInfo

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

    cpp.includePaths: [
        "googletest-1.13.0/googlemock/include",
        "googletest-1.13.0/googlemock",
        "googletest-1.13.0/googletest/include",
        "googletest-1.13.0/googletest",
    ]
    cpp.cxxLanguageVersion: "c++17"

    files: [
        "googletest-1.13.0/googletest/include/gtest/**/*.h",
        "googletest-1.13.0/googletest/src/*.cc",
        "googletest-1.13.0/googletest/src/*.h",
        "googletest-1.13.0/googlemock/include/gmock/**/*.h",
        "googletest-1.13.0/googlemock/src/*.cc",
    ]
    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [
            FileInfo.joinPaths(exportingProduct.sourceDirectory, "googletest-1.13.0/googlemock/include"),
            FileInfo.joinPaths(exportingProduct.sourceDirectory, "googletest-1.13.0/googletest/include"),
        ]
    }
}
