import qbs

Project {
    name: "YamlCpp (Tests)"
    minimumQbsVersion: "1.23.0"

    Product {
        name: "YamlTest"
        targetName: "yaml-test"
        condition: true

        type: ["application", "autotest"]
        consoleApplication: true
        destinationDirectory: "./bin"

        Depends { name: "cpp" }
        Depends { name: "GTest" }
        Depends { name: "Yaml" }

        cpp.cxxFlags: [
            "-ggdb3",
            "-Wall",
            "-Wextra",
            "-Wno-sign-compare",
            "-Wno-unused-parameter",
            "-Wno-unused-variable",
        ]

        cpp.includePaths: [
            "test",
            "src",
        ]

        cpp.dynamicLibraries: [
            "pthread"
        ]

        files: [
            "test/integration/*.cpp",
            "test/node/*.cpp",
            "test/*.cpp",
            "test/*.h",
            //"test/integration/error_messages_test.cpp",
        ]
    }

    references: [
        "yaml.qbs",
        "test/gtest.qbs",
    ]
}
