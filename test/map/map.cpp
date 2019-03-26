#include <iostream>
#include <string>
#include <iomanip>
#include <ala/map.h>

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

int main() {
    // (1) 默认构造函数
    ala::map<std::string, int> map1;
    ala::map<std::string, int>::iterator it = map1.cbegin();
    map1["something"] = 69;
    map1["anything"] = 199;
    map1["that thing"] = 50;
    std::cout << "map1 = ";
    print_map(map1);

    // (2) 范围构造函数
    ala::map<std::string, int> iter(map1.find("anything"), map1.end());
    std::cout << "\niter = ";
    print_map(iter);
    std::cout << "map1 = ";
    print_map(map1);

    // (3) 复制构造函数
    ala::map<std::string, int> copied(map1);
    std::cout << "\ncopied = ";
    print_map(copied);
    std::cout << "map1 = ";
    print_map(map1);

    // (4) 移动构造函数
    ala::map<std::string, int> moved(std::move(map1));
    std::cout << "\nmoved = ";
    print_map(moved);
    std::cout << "map1 = ";
    print_map(map1);

    // (5) initializer_list 构造函数
    const ala::map<std::string, int> init{
        {"this", 100},
        {"can", 100},
        {"be", 100},
        {"const", 100},
    };
    std::cout << "\ninit = ";
    print_map(init);

    // 定制关键类选项 1 ：
    // 使用比较 struct
    ala::map<Point, double, PointCmp> mag = {
        {{5, -12}, 13}, {{3, 4}, 5}, {{-8, -15}, 17}};

    for (auto p : mag)
        std::cout << "The magnitude of (" << p.first.x << ", " << p.first.y
                  << ") is " << p.second << '\n';

    // 定制关键类选项 2 ：
    // 使用比较 lambda
    // 此 lambda 按照其模比较点，注意其中模取自局部变量 mag
    auto cmpLambda = [&mag](const Point &lhs, const Point &rhs) {
        return mag[lhs] < mag[rhs];
    };
    // 你亦可使用不依赖局部变量的 lambda ，像这样：
    // auto cmpLambda = [](const Point &lhs, const Point &rhs) { return lhs.y < rhs.y; };
    ala::map<Point, double, decltype(cmpLambda)> magy(cmpLambda);

    // 各种插入元素的方式：
    magy.insert(ala::pair<Point, double>({5, -12}, 13));
    magy.insert({{3, 4}, 5});
    magy.insert({Point{-8.0, -15.0}, 17});

    std::cout << '\n';
    for (auto p : magy)
        std::cout << "The magnitude of (" << p.first.x << ", " << p.first.y
                  << ") is " << p.second << '\n';
}