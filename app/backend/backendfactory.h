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

    Backend* CreateBackend(BackendTpye type,
                           const QString &username,
                           const QString &password,
                           const QString &locale,
                           const QString &target,
                           const QString &isoPath,
                           int installSize, int swapSize);

    Backend* CreateBackend(BackendTpye type);

private:
    BackendFactory(){;}
    ~BackendFactory();
    BackendFactory(const BackendFactory&){;}
};

}
