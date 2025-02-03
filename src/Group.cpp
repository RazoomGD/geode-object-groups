#include "ObjectGroups.hpp"

Group* Group::create(std::string name, short thumbnailObjId, std::vector<std::vector<short>>&& matrix) {
    auto ret = new Group();
    if (!ret || !ret->init()) {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    ret->m_groupName = name;
    ret->m_thumbnailObjId = thumbnailObjId;
    ret->m_matrix = matrix;
    ret->autorelease();
    return ret;
}
