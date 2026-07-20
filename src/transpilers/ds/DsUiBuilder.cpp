#include "DsUiBuilder.h"
#include "DsEffectBuilder.h"
#include "../../core/IUiVisitor.h"
#include "../../core/models/UiComponents.h"

void DsUiBuilder::buildUi(DsNode* rootUi, const ProjectManager* pm, const QString& bgImageName) {
    rootUi->setAttribute("width", 812);
    rootUi->setAttribute("height", 375);
    if (!bgImageName.isEmpty()) rootUi->setAttribute("bgImage", bgImageName);
    
    DsNode* kbNode = rootUi->addChild("keyboard");
    for (const auto& color : pm->getAudioState()->getKeyboardColors()) {
        DsNode* colorNode = kbNode->addChild("color");
        colorNode->setAttribute("loNote", color.loNote);
        colorNode->setAttribute("hiNote", color.hiNote);
        colorNode->setAttribute("color", "FF" + QString(color.color).remove("#"));
    }
    DsNode* tabNode = rootUi->addChild("tab");
    tabNode->setAttribute("name", "main");
    
    class UiTranspileVisitor : public IUiVisitor {
        DsNode* tabNode;
        const ProjectManager* pm;
        
        void buildBindings(DsNode* parentNode, const UiComponent* c) {
            if (!c->targetNodeId.isNull() && !c->targetProperty.isEmpty()) {
                Node* targetNode = pm->getNode(c->targetNodeId);
                if (targetNode) {
                    QString bType = "effect";
                    if (targetNode->type == "SampleGroup") {
                        if (c->targetProperty.startsWith("ampEnv")) bType = "amp";
                        else bType = "group";
                    }
                    DsNode* bindingNode = parentNode->addChild("binding");
                    bindingNode->setAttribute("type", bType);
                    
                    QString translatedParam = c->targetProperty.toUpper();
                    if (bType == "effect") {
                        int pos = DsEffectBuilder::getEffectPosition(pm, targetNode);
                        if (pos >= 0) bindingNode->setAttribute("position", pos);
                        
                        if (targetNode->type == "Delay") {
                            if (c->targetProperty == "time") translatedParam = "FX_DELAY_TIME";
                            else if (c->targetProperty == "feedback") translatedParam = "FX_DELAY_FEEDBACK";
                        } else if (targetNode->type == "Reverb") {
                            if (c->targetProperty == "size") translatedParam = "FX_REVERB_ROOM_SIZE";
                            else if (c->targetProperty == "damp") translatedParam = "FX_REVERB_DAMPING";
                        } else if (targetNode->type == "Filter") {
                            if (c->targetProperty == "cutoff") translatedParam = "FX_FILTER_FREQUENCY";
                            else if (c->targetProperty == "resonance") translatedParam = "FX_FILTER_RESONANCE";
                        }
                    }
                    
                    if (bType == "amp") {
                        if (c->targetProperty == "attack") translatedParam = "ENV_ATTACK";
                        else if (c->targetProperty == "decay") translatedParam = "ENV_DECAY";
                        else if (c->targetProperty == "sustain") translatedParam = "ENV_SUSTAIN";
                        else if (c->targetProperty == "release") translatedParam = "ENV_RELEASE";
                    }
                    
                    bindingNode->setAttribute("parameter", translatedParam);
                } else {
                    // Check if it's a Macro
                    bool isMacro = false;
                    for (const auto& macro : pm->getAudioState()->getGlobalMacros()) {
                        if (macro.id == c->targetNodeId) {
                            isMacro = true;
                            QString macroSource = "MACRO_" + macro.id.toString(QUuid::WithoutBraces);
                            
                            // Find all routings for this macro in all SampleGroups
                            for (const auto& pair : pm->getAllNodes()) {
                                if (pair.second->type == "SampleGroup") {
                                    SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
                                    for (const auto& r : sg->routings) {
                                        if (r.source == macroSource) {
                                            DsNode* bindingNode = parentNode->addChild("binding");
                                            
                                            QString bType = "group";
                                            QString translatedParam = r.destination.toUpper();
                                            
                                            // Handle specific translations
                                            if (r.destination.startsWith("AmpEnv_")) {
                                                bType = "amp";
                                                QString sub = r.destination.mid(7).toUpper();
                                                translatedParam = "ENV_" + sub;
                                            } else if (r.destination == "Pitch") {
                                                translatedParam = "TUNING";
                                            } else if (r.destination == "Volume") {
                                                translatedParam = "VOLUME";
                                            } else if (r.destination == "Pan") {
                                                translatedParam = "PAN";
                                            }
                                            
                                            bindingNode->setAttribute("type", bType);
                                            bindingNode->setAttribute("parameter", translatedParam);
                                            // For Macro bindings, translation table maps knob value (0-1) to parameter range
                                            bindingNode->setAttribute("translation", "linear");
                                            
                                            // We'd ideally need outputMin and outputMax from the routing amount, 
                                            // but for now let's map according to amount.
                                            // amount goes from -1.0 to 1.0. A Macro is 0.0 to 1.0.
                                            // For now, let's simplify and just map to the min/max of the parameter
                                            // multiplied by the amount.
                                            double paramMax = 1.0;
                                            if (translatedParam == "TUNING") paramMax = 12.0;
                                            else if (translatedParam == "VOLUME") paramMax = 12.0;
                                            else if (translatedParam == "PAN") paramMax = 1.0;
                                            
                                            double range = paramMax * r.amount;
                                            // In a true implementation, we need baseValue + (range * macroValue).
                                            // DS doesn't support complex math in bindings natively without custom curves,
                                            // but we can set outputMin and outputMax if the parameter supports it,
                                            // or output factor. Since DS relies heavily on linear maps:
                                            // We will output a translation min/max.
                                            // Wait, standard DS `translation` is usually handled via `translation="table"` if we want complex logic, 
                                            // but `linear` is default.
                                            // We will just leave `translation` linear for now.
                                            
                                            // A better way is to use "table" translation.
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    public:
        UiTranspileVisitor(DsNode* t, const ProjectManager* p) : tabNode(t), pm(p) {}
        
        void visit(const UiKnob* k) override {
            DsNode* kNode = tabNode->addChild(k->filmstripPath.isEmpty() ? "labeled-knob" : "control");
            if (!k->filmstripPath.isEmpty()) {
                kNode->setAttribute("style", k->isVertical ? "custom_skin_vertical_drag" : "custom_skin_rotary");
                kNode->setAttribute("customSkinImage", k->filmstripPath);
                kNode->setAttribute("customSkinNumFrames", k->frameCount);
            } else {
                kNode->setAttribute("text", k->name);
            }
            kNode->setAttribute("x", k->x); kNode->setAttribute("y", k->y);
            kNode->setAttribute("width", k->width); 
            kNode->setAttribute("value", k->currentValue);
            buildBindings(kNode, k);
        }
        void visit(const UiSlider* s) override {
            DsNode* sNode = tabNode->addChild(s->filmstripPath.isEmpty() ? "slider" : "control");
            if (!s->filmstripPath.isEmpty()) {
                sNode->setAttribute("style", s->isVertical ? "custom_skin_vertical_drag" : "custom_skin_rotary");
                sNode->setAttribute("customSkinImage", s->filmstripPath);
                sNode->setAttribute("customSkinNumFrames", s->frameCount);
            }
            sNode->setAttribute("x", s->x); sNode->setAttribute("y", s->y);
            sNode->setAttribute("width", s->width); sNode->setAttribute("height", s->height);
            sNode->setAttribute("value", s->currentValue);
            buildBindings(sNode, s);
        }
        void visit(const UiButton* b) override {
            DsNode* bNode = tabNode->addChild("button");
            bNode->setAttribute("x", b->x); bNode->setAttribute("y", b->y);
            bNode->setAttribute("width", b->width); bNode->setAttribute("height", b->height);
            bNode->setAttribute("value", b->isPressed ? 1 : 0);
            
            if (!b->imagePathOn.isEmpty() || !b->imagePathOff.isEmpty()) {
                bNode->setAttribute("style", "image");
                DsNode* stateOn = bNode->addChild("state");
                stateOn->setAttribute("name", "On");
                if (!b->imagePathOn.isEmpty()) stateOn->setAttribute("mainImage", b->imagePathOn);
                
                DsNode* stateOff = bNode->addChild("state");
                stateOff->setAttribute("name", "Off");
                if (!b->imagePathOff.isEmpty()) stateOff->setAttribute("mainImage", b->imagePathOff);
            }
            
            buildBindings(bNode, b);
        }
        void visit(const UiLabel* l) override {
            DsNode* lNode = tabNode->addChild("label");
            lNode->setAttribute("x", l->x); lNode->setAttribute("y", l->y);
            lNode->setAttribute("width", l->width); lNode->setAttribute("height", l->height);
            lNode->setAttribute("text", l->text);
            lNode->setAttribute("textColor", "FF" + QString(l->colorHex).remove("#"));
            lNode->setAttribute("textSize", l->fontSize);
        }
        void visit(const UiMenu* m) override {
            DsNode* mNode = tabNode->addChild("menu");
            mNode->setAttribute("x", m->x); mNode->setAttribute("y", m->y);
            mNode->setAttribute("width", m->width); mNode->setAttribute("height", m->height);
            mNode->setAttribute("requireSelection", "true");
            for (int i = 0; i < m->options.size(); ++i) {
                DsNode* optNode = mNode->addChild("option");
                optNode->setAttribute("name", m->options[i]);
                optNode->setAttribute("value", i);
            }
            buildBindings(mNode, m);
        }
        void visit(const UiXYPad* p) override {
            DsNode* pNode = tabNode->addChild("xy-pad");
            pNode->setAttribute("x", p->x); pNode->setAttribute("y", p->y);
            pNode->setAttribute("width", p->width); pNode->setAttribute("height", p->height);
            buildBindings(pNode, p);
        }
        void visit(const UiShape* s) override {
            if (s->shapeType == "Rectangle") {
                DsNode* rNode = tabNode->addChild("rectangle");
                rNode->setAttribute("x", s->x); rNode->setAttribute("y", s->y);
                rNode->setAttribute("width", s->width); rNode->setAttribute("height", s->height);
                rNode->setAttribute("fillColor", "FF" + QString(s->fillColorHex).remove("#"));
            }
        }
        void visit(const UiImage* i) override {
            DsNode* iNode = tabNode->addChild("image");
            iNode->setAttribute("x", i->x); iNode->setAttribute("y", i->y);
            iNode->setAttribute("width", i->width); iNode->setAttribute("height", i->height);
            iNode->setAttribute("path", i->imagePath);
        }
        void visit(const UiOscilloscope* o) override {
            DsNode* oNode = tabNode->addChild("oscilloscope");
            oNode->setAttribute("x", o->x); oNode->setAttribute("y", o->y);
            oNode->setAttribute("width", o->width); oNode->setAttribute("height", o->height);
        }
        void visit(const UiLine* l) override {
            DsNode* lNode = tabNode->addChild("line");
            lNode->setAttribute("x1", l->x); lNode->setAttribute("y1", l->y);
            lNode->setAttribute("x2", l->x + l->width); lNode->setAttribute("y2", l->y + l->height);
            lNode->setAttribute("lineColor", "FF" + QString(l->color).remove("#"));
            lNode->setAttribute("lineThickness", l->thickness);
        }
        void visit(const UiMultiFrameImage* m) override {
            DsNode* mNode = tabNode->addChild("multiFrameImage");
            mNode->setAttribute("x", m->x); mNode->setAttribute("y", m->y);
            mNode->setAttribute("width", m->width); mNode->setAttribute("height", m->height);
            mNode->setAttribute("path", m->imagePath);
            mNode->setAttribute("orientation", m->orientation);
            mNode->setAttribute("numFrames", m->numFrames);
        }
        void visit(const UiKeyboard*) override {}
        void visit(const UiTab*) override {}
    };
    
    UiTranspileVisitor visitor(tabNode, pm);
    for (const auto& pair : pm->getAllUiComponents()) {
        pair.second->accept(&visitor);
    }
}
