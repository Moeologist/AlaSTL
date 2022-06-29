#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##
"""GDB pretty-printers for libc++.

These should work for objects compiled when _LIBCPP_ABI_UNSTABLE is defined
and when it is undefined.
"""

from __future__ import print_function

import math
import re
import gdb

# One under-documented feature of the gdb pretty-printer API
# is that clients can call any other member of the API
# before they call to_string.
# Therefore all self.FIELDs must be set in the pretty-printer's
# __init__ function.

_void_pointer_type = gdb.lookup_type("void").pointer()


_long_int_type = gdb.lookup_type("unsigned long long")

_libcpp_big_endian = False

def addr_as_long(addr):
    return int(addr.cast(_long_int_type))


# The size of a pointer in bytes.
_pointer_size = _void_pointer_type.sizeof


def _remove_cxx_namespace(typename):
    """Removed libc++ specific namespace from the type.

    Arguments:
      typename(string): A type, such as std::__u::something.

    Returns:
      A string without the libc++ specific part, such as std::something.
    """

    return re.sub("std::__.*?::", "std::", typename)


def _remove_generics(typename):
    """Remove generics part of the type. Assumes typename is not empty.

    Arguments:
      typename(string): A type such as std::my_collection<element>.

    Returns:
      The prefix up to the generic part, such as std::my_collection.
    """

    match = re.match("^([^<]+)", typename)
    return match.group(1)


# Some common substitutions on the types to reduce visual clutter (A user who
# wants to see the actual details can always use print/r).
_common_substitutions = [
    ("std::basic_string<char, std::char_traits<char>, std::allocator<char> >",
     "std::string"),
    ("std::basic_string_view<char, std::char_traits<char> >",
     "std::string_view"),
]


def _prettify_typename(gdb_type):
    """Returns a pretty name for the type, or None if no name can be found.

    Arguments:
      gdb_type(gdb.Type): A type object.

    Returns:
      A string, without type_defs, libc++ namespaces, and common substitutions
      applied.
    """

    type_without_typedefs = gdb_type.strip_typedefs()
    typename = type_without_typedefs.name or type_without_typedefs.tag or \
        str(type_without_typedefs)
    result = _remove_cxx_namespace(typename)
    for find_str, subst_str in _common_substitutions:
        result = re.sub(find_str, subst_str, result)
    return result


def _typename_for_nth_generic_argument(gdb_type, n):
    """Returns a pretty string for the nth argument of the given type.

    Arguments:
      gdb_type(gdb.Type): A type object, such as the one for std::map<int, int>
      n: The (zero indexed) index of the argument to return.

    Returns:
      A string for the nth argument, such a "std::string"
    """
    element_type = gdb_type.template_argument(n)
    return _prettify_typename(element_type)


def _typename_with_n_generic_arguments(gdb_type, n):
    """Return a string for the type with the first n (1, ...) generic args."""

    base_type = _remove_generics(_prettify_typename(gdb_type))
    arg_list = [base_type]
    template = "%s<"
    for i in range(n):
        arg_list.append(_typename_for_nth_generic_argument(gdb_type, i))
        template += "%s, "
    result = (template[:-2] + ">") % tuple(arg_list)
    return result


def _typename_with_first_generic_argument(gdb_type):
    return _typename_with_n_generic_arguments(gdb_type, 1)


class StdTuplePrinter(object):
    """Print a std::tuple."""

    class _Children(object):
        """Class to iterate over the tuple's children."""

        def __init__(self, val):
            self.val = val
            self.child_iter = iter(self.val["_impl"].type.fields())
            self.count = 0

        def __iter__(self):
            return self

        def __next__(self):
            # child_iter raises StopIteration when appropriate.
            field_name = next(self.child_iter)
            tuple_base = self.val["_impl"][field_name]
            try:
                child = tuple_base["value"]
            except:
                child = tuple_base[tuple_base.type.fields()[0]]
            self.count += 1
            return ("[%d]" % self.count, child)

        # TODO Delete when we drop Python 2.
        def next(self):
            return self.__next__()

    def __init__(self, val):
        self.val = val

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if not self.val.type.fields():
            return "empty %s" % typename
        return "%s containing" % typename

    def children(self):
        if not self.val.type.fields():
            return iter(())
        return self._Children(self.val)


