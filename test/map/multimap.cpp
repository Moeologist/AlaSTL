#include <iostream>
#include <string>
#include <iomanip>
#include <ala/map.h>

#include <cmath>
#include <iostream>

template<typename Map>
void print_map(Map &m) {
    std::cout << '{';
    for (auto &p : m)
        std::cout << p.first << ':' << p.second << ' ';
    std::cout << "}\n";
}

struct Point {
    double x, y;
};

struct PointCmp {
    bool operator()(const Point &lhs, const Point &rhs) const {
        return lhs.x < rhs.x; // NB 。有意忽略 y
    }
};

typedef Point *PointPtr;
// 比较二个 Point 指针的 x 坐标和 y 坐标
struct PointPtrCmp {
    bool operator()(const PointPtr &lhs, const PointPtr &rhs) const {
        return lhs->x < rhs->x;
    }
};

void test1() {
    // (1) 默认构造函数
    ala:: multimap<std::string, int> map1;
    ala:: multimap<std::string, int>::iterator it = map1.cbegin();
    std::cout << "map1 = ";
    print_map(map1);

    // (2) 范围构造函数
    ala:: multimap<std::string, int> iter(map1.find("anything"), map1.end());
    std::cout << "\niter = ";
    print_map(iter);
    std::cout << "map1 = ";
    print_map(map1);

    // (3) 复制构造函数
    ala:: multimap<std::string, int> copied(map1);
    std::cout << "\ncopied = ";
    print_map(copied);
    std::cout << "map1 = ";
    print_map(map1);

    // (4) 移动构造函数
    ala:: multimap<std::string, int> moved(std::move(map1));
    std::cout << "\nmoved = ";
    print_map(moved);
    std::cout << "map1 = ";
    print_map(map1);

    // (5) initializer_list 构造函数
    const ala:: multimap<std::string, int> init{
        {"this", 100},
        {"can", 100},
        {"be", 100},
        {"const", 100},
    };
    std::cout << "\ninit = ";
    print_map(init);

    // 定制关键类选项 1 ：
    // 使用比较 struct
    ala:: multimap<Point, double, PointCmp> mag = {{{5, -12}, 13},
                                             {{3, 4}, 5},
                                             {{-8, -15}, 17}};

    for (auto p : mag)
        std::cout << "The magnitude of (" << p.first.x << ", " << p.first.y
                  << ") is " << p.second << '\n';

    // 定制关键类选项 2 ：
    // 使用比较 lambda
    // 此 lambda 按照其模比较点，注意其中模取自局部变量 mag
    // auto cmpLambda = [&mag](const Point &lhs, const Point &rhs) {
    //     return mag[lhs] < mag[rhs];
    // };
    // 你亦可使用不依赖局部变量的 lambda ，像这样：
    // auto cmpLambda = [](const Point &lhs, const Point &rhs) { return lhs.y < rhs.y; };
    ala:: multimap<Point, double, decltype(cmpLambda)> magy(cmpLambda);

    // 各种插入元素的方式：
    magy.insert(ala::pair<Point, double>({5, -12}, 13));
    magy.insert({{3, 4}, 5});
    magy.insert({Point{-8.0, -15.0}, 17});

    std::cout << '\n';
    for (auto p : magy)
        std::cout << "The magnitude of (" << p.first.x << ", " << p.first.y
                  << ") is " << p.second << '\n';
}

void display_sizes(const ala:: multimap<int, int> &nums1,
                   const ala:: multimap<int, int> &nums2,
                   const ala:: multimap<int, int> &nums3) {
    std::cout << "nums1: " << nums1.size() << " nums2: " << nums2.size()
              << " nums3: " << nums3.size() << '\n';
}

void test2() {
    ala:: multimap<int, int> nums1{{3, 1}, {4, 1}, {5, 9}, {6, 1}, {7, 1}, {8, 9}};
    ala:: multimap<int, int> nums2;
    ala:: multimap<int, int> nums3;

    std::cout << "Initially:\n";
    display_sizes(nums1, nums2, nums3);

    // 复制赋值从 nums1 复制数据到 nums2
    nums2 = nums1;

    std::cout << "After assigment:\n";
    display_sizes(nums1, nums2, nums3);

    // 移动赋值从 nums1 移动数据到 nums3,
    // 一同修改 nums1 和 nums3
    nums3 = std::move(nums1);

    std::cout << "After move assigment:\n";
    display_sizes(nums1, nums2, nums3);
}

void test3() {
    ala:: multimap<char, int> letter_counts{{'a', 27}, {'b', 3}, {'c', 1}};

    std::cout << "initially:\n";
    for (const auto &pair : letter_counts) {
        std::cout << pair.first << ": " << pair.second << '\n';
    }

    letter_counts['b'] = 42; // 更新既存值
    letter_counts['x'] = 9;  // 插入新值

    std::cout << "after modifications:\n";
    for (const auto &pair : letter_counts) {
        std::cout << pair.first << ": " << pair.second << '\n';
    }

    // 统计每个词的出现数
    // （首次调用 operator[] 以零初始化计数器）
    ala:: multimap<std::string, size_t> word_map;
    for (const auto &w : {"this", "sentence", "is", "not", "a", "sentence",
                          "this", "sentence", "is", "a", "hoax"}) {
        ++word_map[w];
    }

    for (const auto &pair : word_map) {
        std::cout << pair.second << " occurrences of word '" << pair.first
                  << "'\n";
    }
}

