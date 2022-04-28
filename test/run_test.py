import os
import os.path
from os.path import join
from os.path import relpath
from os.path import abspath
import sys
import subprocess
import shutil
import types
import pickle
import platform

sdir = os.path.dirname(abspath(__file__))
home = os.path.dirname(sdir)
test = abspath(join(home, '..', 'libcxx/test'))
build = join(home, 'build')

include_paths = [
    join(home, 'include'),
    join(test, 'support')
]

if platform.system() == 'Windows':
    compiler = 'clang-cl'
    cflags = [
        '/DTEST_STD_VER=17',
        '/DALA_USE_ALLOC_REBIND=1',
        '/Od',
        '/Z7',
        '/std:c++latest',
        '/EHsc',
        '-stdlib=libc++',
        '-fuse-ld=lld-link',
        '-fsanitize=address',
        '-fsanitize=undefined',
        '-ferror-limit=1'
    ]
else:
    compiler = 'clang++'
    cflags = [
        '-DTEST_STD_VER=20',
        '-DALA_USE_ALLOC_REBIND=1',
        '-D_ALA_VERSION=0',
        '-O0',
        '-g',
        '-std=c++20',
        '-fexceptions',
        '-stdlib=libc++',
        '-fuse-ld=lld',
        # '-fsanitize=address',
        # '-fsanitize=undefined',
        # '-fsanitize=thread',
        # '-fsanitize=memory',
        '-ferror-limit=1'
    ]

lflags = []
srcs = [
    # 'std/algorithms',
    # 'std/containers/container.node',
    # 'std/containers/container.general',
    # 'std/containers/container.requirements',
    # 'std/containers/associative/map',
    # 'std/containers/associative/multimap',
    # 'std/containers/associative/set',
    # 'std/containers/associative/multiset',
    # 'std/containers/sequences/array',
    # 'std/containers/sequences/vector',
    # 'std/containers/sequences/list',
    # 'std/utilities/meta',
    # 'std/utilities/function.objects',
    # 'std/utilities/utility',
    # 'std/utilities/tuple',
    # 'std/utilities/any',
    # 'std/utilities/variant',
    # 'std/utilities/optional',
    'std/utilities/smartptr',
    'std/utilities/memory/util.smartptr',
]

skips = [
    # should fix
    # 'std/utilities/tuple/tuple.tuple/tuple.cnstr/PR31384.pass.cpp',
    'std/containers/sequences/vector/vector.modifiers/resize_not_move_insertable.fail.cpp',
    'std/containers/associative/multiset/emplace_hint.pass.cpp',
    'std/utilities/meta/meta.trans/meta.trans.sign/make_signed.pass.cpp',
    'std/utilities/meta/meta.trans/meta.trans.sign/make_unsigned.pass.cpp',
    'std/utilities/meta/meta.const.eval/is_constant_evaluated.pass.cpp',
    'std/utilities/meta/meta.const.eval/is_constant_evaluated.fail.cpp',

    # only C++03
    'std/utilities/utility/pairs/pairs.pair/assign_pair_cxx03.pass.cpp',

    # only C++11
    'std/utilities/utility/pairs/pairs.pair/not_constexpr_cxx11.fail.cpp',

    # uncompat with asan
    # 'std/containers/sequences/list/list.ops/sort_comp.pass.cpp',
    # 'std/containers/associative/map/map.access/at.pass.cpp',
    'std/containers/sequences/list/list.modifiers/insert_iter_size_value.pass.cpp',
    'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/pointer_deleter_throw.pass.cpp',
    'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/pointer_throw.pass.cpp',
    'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/nullptr_t_deleter_throw.pass.cpp',


    # only warning
    'std/containers/sequences/array/empty.fail.cpp',
    'std/containers/sequences/list/list.capacity/empty.fail.cpp',
    'std/containers/sequences/vector/vector.capacity/empty.fail.cpp',
    'std/containers/associative/map/map.access/empty.fail.cpp',
    'std/containers/associative/multimap/empty.fail.cpp',
    'std/containers/associative/set/empty.fail.cpp',
    'std/containers/associative/multiset/empty.fail.cpp',

    # compiler crash
    'std/algorithms/alg.modifying.operations/alg.partitions/stable_partition.pass.cpp',

    # no imple
    'std/utilities/function.objects/func.search',
    'std/utilities/memory/util.smartptr/util.smartptr.shared.atomic',
    'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.io',
    'std/containers/container.requirements/container.requirements.general/allocator_move.pass.cpp',

    # deprecated
    'std/utilities/smartptr/unique.ptr/unique.ptr.class/unique.ptr.ctor/auto_pointer.pass.cpp',
    'std/utilities/function.objects/refwrap/weak_result.pass.cpp',
    'std/utilities/function.objects/negators',
    'std/utilities/function.objects/func.wrap/func.wrap.func/types.pass.cpp',
    'std/utilities/function.objects/func.wrap/func.wrap.func/func.wrap.func.con/alloc',
    'std/utilities/function.objects/func.require/binary_function.pass.cpp',
    'std/utilities/function.objects/func.require/unary_function.pass.cpp',
    'std/utilities/tuple/tuple.tuple/tuple.cnstr/alloc',
    'std/utilities/tuple/tuple.tuple/tuple.traits/uses_allocator.pass.cpp',
    'std/algorithms/alg.modifying.operations/alg.random.shuffle/random_shuffle_rand.pass.cpp',
    'std/algorithms/alg.modifying.operations/alg.random.shuffle/random_shuffle.pass.cpp',
    'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.assign/auto_ptr_Y.pass.cpp',
    'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/auto_ptr.pass.cpp'
]

