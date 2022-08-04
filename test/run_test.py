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
test = abspath(join(home, '..', 'libcxx-13/test'))
build = join(home, 'build')

include_paths = [
    join(home, 'include'),
    join(test, 'support')
]

# if platform.system() == 'Windows':
if False:
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
        '-DTEST_STD_VER=17',
        '-DALA_USE_ALLOC_REBIND=1',
        '-D_ALA_VERSION=0',
        '-O2',
        '-g0',
        '-std=c++17',
        '-fexceptions',
        '-stdlib=libc++',
        '-fuse-ld=lld',
        '-fsanitize=address',
        '-fsanitize=undefined',
        # '-fsanitize=thread',
        # '-fsanitize=memory',
        '-ferror-limit=1',
    ]

lflags = [
    '-rpath',
    '/opt/llvm/lib/x86_64-unknown-linux-gnu/',
]
srcs = [
    # 'std/algorithms',
    # 'std/containers/container.node',
    # 'std/containers/containers.general',
    # 'std/containers/container.requirements',
    # 'std/containers/associative/map',
    # 'std/containers/associative/multimap',
    # 'std/containers/associative/set',
    # 'std/containers/associative/multiset',
    # 'std/containers/sequences/array',
    # 'std/containers/sequences/vector',
    # 'std/containers/sequences/deque',
    # 'std/containers/sequences/list',
    # 'std/containers/sequences/forwardlist',
    # 'std/containers/container.adaptors/stack',
    # 'std/containers/container.adaptors/queue',
    # 'std/containers/container.adaptors/priority.queue',
    'std/containers/views',
    # 'std/utilities/meta',
    # 'std/utilities/function.objects',
    # 'std/utilities/utility',
    # 'std/utilities/tuple',
    # 'std/utilities/any',
    # 'std/utilities/variant',
    # 'std/utilities/optional',
    # 'std/utilities/smartptr',
    # 'std/utilities/memory/util.smartptr',
    # 'std/concepts',
]


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


skips = [
    # should fix
    # 'std/utilities/tuple/tuple.tuple/tuple.cnstr/PR31384.pass.cpp',
    'std/containers/sequences/vector/vector.modifiers/resize_not_move_insertable.fail.cpp',
    'std/containers/associative/multiset/emplace_hint.pass.cpp',
    'std/utilities/meta/meta.trans/meta.trans.sign/make_signed.pass.cpp',
    'std/utilities/meta/meta.trans/meta.trans.sign/make_unsigned.pass.cpp',
    # 'std/utilities/variant/variant.visit/robust_against_adl.pass.cpp',
    'std/utilities/meta/meta.trans/meta.trans.other/aligned_storage.pass.cpp',

    # only C++03
    'std/utilities/utility/pairs/pairs.pair/assign_pair_cxx03.pass.cpp',

    # only C++11
    'std/utilities/utility/pairs/pairs.pair/not_constexpr_cxx11.fail.cpp',

    # only warning
    'std/containers/sequences/array/empty.fail.cpp',
    'std/containers/sequences/list/list.capacity/empty.fail.cpp',
    'std/containers/sequences/vector/vector.capacity/empty.fail.cpp',
    'std/containers/associative/map/map.access/empty.fail.cpp',
    'std/containers/associative/multimap/empty.fail.cpp',
    'std/containers/associative/set/empty.fail.cpp',
    'std/containers/associative/multiset/empty.fail.cpp',

    # compiler crash
    # 'std/algorithms/alg.modifying.operations/alg.partitions/stable_partition.pass.cpp',

    # no imple
    'std/utilities/function.objects/func.search',
    'std/utilities/memory/util.smartptr/util.smartptr.shared.atomic',
    'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.io',
    # 'std/containers/container.requirements/container.requirements.general/allocator_move.pass.cpp',
    'std/utilities/meta/meta.trans/meta.trans.other/common_reference.compile.pass.cpp',
    lambda x: pn(x).name == 'iterator_concept_conformance.compile.pass.cpp' or pn(
        x).name == 'range_concept_conformance.compile.pass.cpp',
    'std/utilities/meta/meta.unary/meta.unary.prop/is_scoped_enum.pass.cpp',
    'std/utilities/meta/meta.const.eval/is_constant_evaluated.pass.cpp',
    'std/utilities/meta/meta.const.eval/is_constant_evaluated.fail.cpp',
    'std/utilities/function.objects/func.identity/identity.pass.cpp',
    'std/utilities/function.objects/range.cmp',
    'std/utilities/utility/utility.intcmp',
    'std/utilities/utility/utility.underlying/to_underlying.pass.cpp',
    'std/containers/sequences/deque/deque.modifiers/push_back_exception_safety.pass.cpp',
    'std/containers/sequences/deque/deque.modifiers/push_front_exception_safety.pass.cpp',
    'std/containers/sequences/deque/deque.cons/deduct.pass.cpp',


    # not deprecated
    'std/utilities/meta/meta.unary/meta.unary.prop/is_literal_type.deprecated.fail.cpp',
    'std/utilities/meta/meta.trans/meta.trans.other/result_of.deprecated.fail.cpp',

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
    'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/auto_ptr.pass.cpp',
    'std/utilities/smartptr/unique.ptr/unique.ptr.class/unique.ptr.ctor/auto_pointer.pass.cpp'
]

