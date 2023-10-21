#pragma once

#include <array>
#include <vector>
#include <string>
#include <cinttypes>

namespace io {

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//            node numbering conventions consistent with gmsh                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Line2:                 Line3:                                             //
//                                                                           //
//       v                                                                   //
//       ^                                                                   //
//       |                                                                   //
//       |                                                                   //
// 0-----+-----1 --> u    0----2----1                                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Tri3:                   Tri6:                                             //
//                                                                           //
// v                                                                         //
// ^                                                                         //
// |                                                                         //
// 2                       2                                                 //
// |`\                     |`\                                               //
// |  `\                   |  `\                                             //
// |    `\                 5    `4                                           //
// |      `\               |      `\                                         //
// |        `\             |        `\                                       //
// 0----------1--> u       0-----3----1                                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Quad4:                 Quad8:                  Quad9:                     //
//                                                                           //
//       v                                                                   //
//       ^                                                                   //
//       |                                                                   //
// 3-----------2          3-----6-----2           3-----6-----2              //
// |     |     |          |           |           |           |              //
// |     |     |          |           |           |           |              //
// |     +---- | --> u    7           5           7     8     5              //
// |           |          |           |           |           |              //
// |           |          |           |           |           |              //
// 0-----------1          0-----4-----1           0-----4-----1              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Tetrahedron:                          Tetrahedron10:                      //
//                                                                           //
//                    v                                                      //
//                  .                                                        //
//                ,/                                                         //
//               /                                                           //
//            2                                     2                        //
//          ,/|`\                                 ,/|`\                      //
//        ,/  |  `\                             ,/  |  `\                    //
//      ,/    '.   `\                         ,6    '.   `5                  //
//    ,/       |     `\                     ,/       8     `\                //
//  ,/         |       `\                 ,/         |       `\              //
// 0-----------'.--------1 --> u         0--------4--'.--------1             //
//  `\.         |      ,/                 `\.         |      ,/              //
//     `\.      |    ,/                      `\.      |    ,9                //
//        `\.   '. ,/                           `7.   '. ,/                  //
//           `\. |/                                `\. |/                    //
//              `3                                    `3                     //
//                 `\.                                                       //
//                    ` w                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Hex8:                  Hex20:                  Hex27:                     //
//                                                                           //
//        v                                                                  //
// 3----------2            3----13----2           3----13----2               //
// |\     ^   |\           |\         |\          |\         |\              //
// | \    |   | \          | 15       | 14        |15    24  | 14            //
// |  \   |   |  \         9  \       11 \        9  \ 20    11 \            //
// |   7------+---6        |   7----19+---6       |   7----19+---6           //
// |   |  +-- |-- | -> u   |   |      |   |       |22 |  26  | 23|           //
// 0---+---\--1   |        0---+-8----1   |       0---+-8----1   |           //
//  \  |    \  \  |         \  17      \  18       \ 17    25 \  18          //
//   \ |     \  \ |         10 |        12|        10 |  21    12|           //
//    \|      w  \|           \|         \|          \|         \|           //
//     4----------5            4----16----5           4----16----5           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Prism6:                      Prism15:               Prism18:               //
//                                                                           //
//            w                                                              //
//            ^                                                              //
//            |                                                              //
//            3                       3                      3               //
//          ,/|`\                   ,/|`\                  ,/|`\             //
//        ,/  |  `\               12  |  13              12  |  13           //
//      ,/    |    `\           ,/    |    `\          ,/    |    `\         //
//     4------+------5         4------14-----5        4------14-----5        //
//     |      |      |         |      8      |        |      8      |        //
//     |    ,/|`\    |         |      |      |        |    ,/|`\    |        //
//     |  ,/  |  `\  |         |      |      |        |  15  |  16  |        //
//     |,/    |    `\|         |      |      |        |,/    |    `\|        //
//    ,|      |      |\        10     |      11       10-----17-----11       //
//  ,/ |      0      | `\      |      0      |        |      0      |        //
// u   |    ,/ `\    |    v    |    ,/ `\    |        |    ,/ `\    |        //
//     |  ,/     `\  |         |  ,6     `7  |        |  ,6     `7  |        //
//     |,/         `\|         |,/         `\|        |,/         `\|        //
//     1-------------2         1------9------2        1------9------2        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Pyr5:                            Pyr13/14:                                //
//                                                                           //
//                4                               4                          //
//              ,/|\                            ,/|\                         //
//            ,/ .'|\                         ,/ .'|\                        //
//          ,/   | | \                      ,/   | | \                       //
//        ,/    .' | `.                   ,/    .' | `.                      //
//      ,/      |  '.  \                ,7      |  12  \                     //
//    ,/       .' w |   \             ,/       .'   |   \                    //
//  ,/         |  ^ |    \          ,/         9    |    11                  //
// 0----------.'--|-3    `.        0--------6-.'----3    `.                  //
//  `\        |   |  `\    \         `\        |      `\    \                //
//    `\     .'   +----`\ - \ ->       `5     .'(13)    10   \               //
//      `\   |    `\     `\  \           `\   |           `\  \              //
//        `\.'      `\     `\`             `\.'             `\`              //
//           1----------------2               1--------8-------2             //
//                     `\                              `\                    //
//                        u                               u                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

struct Element {
  enum class Type {
    Unsupported,
    Line2, Line3,
    Tri3, Tri6,
    Quad4, Quad8, Quad9,
    Tet4, Tet10,
    Pyr5, Pyr13, Pyr14,
    Prism6, Prism15, Prism18,
    Hex8, Hex20, Hex27
  } type;

  std::vector< int > tags;
  std::vector< int > node_ids;
};

struct Mesh {
  std::vector< std::array< double, 3 > > nodes;
  std::vector< Element > elements;
};

enum class FileEncoding { ASCII, Binary };

Mesh import_stl(std::string filename);
Mesh import_vtk(std::string filename);
Mesh import_gmsh_v22(std::string filename);

bool export_stl(const Mesh & mesh, std::string filename);
bool export_vtk(const Mesh & mesh, std::string filename, FileEncoding enc);
bool export_vtu(const Mesh & mesh, std::string filename);
bool export_gmsh_v22(const Mesh & mesh, std::string filename, FileEncoding enc);

}