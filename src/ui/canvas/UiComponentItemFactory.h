#pragma once
#include "UiComponentItem.h"

#include "../designer/UiDesignerExport.h"

class UIDESIGNER_EXPORT UiComponentItemFactory {
public:
    static UiComponentItem* create(const UiComponent* comp, QGraphicsItem* parent = nullptr);
};
