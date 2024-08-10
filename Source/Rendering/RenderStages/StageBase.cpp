#include "StageBase.h"
#include <Rendering/Core/GfxDevice.h>

namespace MagicRed::Rendering
{
    StageBase::StageBase(const GfxDevice& _gfxDevice) : m_gfxDevice(_gfxDevice) {

    }

    StageBase::~StageBase() {
        
    }
}