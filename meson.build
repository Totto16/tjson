project(
    'tjson',
    'c',
    license: 'MIT',
    meson_version: '>=1.10.0',
    default_options: {
        'buildtype': 'release',
        'optimization': '3',
        'c_std': ['gnu23', 'gnu2x'],
        'warning_level': '3',
        'werror': 'true',
        'b_ndebug': 'if-release',
    },
    version: '1.0.1',
)

deps = []
compile_flags = []
public_deps = []

allocators_used = ''
internal_c_args = []
src_files = []
inc_dirs = []


if get_option('tests') and get_option('b_coverage')
    allocators_used = 'tjson_coverage_allocators'
    internal_c_args += '-DTJSON_ALLOCATOR_PREFIX=tjson_coverage_allocator'
    subdir('src/_impl/allocators')
endif

# allocators are not used, as this takes in buffers and never creates or frees them
deps += dependency(
    'libutf8proc',
    allow_fallback: true,
    default_options: {
        'tests': 'disabled',
    },
    version: ['>=2.0.0', '<3'],
)

# allocators are defined, as this is a header only library
deps += dependency(
    'tvec.h',
    allow_fallback: true,
    version: ['>=1.0.0', '<2'],
    default_options: {
        'static_string_modification_behavior': 'error',
    },
)

# allocators are defined, as this is a header only library
# the lib only includes some helper function for hashes
deps += dependency(
    'tmap.h',
    allow_fallback: true,
    version: ['>=1.0.0', '<2'],
)

public_deps += dependency(
    'tstr.h',
    allow_fallback: true,
    default_options: {
        'use_allocators': 'allocators_used',
    },
    version: ['>=1.0.0', '<2'],
)

deps += dependency(
    'tstr_builder',
    allow_fallback: true,
    version: ['>=1.0.0', '<2'],
    default_options: {
        'use_allocators': 'allocators_used',
    },
)

# allocators are defined, as this is a header only library
deps += dependency(
    'trc',
    allow_fallback: true,
    version: ['>=1.0.0', '<2'],
)

cc = meson.get_compiler('c')

deps += cc.find_library(
    'm',
    required: false,
)

internal_c_args += ['-D_TJSON_IMPL_INTERNAL__', '-D__IMPL_TJSON_COMPILE_LIB__INTERNAL']


internal_c_args += cc.get_supported_arguments(
    '-Wassign-enum',
    '-Wenum-conversion',
    '-Wenum-switch',
    '-fstrict-enums',
    '-Wenum-identifier',
    '-Wenum-compare',
    '-Wenum-enum-conversion',
    '-Wenum-float-conversion',
    '-Wenum-too-large',
    '-Wenum-compare-switch',
    '-Wenum-compare-conditional',
    '-fno-strict-aliasing',
    '-Wpointer-arith',
    '-Wmissing-declarations',
    '-Wformat=2',
    '-Wstrict-prototypes',
    '-Wmissing-prototypes',
    '-Wnested-externs',
    '-Wold-style-definition',
    '-Wundef',
    '-Wunused',
    '-Wcast-align',
    '-Wmissing-noreturn',
    '-Wmissing-format-attribute',
    '-Wmissing-include-dirs',
    '-Wlogical-op',
    '-Wignored-qualifiers',
    '-Werror=redundant-decls',
    '-Werror=implicit',
    '-Werror=nonnull',
    '-Werror=init-self',
    '-Werror=main',
    '-Werror=missing-braces',
    '-Werror=sequence-point',
    '-Werror=return-type',
    '-Werror=trigraphs',
    '-Werror=array-bounds',
    '-Werror=write-strings',
    '-Werror=address',
    '-Werror=int-to-pointer-cast',
    '-Werror=pointer-to-int-cast',
    '-Werror=empty-body',
    '-Werror=write-strings',
    '-Werror=strict-aliasing',
    '-Wsign-compare',
    '-Wno-cast-function-type',
    '-Wunused-parameter',
    '-Wmissing-field-initializers',
    '-Wtype-limits',
    '-Wshadow',
    '-Wfloat-conversion',
    '-Wno-nullability-extension',
    '-Wno-nullability-completeness',
    '-Wcast-qual',
    '-Wwrite-strings',
    '-Wdiscarded-qualifiers',
    '-Wconversion',
    '-Wsign-conversion',
    '-Waddress',
    '-Wfloat-equal',
)

if cc.get_id() == 'gcc'
    internal_c_args += cc.get_supported_arguments('-Wno-format-nonliteral')

    if get_option('buildtype') == 'release'
        # wait for the fix of https://gcc.gnu.org/bugzilla/show_bug.cgi?id=124151
        #to land in the gcc the Ci uses, after that disable that
        internal_c_args += cc.get_supported_arguments('-fno-tree-sra')
    endif

endif


subdir('src')

public_deps += json_variants_dep

tjson_lib = library(
    'tjson',
    src_files,
    dependencies: [deps, public_deps],
    include_directories: inc_dirs,
    c_args: [compile_flags, internal_c_args],
    install: true,
)

tjson_dep = declare_dependency(
    link_with: tjson_lib,
    include_directories: inc_dirs,
    compile_args: [compile_flags],
    dependencies: public_deps,
    version: meson.project_version(),
)

meson.override_dependency('tjson', tjson_dep)

## mark as unusable
compile_flags = {
    'don\'t_use': true,
}


if get_option('tests')
    subdir('tests')
endif
