#include "backendfactory.h"

#include "backend.h"

#ifdef Q_OS_WIN32
#include "winbackend.h"
#endif

#include <xsys.h>

namespace DeepinInstaller {

BackendFactory& BackendFactory::Instance() {
    static BackendFactory factory;
    return factory;
}

Backend* BackendFactory::CreateBackend(BackendTpye type, const QString &target, const QString &isoPath) {
    switch(type) {
#ifdef Q_OS_WIN32
    case Windows:
        return new DeepinInstaller::WindowsBackend(target, isoPath, Xapi::Username(), "sdsc=");
#endif
    default:
        return nullptr;
    }
}

}
