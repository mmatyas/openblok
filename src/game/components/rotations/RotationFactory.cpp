#include "RotationFactory.h"

#include "Classic.h"
#include "TGM.h"
#include "SRS.h"

#include <assert.h>


std::unique_ptr<RotationFn> RotationFactory::make(RotationStyle style)
{
    switch (style) {
        case RotationStyle::CLASSIC: return std::make_unique<Rotations::Classic>();
        case RotationStyle::TGM: return std::make_unique<Rotations::TGM>();
        case RotationStyle::SRS: return std::make_unique<Rotations::SRS>();
    }
    assert(false);
}
