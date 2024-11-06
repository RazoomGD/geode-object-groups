#pragma once

#include <vector>
#include <map>
#include <cmath>
#include <fstream>
#include <format>
#include <string>

// objects that should have darker button background
const std::set<short> darkerButtonBgObjIds = {
    146, 147, 204, 206, 673, 674, 1340, 1340, 1341, 1342, 1343, 1344, 1345,
    144, 145, 205, 459, 
    498, 499, 500, 501, 277, 278, 719, 721, 990, 992, 1120, 1122, 1123, 1124, 1125, 1126, 1127, 1132, 1133,
    1134, 1135, 1136, 1137, 1138, 1139, 1241, 1242, 1243, 1244, 1245, 1246,
    1292, 1010, 1009, 1271, 1272, 1760, 1761, 1887, 1011, 1012, 1013, 1269, 1270, 1293, 1762, 1763, 1888,
    740, 741, 742
};

//      <categoryId, groups>
const std::map<short, short> categoryByFirstObjectId = {
    {1, 1},     {35, 6},    {41, 11},
    {467, 2},   {2065, 7},  {1705, 12},
    {1743, 3},  {2083, 8},  {31, 13},
    {8, 4},     {1329, 9},
    {506, 5},   {3801, 10},
};

#define GROUP_HIDE (short) 0x1
#define GROUP_UNGROUP (short) 0x2

struct Group {
    std::string m_groupName;
    short m_thumbnailObjectId;
    std::vector<short> m_objectIds;
    short m_properties = 0;
};

std::map<short, std::vector<Group>> * getCONFIG();

bool writeConfigToJson(std::string filename);

bool readConfigFromJson(std::string filename, std::vector<std::string> * errList);
