#ifndef COMPONENT_H
#define COMPONENT_H
#include "gameobject/GameObject.h"
#include <memory>

class Component {
public:
    GameObjectEH gameObject;

    Component(const GameObjectEH& go);

    virtual ~Component() { }
};

using ComponentPtr = std::shared_ptr<Component>;

#endif  // COMPONENT_H
