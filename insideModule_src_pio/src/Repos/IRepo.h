#ifndef IREPO_H
#define IREPO_H
#include "StatusCodes.h"
template <typename T>
class IRepo {
public:
    virtual ~IRepo() = default;
    virtual StatusCode add(T& item) = 0;
    virtual StatusCode remove(T& refItem) = 0;
    const virtual T* getElements() const = 0;
    virtual T& operator[](size_t index) const = 0;
    const virtual int getCount() const = 0;
};
#endif