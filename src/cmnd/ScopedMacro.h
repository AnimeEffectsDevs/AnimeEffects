#ifndef CMND_SCOPEDMACRO_H
#define CMND_SCOPEDMACRO_H

#include "cmnd/Listener.h"
#include "cmnd/Stack.h"
#include "util/LifeLink.h"

namespace cmnd {

class ScopedMacro {
    Stack& mStack;

public:
    ScopedMacro(Stack& aStack, const QString& aText): mStack(aStack) {
        mStack.beginMacro(aText);
    }

    ~ScopedMacro() {
        mStack.endMacro();
    }

    void setValidLink(util::LifeLink& aLink) {
        mStack.setMacroValidLink(aLink);
    }

    void grabListener(Listener* aListener) {
        mStack.grabMacroListener(aListener);
    }
};

} // namespace cmnd

#endif // CMND_SCOPEDMACRO_H
