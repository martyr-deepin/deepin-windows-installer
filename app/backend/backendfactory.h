#pragma once

#include <QString>

namespace DeepinInstaller {

class Backend;

class BackendFactory
{
public:
    enum BackendTpye{
        Windows,
    };

    static BackendFactory& Instance();

    Backend* CreateBackend(BackendTpye, const QString&, const QString&);

private:
    BackendFactory(){;}
    BackendFactory(const BackendFactory&){;}
};

}
