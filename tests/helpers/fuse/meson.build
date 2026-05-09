


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