class StdVariantPrinter(object):
    """Print a std::variant."""

    def __init__(self, val):
        self.val = val
        self.index = self.val["_index"]
        self.union = self.val["_union"]
        try:
            self.holdtype = self.val.type.template_argument(self.index)
        except:
            self.holdtype = None
        self.addr = self.union.address.cast(self.holdtype.pointer())

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if self.holdtype is None:
            return "valueless %s" % typename
        return "%s containing" % typename

    def __iter__(self):
        if self.holdtype:
            yield "[%s]" % _prettify_typename(self.holdtype), self.addr.dereference()

    def children(self):
        return self


class StdOptionalPrinter(object):
    """Print a std::optional."""

    def __init__(self, val):
        self.val = val
        self.has = self.val["_valid"]
        self.value = self.val["_value"]
        self.holdtype = self.val.type.template_argument(0)

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if not self.has:
            return "empty %s" % typename
        return "%s containing" % typename

    def __iter__(self):
        if self.has:
            yield "[%s]" % _prettify_typename(self.holdtype), self.value

    def children(self):
        return self


class StdAnyPrinter(object):
    """Print a std::any."""

    def __init__(self, val):
        self.val = val
        self.handle = self.val["_op_handle"]
        if self.handle == 0:
            return
        try:
            self.hname = gdb.block_for_pc(int(self.handle.cast(gdb.lookup_type('intptr_t')))).function.name
            regex = r"""^ala::_any_handle<(.*)>::operate\(ala::AnyOP\)"""
            m = re.match(regex, self.hname)
            self.holdname = m.group(1)
            localsym = gdb.lookup_symbol(self.hname.replace("operate\(ala::AnyOP\)", "_is_local"))[0]
            self.local = localsym.value()
            self.holdtype = gdb.lookup_type(self.holdname)
            if self.local:
                self.addr = self.val["_placehold"].address.cast(self.holdtype.pointer())
            else:
                self.addr = self.val["_placehold"][0].cast(self.holdtype.pointer())
        except:
            self.handle = 0

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if self.handle == 0:
            return "empty %s" % typename
        return "%s containing" % typename

    def __iter__(self):
        if self.handle != 0:
            yield "[%s]" % _prettify_typename(self.holdtype), self.addr.dereference()

    def children(self):
        return self


class StdArrayPrinter(object):
    """Print a std::array."""

    def __init__(self, val):
        self.val = val
        self.size = self.val.type.template_argument(1)
        self.data = self.val["_data"]

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if self.size:
            return "%s with %d elements" % (typename, self.size)
        return "%s is empty" % typename

    def __iter__(self):
        for i in range(0, self.size):
            yield "%d" % i, self.data[i]

    def children(self):
        return self if self.size > 0 else iter(())

    def display_hint(self):
        return "array"


def _get_base_subobject(child_class_value, index=0):
    """Returns the object's value in the form of the parent class at index.

    This function effectively casts the child_class_value to the base_class's
    type, but the type-to-cast to is stored in the field at index, and once
    we know the field, we can just return the data.

    Args:
      child_class_value: the value to cast
      index: the parent class index

    Raises:
      Exception: field at index was not a base-class field.
    """

    field = child_class_value.type.fields()[index]
    if not field.is_base_class:
        raise Exception("Not a base-class field.")
    return child_class_value[field]


def _value_of_pair_first(value):
    """Convenience for _get_base_subobject, for the common case."""
    return _get_base_subobject(value, 0)["__value_"]


