#pragma once

typedef unsigned __int64 size_pr;

template <class T>
class ProgressReporter
{
public:
    ProgressReporter(T *owner) {
        m_Owner = owner;
    }

    void Report(size_pr total, size_pr progressed) {
        if (m_Owner) {
            m_Owner->Report(total, progressed);
        }
    }

private:
    T *m_Owner;
};

