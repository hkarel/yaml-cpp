import qbs
import qbs.FileInfo

Product {
    name: "Yaml"
    targetName: "yaml"

    type: (project.yamlDynamicLib === true) ? ["dynamiclibrary"] : ["staticlibrary"]

    Properties {
        destinationDirectory: project.destinationLibDirectory
        condition: project.destinationLibDirectory !== undefined
    }

    Depends { name: "cpp" }

    //Properties {
    //    condition: type.contains("staticlibrary")
    //    cpp.defines: outer.concat(["YAML_CPP_STATIC_DEFINE"])
    //}

    cpp.defines: [
        "YAML_CPP_STATIC_DEFINE"
    ]
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
        "src/node/detail/*.cpp",
        "src/node/*.cpp",
        "src/*.cpp",
        "src/*.h",
    ]

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [
            FileInfo.joinPaths(exportingProduct.sourceDirectory, "include")
        ]
        cpp.defines: ["YAML_CPP_STATIC_DEFINE"]
    }
}