class StdStringPrinter(object):
    """Print a std::string."""

    def _get_short_size(self, short_field, short_size):
        """Short size depends on both endianness and a compile-time define."""

        # If the padding field is present after all this indirection, then string
        # was compiled with _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT defined.
        field = short_field.type.fields()[1].type.fields()[0]
        libcpp_abi_alternate_string_layout = field.name and "__padding" in field.name

        # This logical structure closely follows the original code (which is clearer
        # in C++).  Keep them parallel to make them easier to compare.
        if libcpp_abi_alternate_string_layout:
            if _libcpp_big_endian:
                return short_size >> 1
            else:
                return short_size
        elif _libcpp_big_endian:
            return short_size
        else:
            return short_size >> 1

    def __init__(self, val):
        self.val = val

    def to_string(self):
        """Build a python string from the data whether stored inline or separately."""

        value_field = _value_of_pair_first(self.val["__r_"])
        short_field = value_field["__s"]
        short_size = short_field["__size_"]
        if short_size == 0:
            return ""
        short_mask = self.val["__short_mask"]
        # Counter intuitive to compare the size and short_mask to see if the string
        # is long, but that's the way the implementation does it. Note that
        # __is_long() doesn't use get_short_size in C++.
        is_long = short_size & short_mask
        if is_long:
            long_field = value_field["__l"]
            data = long_field["__data_"]
            size = long_field["__size_"]
        else:
            data = short_field["__data_"]
            size = self._get_short_size(short_field, short_size)
        if hasattr(data, "lazy_string"):
            return data.lazy_string(length=size)
        return data.string(length=size)

    def display_hint(self):
        return "string"


class StdStringViewPrinter(object):
    """Print a std::string_view."""

    def __init__(self, val):
      self.val = val

    def to_string(self):  # pylint: disable=g-bad-name
      """GDB calls this to compute the pretty-printed form."""

      ptr = self.val["__data"]
      length = self.val["__size"]
      print_length = length
      # We print more than just a simple string (i.e. we also print
      # "of length %d").  Thus we can't use the "string" display_hint,
      # and thus we have to handle "print elements" ourselves.
      # For reference sake, gdb ensures limit == None or limit > 0.
      limit = gdb.parameter("print elements")
      if limit is not None:
        print_length = min(print_length, limit)
      # FIXME: Passing ISO-8859-1 here isn't always correct.
      string = ptr.string("ISO-8859-1", "ignore", print_length)
      if length > print_length:
        string += "..."
      return "std::string_view of length %d: \"%s\"" % (length, string)


class StdUniquePtrPrinter(object):
    """Print a std::unique_ptr."""

    def __init__(self, val):
        self.val = val
        self.addr = self.val["_ptr"]
        self.pointee_type = self.val.type.template_argument(0)

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if not self.addr:
            return "%s is nullptr" % typename
        return ("%s<%s> containing" %
                (typename,
                 _remove_generics(_prettify_typename(self.pointee_type))))

    def __iter__(self):
        yield "[pointer]", self.addr.cast(self.pointee_type.pointer())

    def children(self):
        return self


class StdSharedPointerPrinter(object):
    """Print a std::shared_ptr."""

    def __init__(self, val):
        self.val = val
        self.addr = self.val["_ptr"]

    def to_string(self):
        """Returns self as a string."""
        typename = _remove_generics(_prettify_typename(self.val.type))
        pointee_type = _remove_generics(
            _prettify_typename(self.val.type.template_argument(0)))
        if not self.addr:
            return "%s is nullptr" % typename
        return "%s<%s> containing" % (typename, pointee_type)

    def __iter__(self):
        yield "[pointer]", self.addr
        refcount = self.val["_cb"]
        if refcount != 0:
            usecount = refcount["_shared"]
            weakcount = refcount["_weak"]
            yield "[shared]", usecount
            yield "[weak]", weakcount

    def children(self):
        return self


