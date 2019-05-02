#include "VectorPack.h"

void fff(ala::La::Vector8d &v) {
    v.sqrt();
}

// int main(int argc, char **argv) {
//     // auto v = ala::La::Vector4d{(double)argc, (double)argc + 1, (double)argc + 2,
//     //                            (double)argc + 3}
//     //              .sqrt();
//     auto v = ala::La::Vector4d{(double)argc, (double)argc + 1, (double)argc + 2,
//                                (double)argc + 3} *
//              1.5;
//     return v[0] + v[1] + v[2] + v[3];
// }