if '-fsanitize=address' in cflags:
    skips += [
        # uncompat with asan
        'std/containers/sequences/list/list.modifiers/insert_iter_size_value.pass.cpp',
        'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/pointer_deleter_throw.pass.cpp',
        'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/pointer_throw.pass.cpp',
        'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/nullptr_t_deleter_throw.pass.cpp',
        'std/utilities/memory/util.smartptr/util.smartptr.shared/util.smartptr.shared.const/unique_ptr.pass.cpp',

    ]


srcs = [i.replace('/', os.sep) if isinstance(i, str) else i for i in srcs]
skips = [i.replace('/', os.sep) if isinstance(i, str) else i for i in skips]


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


def preprocess_cmd(fname):
    N = pn(fname)
    args = []
    args += cflags
    for i in include_paths:
        args.append('-I{}'.format(i))
    args.append('-I{}'.format(N.dir))
    args.append(output_src(fname))
    if N.base.endswith('compile.pass'):
        args.append('-c')
        args.remove('-fuse-ld=lld')
    else:
        # if platform.system() == 'Windows':
        if False:
            args.append('/link')
            args += lflags
            args.append('/out:{}'.format(output_exe(fname)))
            # args.append('/Fe{}'.format(output_exe(fname)))
        else:
            args.append('-o{}'.format(output_exe(fname)))
            if len(lflags) > 0:
                args.append('-Wl,' + ','.join(lflags))
    return args