srcs = [i.replace('/', os.sep) for i in srcs]
skips = [i.replace('/', os.sep) for i in skips]


def pn(fname, rel=None):
    if rel is None:
        fname = abspath(fname)
    else:
        fname = os.path.relpath(fname, rel)
    m = types.SimpleNamespace()
    m.path = fname
    m.dir, m.name = os.path.split(fname)
    m.base, m.ext = os.path.splitext(m.name)
    return m


def output_exe(fname):
    N = pn(fname, test)
    out_path = join(build, N.dir)
    out = join(out_path, N.base)
    if platform.system() == 'Windows':
        out += '.exe'
    if not os.path.exists(out_path):
        os.makedirs(out_path)
    return out


def output_src(fname):
    N = pn(fname, test)
    out_path = join(build, N.dir)
    out = join(out_path, N.base + N.ext)
    if not os.path.exists(out_path):
        os.makedirs(out_path)
    return out


def preprocess_cmd(fname, output=None):
    N = pn(fname)
    args = []
    args += cflags
    for i in include_paths:
        args.append('-I{}'.format(i))
    args.append('-I{}'.format(N.dir))
    args.append(output_src(fname))
    if output is None:
        if platform.system() == 'Windows':
            args.append('/link')
            args += lflags
            args.append('/out:{}'.format(output_exe(fname)))
            # args.append('/Fe{}'.format(output_exe(fname)))
        else:
            args.append('-o{}'.format(output_exe(fname)))
            if len(lflags) > 0:
                args.append('-Wl,' + ','.join(lflags))
    return args