void test4() {
    ala:: multimap<int, float> num_map;
    num_map[4] = 4.13;
    num_map[9] = 9.24;
    num_map[1] = 1.09;
    // 调用 a_map.begin() 及 a_map.end()
    for (auto it = num_map.begin(); it != num_map.end(); ++it) {
        std::cout << it->first << ", " << it->second << '\n';
    }
}

template<class...>
class FK;

void test5() {
    // 注意即使 x 坐标不按顺序，  multimap 仍将按 x 坐标升序迭代
    Point points[3] = {{2, 0}, {1, 0}, {3, 0}};

    // mag 是将结点地址发送到其在 x-y 平面上的模的映射
    // 尽管关键是指针到 Point ，我们希望按 x 坐标顺序而非按 Point 地址的顺序。
    // 这通过使用 PointPtrCmp 类的比较方法进行。
    ala:: multimap<Point *, double, PointPtrCmp> mag(
        {{points, 2}, {points + 1, 1}, {points + 2, 3}});

    // 从 0 更改每个 y 坐标为模
    for (auto iter = mag.begin(); iter != mag.end(); ++iter) {
        auto cur = iter->first; // 指向 Node 的指针
        cur->y = mag[cur];      // 亦可使用 cur->y = iter->second;
    }

    // 更新并打印每个结点的模
    for (auto iter = mag.begin(); iter != mag.end(); ++iter) {
        auto cur = iter->first;
        mag[cur] = std::hypot(cur->x, cur->y);
        std::cout << "The magnitude of (" << cur->x << ", " << cur->y << ") is ";
        std::cout << iter->second << '\n';
    }

    // 以基于范围的 for 循环重复以上内容
    for (auto i : mag) {
        auto cur = i.first;
        cur->y = i.second;
        mag[cur] = std::hypot(cur->x, cur->y);
        std::cout << "The magnitude of (" << cur->x << ", " << cur->y << ") is ";
        std::cout << mag[cur] << '\n';
        // 注意与 std::cout << iter->second << '\n'; 相反，上述的
        // std::cout << i.second << '\n'; 不会打印更新的模
    }

    // mag.cbegin()->second = 10;
    // (*mag.cbegin()).second = 10;
    // FK<decltype(mag.cbegin().operator ->())> fk;
    // FK<decltype(mag.cbegin().operator *())> fk1;
}

void test6() {
    ala:: multimap<int, int> numbers;
    std::cout << "Initially, numbers.empty(): " << numbers.empty() << '\n';
    std::cout << "`ially" << (numbers.begin() == numbers.end()) << '\n';

    numbers.emplace(42, 13);
    numbers.insert(ala::make_pair(13317, 123));
    std::cout << "After adding elements, numbers.empty(): " << numbers.empty()
              << '\n';
    std::cout << "After adding elements" << (numbers.begin() == numbers.end())
              << '\n';
}

void test7() {
    ala:: multimap<int, double> mag = {{1, 13}, {2, 5}, {3, 17}};
    for (auto i = mag.rbegin(); i != mag.rend(); ++i)
        std::cout << i->first << "," << i->second << "\n";
    for (auto i = mag.begin(); i != mag.end(); ++i)
        std::cout << i->first << "," << i->second << "\n";
    ala:: multimap<char, char> s;
    std::cout << "Maximum size of a ' multimap' is " << s.max_size() << "\n";
}