def patch(str):
    return str.replace(
        '#include <any>', '#include <ala/any.h>').replace(
        '#include <algorithm>', '#include <ala/algorithm.h>\n#include <ala/vector.h>\n#include <random>').replace(
        '#include <array>', '#include <ala/array.h>').replace(
        '#include <functional>', '#include <ala/functional.h>').replace(
        '#include <iterator>', '#include <ala/iterator.h>').replace(
        '#include <random>', '#include <random>\n#include <ala/random.h>').replace(
        '#include <vector>', '#include <ala/vector.h>\n#include <ala/functional.h>').replace(
        '#include <deque>', '#include <ala/deque.h>').replace(
        '#include <stack>', '#include <ala/stack.h>\n#include <ala/deque.h>').replace(
        '#include <queue>', '#include <ala/queue.h>\n#include <ala/deque.h>').replace(
        '#include <optional>', '#include <ala/optional.h>').replace(
        '#include <variant>', '#include <ala/variant.h>\n#include <ala/tuple.h>').replace(
        '#include <list>', '#include <ala/list.h>').replace(
        '#include <map>', '#include <ala/map.h>').replace(
        '#include <set>', '#include <ala/set.h>').replace(
        '#include <utility>', '#include <ala/utility.h>').replace(
        '#include <tuple>', '#include <ala/tuple.h>').replace(
        '#include <type_traits>', '#include <ala/type_traits.h>').replace(
        '#include <memory>', '#include <memory>\n#include <ala/memory.h>').replace(
        '#include <forward_list>', '#include <ala/forward_list.h>').replace(
        '#include <span>', '#include <ala/span.h>').replace(
        '#include <concepts>', '#include <ala/concepts.h>').replace(
        # '#include <ranges>', '#include <ala/ranges.h>').replace(
        'std::string', 'ALASTD::string').replace(
        'std::wstring', 'ALASTD::wstring').replace(
        'std::u8string', 'ALASTD::u8string').replace(
        'std::u16string', 'ALASTD::u16string').replace(
        'std::u32string', 'ALASTD::u32string').replace(
        'std::printf', 'ALASTD::printf').replace(
        'std::pow', 'ALASTD::pow').replace(
        'std::cout', 'ALASTD::cout').replace(
        'std::endl', 'ALASTD::endl').replace(
        'std::complex', 'ALASTD::complex').replace(
        'std::mt19937', 'ALASTD::mt19937').replace(
        'std::mt19937_64', 'ALASTD::mt19937_64').replace(
        'std::uniform_int_distribution', 'ALASTD::uniform_int_distribution').replace(
        'std::unordered_map', 'ALASTD::unordered_map').replace(
        'std::unordered_set', 'ALASTD::unordered_set').replace(
        'std::unordered_multimap', 'ALASTD::unordered_multimap').replace(
        'std::unordered_multiset', 'ALASTD::unordered_multiset').replace(
        'std::lock_guard', 'ALASTD::lock_guard').replace(
        'std::mutex', 'ALASTD::mutex').replace(
        'std::basic_string', 'ALASTD::basic_string').replace(
        'std::', 'ala::').replace(
        'assert(distance(', 'assert(ala::distance(').replace(
        'assert(*next(', 'assert(*ala::next(').replace(
        'namespace std {', 'namespace ala {').replace(
        'namespace std\n{', 'namespace ala {').replace(
        'using namespace std', 'using namespace ala').replace(
        'ALASTD::', 'std::')


def preprocess_file(fname):
    output = output_src(fname)
    with open(fname, mode='r', encoding='utf-8') as R:
        with open(output, mode='w') as W:
            fstr = R.read()
            fstr = patch(fstr)
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
        elif not run:
            print('\033[32mSUCCESS!\033[0m')
            OK.append(fname)
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
    filters = [f for f in skips if callable(f)]
    if any(relpath(fname, test).startswith(i)
           for i in skips if isinstance(i, str)):
        return
    if any(fname.lower() == i.lower() for i in OK):
        return
    if any(f(fname) for f in filters):
        return
    N = pn(fname)

    if (N.base.endswith('compile.pass')):
        do_test_pass(fname, False)
    elif (N.base.endswith('.pass')):
        do_test_pass(fname, run)
    elif (N.base.endswith('.fail')):
        do_test_fail(fname)


def main():
    global OK
    global srcs
    okbin = join(build, 'ok.bin')
    if (len(sys.argv) == 2):
        srcs = sys.argv[1:]
        srcs = [i.replace(os.sep, '/') for i in srcs]

    if os.path.exists(okbin):
        with open(okbin, 'rb') as okf:
            OK = pickle.load(okf)

    os.chdir(test)
    for src in srcs:
        if os.path.isdir(src):
            for root, dirs, files in os.walk(src):
                for f in files:
                    # print(join(root, f))
                    do_test(join(root, f))
        elif os.path.isfile(src):
            do_test(src)
    with open(okbin, 'wb') as okf:
        pickle.dump(OK, okf)


if __name__ == '__main__':
    main()