class StdVectorPrinter(object):
    """Print a std::vector."""

    class _VectorBoolIterator(object):
        """Class to iterate over the bool vector's children."""

        def __init__(self, begin, size, bits_per_word):
            self.item = begin
            self.size = size
            self.bits_per_word = bits_per_word
            self.count = 0
            self.offset = 0

        def __iter__(self):
            return self

        def __next__(self):
            """Retrieve the next element."""

            self.count += 1
            if self.count > self.size:
                raise StopIteration
            entry = self.item.dereference()
            if entry & (1 << self.offset):
                outbit = 1
            else:
                outbit = 0
            self.offset += 1
            if self.offset >= self.bits_per_word:
                self.item += 1
                self.offset = 0
            return ("[%d]" % self.count, outbit)

        # TODO Delete when we drop Python 2.
        def next(self):
            return self.__next__()

    class _VectorIterator(object):
        """Class to iterate over the non-bool vector's children."""

        def __init__(self, begin, end):
            self.item = begin
            self.end = end
            self.count = 0

        def __iter__(self):
            return self

        def __next__(self):
            self.count += 1
            if self.item == self.end:
                raise StopIteration
            entry = self.item.dereference()
            self.item += 1
            return ("[%d]" % self.count, entry)

        # TODO Delete when we drop Python 2.
        def next(self):
            return self.__next__()

    def __init__(self, val):
        """Set val, length, capacity, and iterator for bool and normal vectors."""
        self.val = val
        self.typename = _remove_generics(_prettify_typename(val.type))
        begin = self.val["_data"]
        if False:
        # if self.val.type.template_argument(0).code == gdb.TYPE_CODE_BOOL:
            self.typename += "<bool>"
            self.length = self.val["__size_"]
            bits_per_word = self.val["__bits_per_word"]
            self.capacity = _value_of_pair_first(
                self.val["__cap_alloc_"]) * bits_per_word
            self.iterator = self._VectorBoolIterator(
                begin, self.length, bits_per_word)
        else:
            self.length = self.val["_size"]
            end = begin + self.length
            self.capacity = self.val["_capacity"]
            self.iterator = self._VectorIterator(begin, end)

    def to_string(self):
        return ("%s of length %d, capacity %d" %
                (self.typename, self.length, self.capacity))

    def children(self):
        return self.iterator

    def display_hint(self):
        return "array"


class StdRingPrinter(object):
    """Print a ala::ring."""

    def __init__(self, val):
        """Set val, length, capacity, and iterator for bool and normal vectors."""
        self.val = val
        self.typename = _remove_generics(_prettify_typename(val.type))
        self.data = self.val["_data"]
        self.circ = self.val["_circ"]
        self.head = self.val["_head"]
        self.tail = self.val["_tail"]

    def _size(self):
        return (self.tail + self.circ - self.head) % self.circ;

    def to_string(self):
        return ("%s of length %d, capacity %d" %
                (self.typename, self._size(), self.circ - 1))

    def __iter__(self):
        for i in range(self._size()):
            yield "%d" % i, (self.data + (self.head + i) % self.circ).dereference()

    def children(self):
        return self

    def display_hint(self):
        return "array"


class RingIteratorPrinter(object):
    """Abstract super class for std::list iterator."""

    def __init__(self, val):
        self.val = val
        self.addr = val["_ptr"]
        self.ref = val["_ref"]

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if not self.addr:
            return "%s is nullptr" % typename
        elif self.ref["_data"] + self.ref["_tail"] == self.addr:
            return "%s = end()" % typename
        return "%s " % typename

    def __iter__(self):
        addr_str = "[%s]" % str(self.addr)
        yield addr_str, self.addr.dereference()

    def children(self):
        return self if self.addr else iter(())


class StdBitsetPrinter(object):
    """Print a std::bitset."""

    def __init__(self, val):
        self.val = val
        self.n_words = int(self.val["__n_words"])
        self.bits_per_word = int(self.val["__bits_per_word"])
        self.bit_count = self.val.type.template_argument(0)
        if self.n_words == 1:
            self.values = [int(self.val["__first_"])]
        else:
            self.values = [int(self.val["__first_"][index])
                           for index in range(self.n_words)]

    def to_string(self):
        typename = _prettify_typename(self.val.type)
        return "%s" % typename

    def _list_it(self):
        for bit in range(self.bit_count):
            word = bit // self.bits_per_word
            word_bit = bit % self.bits_per_word
            if self.values[word] & (1 << word_bit):
                yield ("[%d]" % bit, 1)

    def __iter__(self):
        return self._list_it()

    def children(self):
        return self


class StdListPrinter(object):
    """Print a std::list."""

    def __init__(self, val):
        self.val = val
        self.size = self.val["_size"]
        dummy_node = self.val["_guard"][0]
        self.cast_name = "ala::l_vnode<%s>" % self.val.type.template_argument(0)
        self.cast_type = gdb.lookup_type(self.cast_name).pointer()
        self.first_node = dummy_node["_suc"]

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if self.size:
            return "%s with %d elements" % (typename, self.size)
        return "%s is empty" % typename

    def _list_iter(self):
        node = self.first_node
        for i in range(self.size):
            yield "%d" % i, node.cast(self.cast_type).dereference()["_data"]
            node = node.dereference()["_suc"]

    def __iter__(self):
        return self._list_iter()

    def children(self):
        return self if self.cast_type and self.size > 0 else iter(())

    def display_hint(self):
        return "array"


