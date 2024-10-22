#include "config.hpp"

//      <objId, categoryId>
std::map<short, std::vector<Group>> CONFIG = {
    {1, { 
        {"regular blocks",          1, {1}},
        {"grid blocks",             83, {83, 2, 3, 4, 5, 502, 6, 7}},
        {"tile blocks",             69, {69, 70, 71, 72, 73, 74, 75}},
        {"chipped blocks",          76, {76, 77, 78, 80, 81, 82}},
        {"black blocks",            90, {90, 91, 92, 93, 94, 95, 96}},
        {"brick blocks",            116, {116, 117, 118, 119, 120, 121, 122}},

        {"colored regular blocks",  207, {207, 208, 209, 210, 211, 212, 213}},
        {"colored grid blocks",     1820, {1820, 1821, 1823, 1824, 1825, 1826, 1827, 1828}},
        {"colored tile blocks",     255, {255, 256, 257, 258, 259, 260, 261}},
        {"colored chipped blocks",  263, {263, 264, 265, 266, 267, 268}},
        {"colored brick blocks",    269, {269, 270, 271, 272, 273, 274, 275}},

        {"grayscale blocks",        1142, {1142, 1141, 1140, 1143, 1144, 1149, 1145, 1146, 1147, 1153, 1148, 1150, 1151, 1152}},
        {"beam blocks",             482, {476, 477, 478, 479, 480, 481, 482}},
        {"patterned blocks",        491, {485, 486, 487, 488, 489, 490, 491}},
        {"cross blocks",            644, {644, 650, 641, 1075, 646, 642, 739, 643, 645, 647, 648, 649, 1077, 1298, 1076}},
        {"pane blocks",             722, {658, 722, 659, 734, 653, 724, 654, 736, 657, 656, 723, 655, 735}},
        {"ground blocks",           752, {752, 753, 754, 755, 756, 757, 758, 759, 903, 904, 905, 911, 1045, 1046, 1294, 1295, 769, 770}},
        {"colored ground blocks",   952, {952, 953, 954, 955, 956, 957, 958, 959, 974, 975, 976, 977, 1047, 1048, 1296, 1297, 1299, 1300, 1301, 1302, 1303, 1304, 967, 968}},
        {"turn blocks",             1162, {1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1170, 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183, 1184, 1185, 1186, 1322, 1191, 1192, 1193, 1197, 1196, 1194, 1195}},

        {"striped blocks",          807, {807, 808, 809, 810, 811, 812, 813, 814, 815, 816, 817, 1078, 1079, 1080, 1081}},
        {"glass pane blocks",       817, {818, 819, 820, 821, 822, 823, 824, 825}},
        {"colored stripped blocks", 1082, {1082, 1100, 1083, 1101, 1084, 1102, 1085, 1103, 1086, 1104, 1088, 1105, 1089, 1106, 1087, 1090, 1099, 1095, 1096, 1097, 1098, 1110, 1111}},
        {"class tube blocks",       841, {841, 1112, 842, 1113, 843, 844, 1114, 845, 1115, 846, 1116, 847, 1117, 848, 1118}},
        {"big brick blocks",        867, {867, 868, 869, 870, 871, 872, 1266, 1267, 873, 874}},
        {"small brick blocks",      880, {880, 881, 882, 883, 884, 885, 1062, 1159, 1160, 1161}},
        {"bevel blocks",            890, {890, 1247, 1277, 1278, 1248, 1279, 1249, 1280, 1250, 1281, 1251, 1252, 1253, 1282, 1254, 1283, 1255, 1284, 1285, 1286, 1287, 1288, 1289, 1290}},
        {"rainbow blocks",          927, {927, 928, 929, 930, 931, 932, 933, 934, 935, 1229, 1230, 1238, 1231, 1232, 1233, 1234, 1237, 1235, 1236, 1239, 1240}},

        {"cobblestone blocks",      1348, {1348, 1349, 1350, 1351, 1395, 1356, 1357, 1358, 1359, 1360, 1361, 1362, 1363, 1364, 1365, 1366, 1777, 1780, 1778, 1779, 1785, 1786, 1787, 1788}},
        {"cobblestone bases",       1353, {1352, 1353, 1354, 1355, 1387, 1388, 1389, 1390, 1391, 1392, 1393, 1394, 1781, 1784, 1782, 1783, 1789, 1790, 1791, 1792}},
        {"cobblestone outlines",    1368, {1367, 1368, 1369, 1370, 1371, 1372, 1373, 1374, 1375, 1376, 1377, 1378, 1379, 1380, 1381, 1382, 1383, 1384, 1385, 1386}},

        {"cobblestone-2 blocks",    1431, {1431, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441, 1510, 1511, 1512, 1793, 1803, 1794, 1804, 1795, 1807, 1796, 1808}},
        {"cobblestone-2 bases",     1442, {1442, 1443, 1444, 1445, 1446, 1447, 1448, 1449, 1450, 1451, 1452, 1513, 1514, 1515, 1799, 1805, 1800, 1806, 1801, 1809, 1802, 1810}},
        {"cobblestone-2 outlines",  1453, {1453, 1454, 1455, 1456, 1457, 1458, 1459, 1460}},

        {"cliff blocks",            1461, {1461, 1462, 1463, 1464, 1769, 1770, 1771, 1772, 1773, 1774, 1775, 1776}},
        {"cliff edges",             1629, {1621, 1622, 1623, 1624, 1625, 1626, 1627, 1628, 1629, 1630, 1631, 1632, 1633, 1634, 1635, 1636, 1637, 1638, 1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647, 1648, 1649, 1650, 1651, 1652}},
        {"cliff outlines",          1653, {1653, 1654, 1655, 1656, 1657, 1658, 1659, 1660, 1661, 1662, 1663, 1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677, 1678, 1679, 1680, 1681, 1682, 1683, 1684}},

        {"ice crust blocks",        1471, {1471, 1507, 1617, 1472}},
        {"large rock",              1473, {1473, 1496}},
        {"jigsaw blocks",           1686, {1685, 1686, 1687, 1688, 1689, 1690, 1691, 1692, 1693, 1694, 1695, 1696, 1797, 1798}},

        {"volcano blocks",          1861, {1861, 1862, 1863, 1864}},
        {"stone brick blocks",      1865, {1865, 1866, 1867, 1868}},
        {"cracked blocks",          1869, {1869, 1870, 1871, 1872}},
        {"plank blocks",            1881, {1873, 1878, 1879, 1880, 1881}},
        {"moon blocks",             1874, {1874, 1875, 1876, 1877}},
        {"cracked blocks 2",        1882, {1882, 1883, 1884, 1885}},
        {"wavy blocks",             2703, {2703, 2704}},

        {"random top details",      2710, {2708, 2709, 2710, 2711, 2712, 2713, 2714, 2715, 2716, 2717, 2718, 2719, 2720, 2721, 2722, 2723, 2724, 2725, 2726, 2727, 2728, 2729, 2730, 2731, 2764, 2765, 2766, 2767, 2768, 2769, 2770, 2773}},
        {"random right details",    2734, {2732, 2733, 2734, 2735, 2736, 2737, 2738, 2739, 2740, 2741, 2742, 2743, 2776, 2777, 2778, 2779, 2780, 2781, 2782, 2783, 2784, 2785, 2786, 2787}},
        {"random left details",     2754, {2752, 2753, 2754, 2755, 2756, 2757, 2758, 2759, 2760, 2761, 2762, 2763, 2796, 2797, 2798, 2799, 2800, 2801, 2802, 2803, 2804, 2805, 2806, 2807}},
        {"random bottom details",   2928, {2744, 2745, 2746, 2747, 2748, 2749, 2750, 2751, 2788, 2927, 2928, 2789, 2929, 2930, 2790, 2931, 2932, 2791, 2933, 2934, 2792, 2935, 2936, 2793, 2937, 2938, 2794, 2939, 2940, 2795, 2941, 2942}},

        {"half-brick blocks",       2839, {2838, 2839, 2840, 2841, 2842, 2843, 2844, 2845, 2846, 2847, 2848, 2849}},
        {"shading blocks",          2852, {2850, 2851, 2852, 2853, 2854, 2855, 2856, 2857, 2858, 2859, 2860, 2861, 2862, 2863}},
        {"modern tile blocks",      2947, {2943, 2944, 2945, 2946, 2947, 2948, 2949, 2950, 2951, 2952, 2953, 2954, 2955, 2956, 2957, 2958, 2959, 2960}},
        {"shading blocks 2",        2961, {2961, 2962, 2963, 2964, 2965, 2966, 2967, 2968, 2969, 2970, 2971, 2972}},
        {"glowing blocks",          2973, {2973, 2974, 2975, 2976, 2977, 2978, 2979, 2980, 2981, 2982, 2983, 2984, 2985, 2986}},
        {"leaf blocks",             2988, {2987, 2988, 2989, 2990, 2991, 2992, 2993, 2994, 2995, 2996, 2997, 2998}},

        {"spike decor",             3034, {3034, 3038, 3042, 3046, 3035, 3039, 3043, 3047, 3036, 3040, 3044, 3048, 3037, 3041, 3045, 3049}},
        {"rounded blocks",          3050, {3050, 3051, 3052, 3053, 3054, 3067, 3055, 3068, 3056, 3069, 3057, 3070, 3058, 3071, 3059, 3072, 3060, 3073, 3061, 3074, 3062, 3075, 3063, 3076, 3064, 3077, 3065, 3078, 3066, 3079, 3090, 3091}},
        {"small shapes",            3081, {3080, 3081, 3082, 3083, 3084, 3085}},

        {"crystal block",           3086, {3086, 3087, 3088, 3089}},
        {"cloud block",             1919, {1919, 1920, 1921}},
        {"small turn blocks",       1924, {1922, 1923, 1924, 1925, 1926, 1927, 1928}},
        {"semi-blocks",             1903, {1903, 1904, 1905, 1910, 1911, 215, 220, 219}},
        {"wavy blocks",             62, {62, 66, 65, 68, 63, 64}},
        {"metallic blocks",         175, {170, 171, 172, 192, 173, 194, 174, 175, 176, 197}},

        {"breaking block",          143, {143}},
        {"small bevel blocks",      850, {850, 853, 854, 855, 856, 857, 859, 861, 862, 863, 891, 893, 894}},

        {"reference blocks",        2895, {2895, 2896, 2897}},

        // {"",            644, {}},


    }},

    {4, {
        {"regular spikes",          8, {8, 39, 103, 392}},
        {"colored spikes",          216, {216, 217, 218, 458}},
        {"invisible spikes",        144, {144, 205, 145, 459}},
        {"ice spikes",              177, {177, 178, 179}},
        {"fake spikes",             1889, {1889, 1890, 1891, 1892}},
        {"ground spikes",           1715, {1715, 1716, 1717, 1718}},
        {"wavy spikes",             1719, {1719, 1720, 1721, 1722, 1723, 1724}},
        {"bushes",                  135, {135}},
        {"colored bushes",          1711, {1711, 1712, 1713, 1714}},
        {"sharp spikes",            1725, {1725, 1726, 1727}},
        {"smooth spikes",           1728, {1728, 1729}},
        {"square spikes",           1730, {1730, 1731, 1732, 1733}},
    }},

    {6, {
        {"pads",                    35, {35, 140, 1332, 67, 3005}},
        {"orbs",                    36, {36, 141, 1333, 84, 1022, 1330, 3004}},
        {"gravity portals",         10, {10, 11, 2926}},
        {"mode portals",            13, {12, 13, 47, 111, 660, 745, 1331, 1933}},
        {"speed portals",           201, {200, 201, 202, 203, 1334}},
        {"special blocks",          1755, {1755, 1813, 1829, 1859, 2866}},
    }},

    {13, {
        {"area triggers",           3006, {3006, 3007, 3008, 3009, 3010, 3011, 3012, 3013, 3014, 3015, 3024}},
        {"camera triggers",         1913, {1913, 1914, 1916, 2901, 2015, 2062, 2925, 2016}},
        {"shaders",                 2904, {2904, 2905, 2907, 2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2919, 2920, 2921, 2922, 2923, 2924}},
        {"old enter triggers",      25, {22, 23, 24, 25, 26, 27, 28, 55, 56, 57, 58, 59, 1915}},
        {"new enter triggers",      3017, {3017, 3018, 3019, 3020, 3021, 3023}},
    }}
};

std::map<short, std::vector<Group>> * getCONFIG() {
    return &CONFIG;
}