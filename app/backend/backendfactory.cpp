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

Backend* BackendFactory::CreateBackend(BackendTpye type,
                                       const QString &username,
                                       const QString &hostname,
                                       const QString &password,
                                       const QString &locale,
                                       const QString &target,
                                       const QString &isoPath,
                                       int installSize,
                                       int swapSize) {
    switch(type) {
#ifdef Q_OS_WIN32
    case Windows:
        return new WindowsBackend(username,
                                  hostname,
                                  password,
                                  locale,
                                  target,
                                  isoPath,
                                  installSize,
                                  swapSize);
#endif
    default:
        return NULL;
    }
}

Backend* BackendFactory::CreateBackend(BackendTpye type) {
    return CreateBackend(type, "", "", "", "", "", "", 0,0);
}

BackendFactory::~BackendFactory(){

}

}