class StdQueueOrStackPrinter(object):
    """Print a std::queue or std::stack."""

    def __init__(self, val):
        self.val = val
        self.underlying = val["c"]

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        return "%s wrapping" % typename

    def children(self):
        return iter([("", self.underlying)])

    def display_hint(self):
        return "array"


class StdPriorityQueuePrinter(object):
    """Print a std::priority_queue."""

    def __init__(self, val):
        self.val = val
        self.underlying = val["c"]

    def to_string(self):
        # TODO(tamur): It would be nice to print the top element. The technical
        # difficulty is that, the implementation refers to the underlying
        # container, which is a generic class. libstdcxx pretty printers do not
        # print the top element.
        typename = _remove_generics(_prettify_typename(self.val.type))
        return "%s wrapping" % typename

    def children(self):
        return iter([("", self.underlying)])

    def display_hint(self):
        return "array"


class AbstractRBTreePrinter(object):
    """Abstract super class for std::(multi)map, and std::(multi)set."""

    def __init__(self, val):
        self.val = val["tree"]
        self.size = int(self.val["_size"])
        self.root = self.val["_root"]
        self.cast_name = "ala::rb_vnode<%s>" % self.val.type.template_argument(0)
        self.cast_type = gdb.lookup_type(self.cast_name).pointer()

    def is_nil(self, node):
        return node == 0 or node.dereference()["_is_nil"]

    def _traverse(self, node):
        """Traverses the binary search tree in order."""
        if not self.is_nil(node.dereference()["_left"]):
            for i in self._traverse(node.dereference()["_left"]):
                yield i
        for kv in self._get_key_value(node):
            yield "", kv
        if not self.is_nil(node.dereference()["_left"]):
            for i in self._traverse(node.dereference()["_rght"]):
                yield i

    def __iter__(self):
        return self._traverse(self.root)

    def children(self):
        return self if self.cast_type and self.size > 0 else iter(())

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if self.size:
            return "%s with %d elements" % (typename, self.size)
        return "%s is empty" % typename

    def _get_key_value(self, node):
        if self._is_map():
            return [node.cast(self.cast_type).dereference()["_data"]["first"],
                    node.cast(self.cast_type).dereference()["_data"]["second"]]
        return [node.cast(self.cast_type).dereference()["_data"]];

    def _is_map(self):
        return self.val.type.template_argument(3) == True

    def display_hint(self):
        return "map" if self._is_map() else "array" 


class AbstractRBTreeIteratorPrinter(object):
    """Abstract super class for std::(multi)map, and std::(multi)set iterator."""

    def __init__(self, val):
        self.val = val
        self.addr = val["_ptr"]
        self.cast_name = "ala::rb_vnode<%s>" % self.val.type.template_argument(0)
        self.cast_type = gdb.lookup_type(self.cast_name).pointer()
        if self.addr:
            self.node = self.addr.cast(self.cast_type).dereference()

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if not self.addr:
            return "%s is nullptr" % typename
        elif self.node["_is_nil"]:
            return "%s = end()" % typename
        return "%s " % typename

    def __iter__(self):
        addr_str = "[%s]" % str(self.addr)
        yield addr_str, self.node["_data"]

    def children(self):
        return self if self.addr else iter(())


class ListIteratorPrinter(object):
    """Abstract super class for std::list iterator."""

    def __init__(self, val):
        self.val = val
        self.addr = val["_ptr"]
        self.cast_name = "ala::l_vnode<%s>" % self.val.type.template_argument(0)
        self.cast_type = gdb.lookup_type(self.cast_name).pointer()
        if self.addr:
            self.node = self.addr.cast(self.cast_type).dereference()

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if not self.addr:
            return "%s is nullptr" % typename
        elif not self.node["_suc"]:
            return "%s = end()" % typename
        return "%s " % typename

    def __iter__(self):
        addr_str = "[%s]" % str(self.addr)
        yield addr_str, self.node["_data"]

    def children(self):
        return self if self.addr else iter(())


