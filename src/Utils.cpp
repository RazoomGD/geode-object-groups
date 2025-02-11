#include "ObjectGroups.hpp"


// colors: 1-green, 2-cyan, 3-pink, 4-gray, 5-darker gray, 6-red
CreateMenuItem* getCustomCreateBtn(int id, int bg, bool doRegister) {
    CreateMenuItem* btn;
    auto editor = Global::get().m_editorUI;
    if (bg <= 6) {
        btn = editor->getCreateBtn(id, bg);
    } else {
        btn = editor->getCreateBtn(id, 1);
        if (auto btnSpr = typeinfo_cast<ButtonSprite*>(btn->getChildren()->objectAtIndex(0)))
        switch (bg) {
            case 7: {btnSpr->updateBGImage("OG_button_07.png"_spr); break;}
            case 8: {btnSpr->updateBGImage("OG_button_08.png"_spr); break;}
            case 9: {btnSpr->updateBGImage("OG_button_09.png"_spr); break;}
            case 10: {btnSpr->updateBGImage("OG_button_10.png"_spr); break;}
            default: break;
        }
    }
    if (!doRegister && editor->m_createButtonArray->lastObject() == btn) {
        editor->m_createButtonArray->removeLastObject();
    }
    return btn;
}

// void recursiveSetChildrenColor(CCSprite* parent, ccColor3B col) {
//     if (auto children = parent->getChildren()) {
//         for (unsigned i = 0; i < children->count(); i++) {
//             auto child = typeinfo_cast<CCSprite*>(children->objectAtIndex(i));
//             if (child) {
//                 child->setColor(col);
//                 recursiveSetChildrenColor(child, col);
//             }
//         }
//     }
// }

// void setColorToCreateBtn(CreateMenuItem* cmi, ccColor3B col) {
//     if (auto children = cmi->getChildren()) {
//         for (int i = 0; i < children->count(); i++) {
//             // recursively set color starting from ButtonSprite children
//             if (auto btnSpr = typeinfo_cast<ButtonSprite*>(children->objectAtIndex(i))) {
//                 recursiveSetChildrenColor(btnSpr, col);
//                 break;
//             }
//         }
//     }
// }

void setColorToCreateBtnNew(CreateMenuItem* cmi, bool isBright) {
    // ! mostly decompiled code of EditorUI::updateCreateMenu() that sets the color

    ccColor3B color = isBright ? ccc3(255, 255, 255) : ccc3(127, 127, 127);
    if (auto spr = cmi->getChildByType<ButtonSprite>(0)) {
        if (spr->m_subBGSprite) {
            spr->m_subBGSprite->setColor(color); // button bg sprite
        }

        if (auto gameObj = typeinfo_cast<GameObject*>(spr->m_subSprite)) {
            int objId;
            if (gameObj->m_unk4D0 == 1) {
                bool cVar14;
                if (gameObj->m_customColorType == 0) {
                    cVar14 = gameObj->m_maybeNotColorable;
                } else {
                    cVar14 = (gameObj->m_customColorType == 1);
                }
                if (cVar14 || gameObj->m_colorSprite || 
                    gameObj->m_baseColor->m_defaultColorID == 0x3ec || 
                    gameObj->m_baseColor->m_defaultColorID == 0x0  || 
                    /* (*(char *)(gameObj + 0xdf) == '\0') */ false) {
                    goto LAB_14010da56;
                }
                color = isBright ? ccc3(200, 200, 255) : ccc3(100, 100, 127);
                goto LAB_14010dac2;
            }
LAB_14010da56:
            objId = gameObj->m_objectID;
            bool bVar12;
            if (objId < 0x531) {
                if (((objId == 0x530) || (objId == 0x396)) || (objId == 0x397)) goto LAB_14010daad;
                bVar12 = (objId == 0x52f);
LAB_14010da89:
                if (bVar12) goto LAB_14010daad;
                auto piVar3 = gameObj->m_baseColor;
                if (piVar3 != 0) {
                    objId = piVar3->m_colorID;
                    if ((piVar3->m_defaultColorID == objId) || (objId == 0x0)) {
                        objId = piVar3->m_defaultColorID;
                    }
                    if (objId == 0x3f2) goto LAB_14010daad;
                }
            }
            else {
                if (objId != 0x630) {
                    bVar12 = (objId == 0x7dc);
                    goto LAB_14010da89;
                }
LAB_14010daad:
                // color = ccc3(0, 0, 0);
                color = isBright ? ccc3(0, 0, 0) : ccc3(127, 127, 127);
            }
LAB_14010dac2:
            gameObj->setObjectColor(color);
            color = isBright ? ccc3(200, 200, 255) : ccc3(100, 100, 127);
            gameObj->setChildColor(color);
        }
    }
}