def preprocess_file(fname):
    output = output_src(fname)
    with open(fname, mode='r') as R:
        with open(output, mode='w') as W:
            fstr = R.read()
            fstr = fstr.replace(
                'ASSERT_SAME_TYPE(void, decltype(std::erase(s, val)));', '// ASSERT_SAME_TYPE(typename M::size_type, decltype(std::erase(s, val)));').replace(
                'ASSERT_SAME_TYPE(void, decltype(std::erase_if(s, p)));', '// ASSERT_SAME_TYPE(typename M::size_type, decltype(std::erase_if(s, p)));').replace(
                '#include <any>', '#include <ala/any.h>').replace(
                '#include <algorithm>', '#include <ala/algorithm.h>\n#include <ala/vector.h>').replace(
                '#include <array>', '#include <ala/array.h>').replace(
                '#include <functional>', '#include <ala/functional.h>').replace(
                '#include <iterator>', '#include <ala/iterator.h>').replace(
                '#include <random>', '#include <ala/random.h>').replace(
                '#include <vector>', '#include <ala/vector.h>').replace(
                '#include <optional>', '#include <ala/optional.h>').replace(
                '#include <variant>', '#include <ala/variant.h>').replace(
                '#include <list>', '#include <ala/list.h>').replace(
                '#include <map>', '#include <ala/map.h>').replace(
                '#include <set>', '#include <ala/set.h>').replace(
                '#include <utility>', '#include <ala/utility.h>').replace(
                '#include <tuple>', '#include <ala/tuple.h>').replace(
                '#include <type_traits>', '#include <ala/type_traits.h>').replace(
                '#include <memory>', '#include <memory>\n#include <ala/memory.h>').replace(
                'std::string', 'ALASTD::string').replace(
                'std::pow', 'ALASTD::pow').replace(
                'std::cout', 'ALASTD::cout').replace(
                'std::endl', 'ALASTD::endl').replace(
                'std::complex', 'ALASTD::complex').replace(
                # 'std::allocator', 'ALASTD::allocator').replace(
                'std::mt19937 randomness', 'ala::xoshiro256ss randomness{12345}').replace(
                'std::', 'ala::').replace(
                'assert(distance(', 'assert(ala::distance(').replace(
                'assert(*next(', 'assert(*ala::next(').replace(
                'namespace std {', 'namespace ala {').replace(
                'namespace std\n{', 'namespace ala {').replace(
                'using namespace std', 'using namespace ala').replace(
                'ALASTD::', 'std::')
            W.write(fstr)

    return output


OK = []


def do_test_pass(fname, run=True):
    global OK
    print(abspath(fname).replace(os.sep, '/') + ': ', end='', flush=True)
    try:
        src = preprocess_file(fname)
        cmd = [compiler] + preprocess_cmd(fname)
        process = subprocess.Popen(cmd)
        process.wait()
        if process.returncode != 0:
            print('\033[31mFAILED!\033[0m')
        elif run:
            try:
                process = subprocess.Popen([output_exe(src)])
                process.wait()
                if process.returncode != 0:
                    print('\033[31mFAILED!\033[0m')
                else:
                    print('\033[32mSUCCESS!\033[0m')
                    OK.append(fname)
            except Exception as e:
                print(e)
                print('\033[31mFAILED!\033[0m')
    except Exception as e:
        print(e)
        print('\033[31mFAILED!\033[0m')


def do_test_fail(fname):
    global OK
    print(abspath(fname).replace(os.sep, '/') + ': ', end='', flush=True)
    try:
        src = preprocess_file(fname)
        cmd = [compiler] + preprocess_cmd(fname)
        with open(os.devnull, 'w') as null:
            process = subprocess.Popen(cmd, stdout=null, stderr=null)
            process.wait()
            if process.returncode != 0:
                print('\033[32mSUCCESS!\033[0m')
                OK.append(fname)
            else:
                print('\033[31mFAILED!\033[0m')
    except Exception as e:
        print('\033[31mFAILED!\033[0m')


def do_test(fname, run=True):
    global skips
    if any(relpath(fname, test).startswith(i) for i in skips):
        return
    if any(fname.lower() == i.lower() for i in OK):
        return
    N = pn(fname)

    if (N.base.endswith('.pass')):
        do_test_pass(fname, run)
    elif (N.base.endswith('.fail')):
        do_test_fail(fname)


def main():
    global OK
    global srcs
    okbin = join(build, 'ok.bin')
    if (len(sys.argv) == 2):
        srcs = sys.argv[1:]
        srcs = [abspath(i) for i in srcs]
    elif os.path.exists(okbin):
        with open(okbin, 'rb') as okf:
            OK = pickle.load(okf)
    else:
        OK = []

    os.chdir(test)
    for src in srcs:
        if os.path.isdir(src):
            for root, dirs, files in os.walk(src):
                for f in files:
                    do_test(join(root, f))
        elif os.path.isfile(src):
            do_test(src)
    with open(okbin, 'wb') as okf:
        pickle.dump(OK, okf)


if __name__ == '__main__':
    main()
