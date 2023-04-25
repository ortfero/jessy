project('jessy', 'cpp', 'c',
    license: 'MIT',
    default_options: [
        'cpp_std=c++17',
        'warning_level=3'
    ])

incdir = include_directories('./include')

headers = [
    './include/jessy/jessy.hpp'
]

jessy = declare_dependency(
    include_directories: incdir,
    sources: headers,
)

subdir('test')

