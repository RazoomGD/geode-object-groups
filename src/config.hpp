#pragma once
#include <vector>
#include <map>

struct Group {
    const char* m_groupName;
    short m_thumbnailObjectId;
    std::vector<short> m_objectIds;
};

//      <objId, categoryId>
std::map<short, short> categoryByFirstObjectId = {
    {1, 1},
    {467, 2},
    {1743, 3},
    {8, 4},
    {506, 5},
    {35, 6},
    {2065, 7},
    {2083, 8},
    {1329, 9},
    {3801, 10},
    {41, 11},
    {1705, 12},
    {31, 13},
};

//      <categoryId, groups>
std::map<short, std::vector<Group>> CONFIG = {
    {1, { 
        {"regular-blocks",          1, {1}},
        {"regular-blocks",          83, {83}}, // todo
        {"regular-blocks",          2, {2}}, // todo
        {"grid-blocks",             83, {83, 2, 3, 4, 5, 502, 6, 7}},
        {"tile-blocks",             69, {69, 70, 71, 72, 73, 74, 75}},
        {"chipped-blocks",          76, {76, 77, 78, 80, 81, 82}},
        {"black-blocks",            90, {90, 91, 92, 93, 94, 95, 96}},
        {"brick-blocks",            116, {116, 117, 118, 119, 120, 121, 122}},

        {"colored-regular-blocks",  207, {207, 208, 209, 210, 211, 212, 213}},
        {"colored-grid-blocks",     1820, {1820, 1821, 1823, 1824, 1825, 1826, 1827, 1828}},
        {"colored-tile-blocks",     255, {255, 256, 257, 258, 259, 260, 261}},
        {"colored-chipped-blocks",  263, {263, 264, 265, 266, 267, 268}},
        {"colored-brick-blocks",    269, {269, 270, 271, 272, 273, 274, 275}},
    }},
};

