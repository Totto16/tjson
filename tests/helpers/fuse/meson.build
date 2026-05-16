


test_src += files(
    'fuse_impl.c',
    'fuse_impl.h',
)

shared_files = files(
    'fuse_shared.c',
    'fuse_shared.h',
)
shared_deps = [tvec_dep]

test_src += shared_files


c_variant_generator = find_program(
    'c_variant_generator',
    version: ['>=1.0.0', '<2'],
)

fuse_variants_generated_tgt = custom_target(
    'json_variants',
    command: [
        c_variant_generator,
        '-o',
        '@OUTPUT@',
        '-i',
        '@INPUT@',
    ],
    input: [meson.project_source_root() / 'tests' / 'helpers' / 'fuse' / 'fuse.variants.def.json'],
    output: ['fuse_variants.h'],
    build_subdir: 'generated' / 'fuse' / 'variants',
    install: false,
    ##TODO: wait for meson 1.11.2
    # depends: [c_variant_generator],
)

fuse_variants_flags = []

cc = meson.get_compiler('c')

stdc_version = cc.get_define('__STDC_VERSION__ ')
stdc_version_int = stdc_version.replace('L', '').to_int()
if stdc_version_int >= 202000
    fuse_variants_flags += '-D_FUSE_HELPER_COMPILE_WITH_NARROWED_ENUMS'
endif

fuse_variants_dep = declare_dependency(
    version: meson.project_version(),
    sources: [fuse_variants_generated_tgt],
    compile_args: fuse_variants_flags,
)


shared_deps += fuse_variants_dep

fuse_helper_deps = []

fuse_helper_deps += dependency(
    'fuse3',
    version: ['>=3.0', '<4'],
)

fuse_helper_deps += tstr_dep
fuse_helper_deps += trtti_dep


fuse_helper_deps += shared_deps

fuse_helper_exe = executable(
    'fuse_helper_exe',
    files(
        'fuse_helper_main.c',
        'fuse_impl_run.c',
        'fuse_impl_run.h',
    ),
    shared_files,
    include_directories: include_directories('.'),
    dependencies: fuse_helper_deps,
    c_args: internal_c_args,
    override_options: {
        'warning_level': '3',
        'werror': true,
        'b_coverage': false,
    },
)

conf_data = configuration_data()
conf_data.set('FUSE_PROGRAM_PATH', fuse_helper_exe.full_path())

fuse_helper_h = configure_file(
    input: 'fuse_helper.h.in',
    output: 'fuse_helper.h',
    configuration: conf_data,
)

test_src += fuse_helper_h