class AbstractUnorderedCollectionPrinter(object):
    """Abstract super class for std::unordered_(multi)[set|map]."""

    def __init__(self, val):
        self.val = val
        self.table = val["__table_"]
        self.sentinel = self.table["__p1_"]
        self.size = int(_value_of_pair_first(self.table["__p2_"]))
        node_base_type = self.sentinel.type.template_argument(0)
        self.cast_type = node_base_type.template_argument(0)

    def _list_it(self, sentinel_ptr):
        next_ptr = _value_of_pair_first(sentinel_ptr)["__next_"]
        while str(next_ptr.cast(_void_pointer_type)) != "0x0":
            next_val = next_ptr.cast(self.cast_type).dereference()
            for key_value in self._get_key_value(next_val):
                yield "", key_value
            next_ptr = next_val["__next_"]

    def to_string(self):
        typename = _remove_generics(_prettify_typename(self.val.type))
        if self.size:
            return "%s with %d elements" % (typename, self.size)
        return "%s is empty" % typename

    def _get_key_value(self, node):
        """Subclasses should override to return a list of values to yield."""
        raise NotImplementedError

    def children(self):
        return self if self.cast_type and self.size > 0 else iter(())

    def __iter__(self):
        return self._list_it(self.sentinel)


class StdUnorderedSetPrinter(AbstractUnorderedCollectionPrinter):
    """Print a std::unordered_(multi)set."""

    def _get_key_value(self, node):
        return [node["__value_"]]

    def display_hint(self):
        return "array"


class StdUnorderedMapPrinter(AbstractUnorderedCollectionPrinter):
    """Print a std::unordered_(multi)map."""

    def _get_key_value(self, node):
        key_value = node["__value_"]["__cc"]
        return [key_value["first"], key_value["second"]]

    def display_hint(self):
        return "map"


class AbstractHashMapIteratorPrinter(object):
    """Abstract class for unordered collection iterators."""

    def _initialize(self, val, addr):
        self.val = val
        self.typename = _remove_generics(_prettify_typename(self.val.type))
        self.addr = addr
        if self.addr:
            self.node = self.addr.cast(self.cast_type).dereference()

    def _get_key_value(self):
        """Subclasses should override to return a list of values to yield."""
        raise NotImplementedError

    def to_string(self):
        if not self.addr:
            return "%s = end()" % self.typename
        return "%s " % self.typename

    def children(self):
        return self if self.addr else iter(())

    def __iter__(self):
        for key_value in self._get_key_value():
            yield "", key_value


class StdUnorderedSetIteratorPrinter(AbstractHashMapIteratorPrinter):
    """Print a std::(multi)set iterator."""

    def __init__(self, val):
        self.cast_type = val.type.template_argument(0)
        self._initialize(val, val["__node_"])

    def _get_key_value(self):
        return [self.node["__value_"]]

    def display_hint(self):
        return "array"


class StdUnorderedMapIteratorPrinter(AbstractHashMapIteratorPrinter):
    """Print a std::(multi)map iterator."""

    def __init__(self, val):
        self.cast_type = val.type.template_argument(0).template_argument(0)
        self._initialize(val, val["__i_"]["__node_"])

    def _get_key_value(self):
        key_value = self.node["__value_"]["__cc"]
        return [key_value["first"], key_value["second"]]

    def display_hint(self):
        return "map"


def _remove_std_prefix(typename):
    match = re.match("^ala::(.+)", typename)
    return match.group(1) if match is not None else ""


