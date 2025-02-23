#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/FLAlertLayer.hpp>

using namespace geode::prelude;

class $modify(EditUI, EditorUI) {
    void quickFill(CCObject* sender) {
        if (auto selectedObjs = this->getSelectedObjects(); selectedObjs->count() == 2) {
            auto obj1 = static_cast<GameObject*>(selectedObjs->objectAtIndex(0));
            auto obj2 = static_cast<GameObject*>(selectedObjs->objectAtIndex(1));
            auto obj1Pos = obj1->getPosition();
            auto obj2Pos = obj2->getPosition();
            auto properties = Mod::get()->getSettingValue<bool>("use-obj-2") ? obj2 : obj1;
            if (obj1Pos == obj2Pos) {
                FLAlertLayer::create("Quick Fill", "Both objects have the same position, so nothing was done :3", "Ok")->show();
                return;
            }
            if (!Mod::get()->getSettingValue<bool>("dont-use-workaround")) {
                this->selectObject(properties, true);
                m_copyValuesBtn->activate();
            }
            
            this->selectObjects(selectedObjs, true);
            m_trashBtn->activate(); // workaround to make undo work and not crash

            this->updateGridNodeSize(); // workaround to getting the correct gridsize
            float gridSize = m_gridSize;
            
            std::vector<CCPoint> points;
          
            float startX = std::min(obj1Pos.x, obj2Pos.x);
            float endX = std::max(obj1Pos.x, obj2Pos.x);
            float startY = std::min(obj1Pos.y, obj2Pos.y);
            float endY = std::max(obj1Pos.y, obj2Pos.y);
        
            for (float x = startX; x <= endX; x += gridSize) {
                for (float y = startY; y <= endY; y += gridSize) {
                    points.push_back({x, y});
                }
            }

            int id = (Mod::get()->getSettingValue<bool>("use-obj-2") ? obj2 : obj1)->m_objectID;
            auto editorLayer = LevelEditorLayer::get();
            CCArray objs;
            
            for (const auto& point : points) {
                auto obj = editorLayer->createObject(id, point, false);
                
                if (Mod::get()->getSettingValue<bool>("copy-values")) {
                    
                    if (Mod::get()->getSettingValue<bool>("dont-use-workaround")) {
                        // add some easy to get values
                        obj->setScaleX(properties->m_scaleX);
                        obj->setScaleY(properties->m_scaleY);
                        obj->setRotation(properties->getObjectRotation());
                        obj->setFlipX(properties->m_isFlipX);
                        obj->setFlipY(properties->m_isFlipY);
                    }
                    else {
                        obj->setScaleX(properties->m_scaleX);
                        obj->setScaleY(properties->m_scaleY);
                        obj->setRotation(properties->getObjectRotation());
                        obj->setFlipX(properties->m_isFlipX);
                        obj->setFlipY(properties->m_isFlipY);

                        this->selectObject(obj, true);
                        m_pasteStateBtn->activate();
                        m_pasteColorBtn->activate();
                    }
                }
                
                objs.addObject(obj);
            }     
            
            this->deselectAll();
            if (Mod::get()->getSettingValue<bool>("select-on-fill")) {
                this->selectObjects(&objs, true);
            }
            this->updateButtons();
        }
        else FLAlertLayer::create("Quick Fill", "You have to select 2 objects for this function to work, so, uh, do that :3", "Ok")->show();
    }  
    
    void createMoveMenu() {
		EditorUI::createMoveMenu();
        auto* btn = this->getSpriteButton("Sheet.png"_spr, menu_selector(EditUI::quickFill), nullptr, 1);
        m_editButtonBar->m_buttonArray->addObject(btn);
        auto rows = GameManager::sharedState()->getIntGameVariable("0049");
        auto cols = GameManager::sharedState()->getIntGameVariable("0050");
        m_editButtonBar->reloadItems(rows, cols);
	}
};
