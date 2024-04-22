#ifndef STATUSCODES_H
#define STATUSCODES_H
#include <Arduino.h>
enum class StatusCode : uint8_t {
NoError,
NotInitialized,
IndexOutOfRange,
InvalidFormat,
RepoEmpty,
RepoFull,
ItemAlreadyAdded,
ItemIsNotAdded,
NameNullOrEmpty
};
#endif