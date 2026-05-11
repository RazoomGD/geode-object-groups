#pragma once

#include <Geode/loader/Event.hpp>
#include <Geode/binding/CreateMenuItem.hpp>

namespace razoom {

class ObjectFoundEvent : public geode::Event<ObjectFoundEvent, bool(CreateMenuItem*)> {
    using Event::Event;
};

}