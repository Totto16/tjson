

add_languages(
    'cpp',
    native: false,
)

#TODO: get rid of this once we use clang >= 22
correct_cpp_args = ['-Wno-c2y-extensions']

foreach internal_c_arg : internal_c_args

    # cpp.get_supported_arguments reports true, but it is not really smart and the actual compiler doesn't accept this :(
    ##TODO: maybe file a meson bug?
    if internal_c_arg not in ['-Werror=implicit', '-Werror=pointer-to-int-cast']
        correct_cpp_args += internal_c_arg
    endif

endforeach

cpp = meson.get_compiler('cpp')

internal_cpp_args = []

internal_cpp_args += cpp.get_supported_arguments(correct_cpp_args)

if cpp.get_id() == 'gcc'
    internal_cpp_args += cpp.get_supported_arguments('-Wno-missing-braces')
endif

cpp_std_to_use = ['c++23', 'c++latest', 'c++20']


test_deps = []
test_src = []
test_inc_dirs = []

## see: https://github.com/doctest/doctest/commit/5eb20178ba43d7b3fd6dbf97b4d2a0907b59a42a
# and https://github.com/doctest/doctest/pull/901
# for the reason, why this is needed  like this

# don't mess with allocators, as this is test code, it should just use the default ones
test_deps += dependency(
    'doctest_patched',
    version: ['>=2.4.12', '<3'],
    fallback: ['doctest', 'doctest_dep'],
)

test_deps += dependency(
    'nlohmann_json',
    version: ['>=3.0', '<4'],
)

test_deps += tjson_dep

test_inc_dirs += include_directories('.')

test_src += files(
    'helpers/c_types.cpp',
    'helpers/c_types.hpp',
    'helpers/cpp_types.cpp',
    'helpers/cpp_types.hpp',
    'helpers/generic.cpp',
    'helpers/generic.hpp',
    'helpers/helpers.hpp',
    'helpers/json.cpp',
    'helpers/json.hpp',
    'helpers/main.cpp',
    'helpers/string_maker.hpp',
    'helpers/support.hpp',
)


test_cpp_args = [
    '-DDOCTEST_CONFIG_REQUIRE_STRINGIFICATION_FOR_ALL_USED_TYPES',
    '-DDOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING',
    '-DDOCTEST_CONFIG_USE_STD_HEADERS',
]

test_cpp_args += internal_cpp_args


test_files_manual = [
    'json.cpp',
]

if get_option('b_coverage')


    if allocators_used != 'tjson_coverage_allocators'
        error('need custom allocators for the json oom test')
    endif

    test_files_manual += 'json_oom.cpp'
    test_src += files(
        'helpers/allocators.cpp',
        'helpers/allocators.hpp',
    )

endif

workdir = meson.project_source_root() / 'tests' / 'files'

foreach test_file : test_files_manual

    test_name = test_file.replace('.cpp', '').replace('/', '_')

    this_test = executable(
        test_name,
        test_src,
        files(test_file),
        include_directories: test_inc_dirs,
        dependencies: test_deps,
        cpp_args: test_cpp_args,
        c_args: internal_c_args,
        override_options: {
            'warning_level': '3',
            'werror': true,
            'b_coverage': false,
            'cpp_std': cpp_std_to_use,
        },
    )

    test(
        test_name,
        this_test,
        protocol: 'exitcode',
        workdir: workdir,
        is_parallel: true,
        timeout: 30,
    )

endforeach

all_tests = executable(
    'all_tests',
    test_src,
    files(test_files_manual),
    include_directories: test_inc_dirs,
    dependencies: test_deps,
    cpp_args: test_cpp_args,
    c_args: internal_c_args,
    override_options: {
        'warning_level': '3',
        'werror': true,
        'b_coverage': false,
        'cpp_std': cpp_std_to_use,
    },
)

if get_option('b_coverage')
    # check if we have gcovr >= 8.4, as this is needed for the 'GCOVR_EXCL_BR_WITHOUT_HIT: x/y' feature
    gocvr = find_program(
        'gcovr',
        version: ['>=8.4', '<9'],
    )

    # GCOVR_EXCL_BR_SOURCE was added in gcovr 8.0, but requries gcc to have enough information, to use it! (it need branch to destination mapping (especially for switch cases), only gcc produces)
    if cc.get_id() != 'gcc' or cpp.get_id() != 'gcc'
        error(
            'we only support gcc for builds with coverage, as we use a gcovr gcc only feature: GCOVR_EXCL_BR_SOURCE'
        )
    endif

    if cc.version().version_compare('<14') or cpp.version().version_compare('<14')
        error('require gcc >= 14 for the gcovr feature: GCOVR_EXCL_BR_SOURCE')

    endif

endif