void getBarSize(int* rows, int* cols) {
    *cols = GameManager::sharedState()->getIntGameVariable("0049");
    *rows = GameManager::sharedState()->getIntGameVariable("0050");
}

// lol these numbers are really hardcoded in RobTop's code
static const std::set<short> darkerButtonBgObjIds = {
    146, 147, 204, 206, 673, 674, 1340, 1340, 1341, 1342, 1343, 1344, 1345,
    144, 145, 205, 459, 
    498, 499, 500, 501, 277, 278, 719, 721, 990, 992, 1120, 1122, 1123, 1124, 1125, 1126, 1127, 1132, 1133,
    1134, 1135, 1136, 1137, 1138, 1139, 1241, 1242, 1243, 1244, 1245, 1246,
    1292, 1010, 1009, 1271, 1272, 1760, 1761, 1887, 1011, 1012, 1013, 1269, 1270, 1293, 1762, 1763, 1888,
    740, 741, 742
};

int getItemBtnColor(short objId) {
    return darkerButtonBgObjIds.contains(objId) ? 5 : 4;
}

int getGroupBtnColor() {
    return Global::get().m_settings.m_groupColor;
}

bool divideGridAlignedObjects(CCArrayExt<GameObject*> objects, std::vector<std::vector<short>>* result) {
    const float minGap = 15.0;
    const float maxWidth = 30.0;

    std::sort(objects.begin(), objects.end(), [](GameObject* a, GameObject* b) {
        return a->getPositionY() > b->getPositionY(); // descending
    });

    // divide into rows
    std::vector<GameObject*> row;
    std::vector<std::vector<GameObject*>> matrix;
    float rowBegin = objects[0]->getPositionY();
    float rowLast = rowBegin;

    for (auto& obj : objects) {
        if (obj->getPositionY() < rowLast - minGap) {
            // found a gap => start a new row from this object
            matrix.push_back(std::move(row));
            row.clear();
            rowBegin = obj->getPositionY();
            rowLast = rowBegin;
            row.push_back(obj);
        } else {
            if (obj->getPositionY() < rowBegin - maxWidth) {
                // row is too wide, so we assume that objects are not grid-aligned
                result->clear();
                return false;
            } else {
                rowLast = obj->getPositionY();
                row.push_back(obj);
            }
        }
    }

    // divide into columns
    result->clear();
    std::vector<std::vector<GameObject*>::iterator> iters; // iterators for each row
    int rowCount = matrix.size();

    for (int i = 0; i < rowCount; i++) {
        std::sort(matrix[i].begin(), matrix[i].end(), [](GameObject* a, GameObject* b) {
            return a->getPositionX() < b->getPositionX(); // ascending
        });
        result->push_back({});
        iters.push_back(matrix[i].begin());
    }

    float columnBegin = -1e15f;
    float columnLast = -1e15f;

    for (;;) {
        std::vector<std::pair<GameObject*, int>> maybeColumn;
        int objCount = 0;
        for (int i = 0; i < rowCount; i++) {
            if (iters[i] != matrix[i].end()) {
                maybeColumn.push_back({*iters[i], i});
                objCount++;
            } else {
                maybeColumn.push_back({nullptr, i});
            }
        }

        if (objCount == 0) break; // no more objects

        std::sort(maybeColumn.begin(), maybeColumn.end(), 
            [](std::pair<GameObject*, int>* a, std::pair<GameObject*, int>* b) {
                float xA = (a->first == nullptr) ? 1e15f : a->first->getPositionX();
                float xB = (b->first == nullptr) ? 1e15f : b->first->getPositionX();
                return xA < xB; // ascending, nullptr-s at the end
            }
        );

        // build actual column
        columnBegin = maybeColumn[0].first->getPositionX();
        if (columnBegin - columnLast < minGap) {
            // columns are too close, so we assume that objects are not grid-aligned
            result->clear();
            return false;
        }
        columnLast = columnBegin;

        std::vector<GameObject*> column(rowCount, nullptr);

        for (int i = 0; i < rowCount; i++) {
            auto obj = maybeColumn[i].first;

            if (obj == nullptr || obj->getPositionX() > columnLast + minGap) {
                // found a gap or ran out of objects => finish this column
                break;
            } else {
                if (obj->getPositionX() > columnBegin + maxWidth) {
                    // column is too wide, so we assume that objects are not grid-aligned
                    result->clear();
                    return false;
                } else {
                    columnLast = obj->getPositionX();
                    int realIndex = maybeColumn[i].second;
                    column[realIndex] = obj;
                    iters[realIndex]++;
                }
            }
        }

        // write changes
        for (int i = 0; i < rowCount; i++) {
            if (column[i] != nullptr) {
                result->at(i).push_back(column[i]->m_objectID);
            } else {
                result->at(i).push_back(0);
            }
        }
    }
    
    return true; // tomorrow I will have no idea how it works
}