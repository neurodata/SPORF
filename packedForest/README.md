[![Build Status](https://travis-ci.org/neurodata/packedForest.svg?branch=master)](https://travis-ci.org/neurodata/packedForest)

# packedForest
A memory efficient and fast random forest which produces trees with fast prediction speeds.


## Steps to compile

### macOS

For macOS (10.14) these were a sequence of steps that resulted in a
succesful outcome (yours may be different.) 

#### Using hombrew

- `brew install gcc@8`
- `brew install libomp`
- `brew install llvm`

If you have multiple versions of gcc/g++ already you may need to edit
the `Makefile` to use the your specific version, i.e. `g++-8`.

#### Update path and environment variables.

Get the path to your brew install of `llvm` with the following command:
`brew --prefix llvm` and add it to the lines below and run them

```
echo 'export PATH="\<path to llvm\>/bin:$PATH"' >> ~/.bash_profile
echo \#\#brew install llvm for openmp support >> ~/.bash_profile
echo export LDFLAGS="-L\<path to llvm\>/lib"  >> ~/.bash_profile
echo export CPPFLAGS="-I\<path to llvm\>/include >> ~/.bash_profile
```

#### Compile and run

The first number specifies the algorithm, the second the dataset, and
the third is number of cores.
- `make`
- `./bin/fp 8 1 1` 

#### VSCode setup

##### Example `launch.json`

```json
{
    "version": "0.2.0",
    "configurations": [
      {
        "name": "(lldb) Launch",
        "type": "cppdbg",
        "request": "launch",
        "program": "${workspaceRoot}/a.out",
        "args": ["8", "1", "1"],
        "stopAtEntry": false,
        "cwd": "${workspaceFolder}/packedForest",
        "environment": [],
        "externalConsole": true,
        "MIMode": "lldb",
        "miDebuggerPath": "/Applications/Xcode.app/Contents/Developer/usr/bin/lldb-mi",
        "setupCommands": [
          {
            "description": "Enable pretty-printing for lldb",
            "text": "-enable-pretty-printing",
            "ignoreFailures": true
          }
        ]
      },
    ]
}
```

##### Example `task.json`

```json
{
    "version": "2.0.0",
    "tasks": [
      {
        "label": "build fp",
        "type": "shell",
        "command": "g++",
        "args": [
          "-g",
          "-std=c++11",
          // "-fopenmp",
          "-DDEBUG=3",
          "-Wall",
          "-O0",
          "-ffast-math",
          "packedForest/src/fp.cpp"
        ],
        "group": {
          "kind": "build",
          "isDefault": true
        }
      }
    ]
  }
```



##### Example `c_cpp_properties.json`

```json
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/packedForest/**"
            ],
            "defines": [],
            "macFrameworkPath": [
                "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk/System/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/g++",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-x64"
        }
    ],
    "version": 4
}
```