void test8() {
    ala:: multimap<char, int> mymap;

    // first insert function version (single parameter):
    mymap.insert(ala::pair<char, int>('a', 100));
    mymap.insert(ala::pair<char, int>('z', 200));

    ala::pair<ala:: multimap<char, int>::iterator, bool> ret;
    ret = mymap.insert(ala::pair<char, int>('z', 500));
    if (ret.second == false) {
        std::cout << "element 'z' already existed";
        std::cout << " with a value of " << ret.first->second << '\n';
    }

    // second insert function version (with hint position):
    ala:: multimap<char, int>::iterator it = mymap.begin();
    mymap.insert(it, ala::pair<char, int>('b', 300)); // max efficiency inserting
    mymap.insert(it,
                 ala::pair<char, int>('c', 400)); // no max efficiency inserting

    // third insert function version (range insertion):
    ala:: multimap<char, int> anothermap;
    anothermap.insert(mymap.begin(), mymap.find('c'));

    // showing contents:
    std::cout << "mymap contains:\n";
    for (it = mymap.begin(); it != mymap.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';

    std::cout << "anothermap contains:\n";
    for (it = anothermap.begin(); it != anothermap.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
}

void test9() {
    ala:: multimap<std::string, std::string> myMap;
    myMap.insert_or_assign("a", "apple");
    myMap.insert_or_assign("b", "bannana");
    myMap.insert_or_assign("c", "cherry");
    myMap.insert_or_assign("c", "clementine");

    for (const auto &pair : myMap) {
        std::cout << pair.first << " : " << pair.second << '\n';
    }
}

void test10() {
    ala:: multimap<std::string, std::string> m;

    // 使用 pair 的移动构造函数
    m.emplace(ala::make_pair(std::string("a"), std::string("a")));

    // 使用 pair 的转换移动构造函数
    m.emplace(ala::make_pair("b", "abcd"));

    // 使用 pair 的模板构造函数
    m.emplace("d", "ddd");

    // 使用 pair 的逐片构造函数
    m.emplace(ala::piecewise_construct, ala::forward_as_tuple("c"),
              ala::forward_as_tuple(10, 'c'));
    // C++17 起，能使用 m.try_emplace("c", 10, 'c');

    for (const auto &p : m) {
        std::cout << p.first << " => " << p.second << '\n';
    }
}

void test12() {
    using namespace std::literals;
    ala:: multimap<std::string, std::string> m;

    m.try_emplace("a", "a"s);
    m.try_emplace("b", "abcd");
    m.try_emplace("c", 10, 'c');
    m.try_emplace("c", "Won't be inserted");

    for (const auto &p : m) {
        std::cout << p.first << " => " << p.second << '\n';
    }
}

void test13() {
    ala:: multimap<int, std::string> c = {{1, "one"},  {2, "two"},  {3, "three"},
                                    {4, "four"}, {5, "five"}, {6, "six"}};
    // 从 c 擦除所有奇数
    for (auto it = c.begin(); it != c.end();)
        if (it->first % 2 == 1)
            it = c.erase(it);
        else
            ++it;
    for (auto &p : c)
        std::cout << p.second << ' ';
}

void test14() {
    ala:: multimap<int, std::string> m{{1, "mango"}, {2, "papaya"}, {3, "guava"}};
    auto nh = m.extract(2);
    nh.key() = 4;
    m.insert(ala::move(nh));
    // m == {{1, "mango"}, {3, "guava"}, {4, "papaya"}}
}

struct FatKey {
    int x;
    int data[1000];
};
struct LightKey {
    int x;
};
bool operator<(const FatKey &fk, const LightKey &lk) {
    return fk.x < lk.x;
}
bool operator<(const LightKey &lk, const FatKey &fk) {
    return lk.x < fk.x;
}
bool operator<(const FatKey &fk1, const FatKey &fk2) {
    return fk1.x < fk2.x;
}
void test15() {
    // 简单比较演示
    ala:: multimap<int, char> example = {{1, 'a'}, {2, 'b'}};

    auto search = example.find(2);
    if (search != example.end()) {
        std::cout << "Found " << search->first << " " << search->second << '\n';
    } else {
        std::cout << "Not found\n";
    }

    // 通透比较演示
    ala:: multimap<FatKey, char, std::less<>> example2 = {{{1, {}}, 'a'},
                                                    {{2, {}}, 'b'}};

    LightKey lk = {2};
    auto search2 = example2.find(lk);
    if (search2 != example2.end()) {
        std::cout << "Found " << search2->first.x << " " << search2->second
                  << '\n';
    } else {
        std::cout << "Not found\n";
    }
}

void test16() {
    ala:: multimap<int, char> example = {{1, 'a'}, {2, 'b'}};

    if (example.contains(2)) {
        std::cout << "Found\n";
    } else {
        std::cout << "Not found\n";
    }
}

void test17() {
    const ala:: multimap<int, const char *> m{
        {0, "zero"},
        {1, "one"},
        {2, "two"},
    };

    {
        auto p = m.equal_range(1);
        for (auto &q = p.first; q != p.second; ++q) {
            std::cout << "m[" << q->first << "] = " << q->second << '\n';
        }

        if (p.second == m.find(2)) {
            std::cout << "end of equal_range (p.second) is one-past p.first\n";
        } else {
            std::cout
                << "unexpected; p.second expected to be one-past p.first\n";
        }
    }

    {
        auto pp = m.equal_range(-1);
        if (pp.first == m.begin()) {
            std::cout << "pp.first is iterator to first not-less than -1\n";
        } else {
            std::cout << "unexpected pp.first\n";
        }

        if (pp.second == m.begin()) {
            std::cout
                << "pp.second is iterator to first element greater-than -1\n";
        } else {
            std::cout << "unexpected pp.second\n";
        }
    }

    {
        auto ppp = m.equal_range(3);
        if (ppp.first == m.end()) {
            std::cout << "ppp.first is iterator to first not-less than 3\n";
        } else {
            std::cout << "unexpected ppp.first\n";
        }

        if (ppp.second == m.end()) {
            std::cout
                << "ppp.second is iterator to first element greater-than 3\n";
        } else {
            std::cout << "unexpected ppp.second\n";
        }
    }
}

int main() {
    
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    // test11();
    test12();
    test13();
    test14();
    test15();
    test16();
    test17();

    return 0;
}