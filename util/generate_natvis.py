def tuple(n):
    for i in range(n + 1):
        s = ''
        s += '<Type Name="ala::tuple&lt;{}&gt;">\n'.format(
            ','.join((['*'] * i)))
        s += '  <DisplayString Optional="true">'
        s += '({})'.format(', '.join(
            ['{{((ala::_tuple_base&lt;{},$T{},0&gt;*) &amp;_impl)->value}}'.format(j, j + 1) for j in range(i)]))
        s += '</DisplayString>\n'
        s += '  <Expand>\n'
        for j in range(i):
            s += '    <Item Name="[{}]" Optional="true">'.format(j)
            s += '((ala::_tuple_base&lt;{},$T{},0&gt;*) &amp;_impl)->value</Item>\n'.format(j, j + 1)
            s += '    <Item Name="[{}]" Optional="true">'.format(j)
            s += '*(($T{}*) (ala::_tuple_base&lt;{},$T{},1&gt;*) &amp;_impl)</Item>\n'.format(j + 1, j, j + 1)
        s += '  </Expand>\n'
        s += '</Type>\n'
        print(s)


def variant_old(n):
    for i in range(n):
        s = ''
        s += '<Type Name="ala::_variant_union&lt;*,*,*&gt;" IncludeView="VariantHelper{}" Priority="MediumHigh">'.format(
            i)
        s += '<Expand><Item Name="[value]" Condition="$T2 == {}">_value</Item></Expand></Type>'.format(
            i)
        print(s)

    for i in range(n):
        s = ''
        s += '<Type Name="ala::_variant_union&lt;*&gt;" IncludeView="VariantHelper{}" Priority="Medium">'.format(
            i)
        s += '<Expand><ExpandedItem>_left,view(VariantHelper{})</ExpandedItem><ExpandedItem>_rght,view(VariantHelper{})</ExpandedItem></Expand></Type>'.format(i, i)
        print(s)

    s = '\n'
    s += '<Type Name="ala::variant&lt;*&gt;">\n'
    s += '  <DisplayString Condition="_index &lt; 0">[valueless_by_exception]</DisplayString>\n'
    s += '  <Expand>\n'
    s += '    <Item Name="[index]">_index</Item>\n'
    for i in range(n):
        s += '    <ExpandedItem Condition="_index == {}">_union,view(VariantHelper{})</ExpandedItem>\n'.format(
            i, i)
    s += '  </Expand>\n'
    s += '</Type>\n'
    print(s)


def variant(n):
    s = '\n'
    s += '<Type Name="ala::variant&lt;*&gt;">\n'
    s += '  <DisplayString Condition="_index &lt; 0">[valueless_by_exception]</DisplayString>\n'
    s += '  <Expand>\n'
    s += '    <Item Name="[index]">_index</Item>\n'
    for i in range(n):
        s += '    <Item Name="[value]" Condition="_index == {}" Optional="true">((ala::_variant_union&lt;0,$T{}&gt;*) &amp;_union)->_value</Item>\n'.format(
            i, i + 1)
    for i in range(n):
        s += '    <Item Name="[value]" Condition="_index == {}" Optional="true">((ala::_variant_union&lt;1,$T{}&gt;*) &amp;_union)->_value</Item>\n'.format(
            i, i + 1)
    s += '  </Expand>\n'
    s += '</Type>\n'
    print(s)


tuple(10)
variant(10)
