import qbs
import qbs.File

Product {
    name: "Yaml"
    targetName: "yaml"

    type: "staticlibrary"
    Depends { name: "cpp" }

    cpp.cxxFlags: [
        "-ggdb3",
        "-Wall",
        "-Wextra",
        "-Wno-unused-parameter",
    ]
    cpp.includePaths: ["include"]
    cpp.cxxLanguageVersion: "c++17"

    files: [
        "include/yaml-cpp/contrib/*.h",
        "include/yaml-cpp/node/detail/*.h",
        "include/yaml-cpp/node/*.h",
        "include/yaml-cpp/*.h",
        "src/*.cpp",
        "src/*.h",
    ]
    Export {
        Depends { name: "cpp" }
        cpp.systemIncludePaths: ["include"]
    }
}
