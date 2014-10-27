#pragma once

#include <QString>
#include <QDebug>

void InstallLogHandler();

namespace DeepinInstaller{

template<typename RetType>
class FunctionLoger {
public:
    FunctionLoger(const QString &funcname, RetType& ret):
    m_FunctionName(funcname), m_Ret(ret){
        qDebug()<<"Begin "<<funcname;
    }

    ~FunctionLoger(){
        qDebug()<<"End "<<m_FunctionName<<" Result: "<<m_Ret;
    }
private:
    QString m_FunctionName;
    RetType &m_Ret;
};

}
