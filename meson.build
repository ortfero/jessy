project('jessy', 'cpp', 'c',
    license: 'MIT',
    default_options: [
        'cpp_std=c++20',
        'warning_level=3'
    ])

incdir = include_directories('./include')

yyjson = declare_dependency(
    include_directories: include_directories('./thirdparty/yyjson'),
    sources: [
        './thirdparty/yyjson/yyjson.h',
        './thirdparty/yyjson/yyjson.c'
    ]
)

headers = [
    './include/jessy/jessy.hpp'
]

jessy = declare_dependency(
    include_directories: incdir,
    sources: headers,
    dependencies: [yyjson]
)

subdir('test')