class LibcxxPrettyPrinter(object):
    """PrettyPrinter object so gdb-commands like 'info pretty-printers' work."""

    def __init__(self, name):
        super(LibcxxPrettyPrinter, self).__init__()
        self.name = name
        self.enabled = True

        self.lookup = {
            "basic_string": StdStringPrinter,
            "string": StdStringPrinter,
            "string_view": StdStringViewPrinter,
            "tuple": StdTuplePrinter,
            "variant": StdVariantPrinter,
            "optional": StdOptionalPrinter,
            "any": StdAnyPrinter,
            "array": StdArrayPrinter,
            "unique_ptr": StdUniquePtrPrinter,
            "shared_ptr": StdSharedPointerPrinter,
            "weak_ptr": StdSharedPointerPrinter,
            "bitset": StdBitsetPrinter,
            "ring": StdRingPrinter,
            "list": StdListPrinter,
            "forward_list": StdListPrinter,
            "queue": StdQueueOrStackPrinter,
            "stack": StdQueueOrStackPrinter,
            "priority_queue": StdPriorityQueuePrinter,
            "map": AbstractRBTreePrinter,
            "multimap": AbstractRBTreePrinter,
            "set": AbstractRBTreePrinter,
            "multiset": AbstractRBTreePrinter,
            "vector": StdVectorPrinter,
            "rb_iterator": AbstractRBTreeIteratorPrinter,
            "l_iterator": ListIteratorPrinter,
            "ring_iterator": RingIteratorPrinter,
            "unordered_set": StdUnorderedSetPrinter,
            "unordered_multiset": StdUnorderedSetPrinter,
            "unordered_map": StdUnorderedMapPrinter,
            "unordered_multimap": StdUnorderedMapPrinter,
            "__hash_map_iterator": StdUnorderedMapIteratorPrinter,
            "__hash_map_const_iterator": StdUnorderedMapIteratorPrinter,
            "__hash_iterator": StdUnorderedSetIteratorPrinter,
            "__hash_const_iterator": StdUnorderedSetIteratorPrinter,
        }

        self.subprinters = []
        for name, subprinter in self.lookup.items():
            # Subprinters and names are used only for the rarely used command "info
            # pretty" (and related), so the name of the first data structure it prints
            # is a reasonable choice.
            if subprinter not in self.subprinters:
                subprinter.name = name
                self.subprinters.append(subprinter)

    def __call__(self, val):
        """Return the pretty printer for a val, if the type is supported."""

        # Do not handle any type that is not a struct/class.
        if val.type.strip_typedefs().code != gdb.TYPE_CODE_STRUCT:
            return None

        # Don't attempt types known to be inside libstdcxx.
        typename = val.type.name or val.type.tag or str(val.type)
        match = re.match("^ala::(__.*?)::", typename)
        if match is not None and match.group(1) in ["__cxx1998",
                                                    "__debug",
                                                    "__7",
                                                    "__g"]:
            return None

        # Handle any using declarations or other typedefs.
        typename = _prettify_typename(val.type)
        if not typename:
            return None
        without_generics = _remove_generics(typename)
        lookup_name = _remove_std_prefix(without_generics)
        if lookup_name in self.lookup:
            return self.lookup[lookup_name](val)
        return None


_libcxx_printer_name = "libcxx_pretty_printer"


# These are called for every binary object file, which could be thousands in
# certain pathological cases. Limit our pretty printers to the progspace.
def _register_libcxx_printers(event):
    progspace = event.new_objfile.progspace
    # It would be ideal to get the endianness at print time, but
    # gdb.execute clears gdb's internal wrap buffer, removing any values
    # already generated as part of a larger data structure, and there is
    # no python api to get the endianness. Mixed-endianness debugging
    # rare enough that this workaround should be adequate.
    _libcpp_big_endian = "big endian" in gdb.execute("show endian",
                                                     to_string=True)

    if not getattr(progspace, _libcxx_printer_name, False):
        print("Loading libc++ pretty-printers.")
        gdb.printing.register_pretty_printer(
            progspace, LibcxxPrettyPrinter(_libcxx_printer_name))
        setattr(progspace, _libcxx_printer_name, True)


def _unregister_libcxx_printers(event):
    progspace = event.progspace
    if getattr(progspace, _libcxx_printer_name, False):
        for printer in progspace.pretty_printers:
            if getattr(printer, "name", "none") == _libcxx_printer_name:
                progspace.pretty_printers.remove(printer)
                setattr(progspace, _libcxx_printer_name, False)
                break


def register_libcxx_printer_loader():
    """Register event handlers to load libc++ pretty-printers."""
    gdb.events.new_objfile.connect(_register_libcxx_printers)
    gdb.events.clear_objfiles.connect(_unregister_libcxx_printers)
