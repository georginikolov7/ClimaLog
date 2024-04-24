#include "Repos/OutsideMeasurersRepo.h"

OutsideMeasurersRepo::~OutsideMeasurersRepo()
{
    delete[] array;
}

OutsideMeasurersRepo::OutsideMeasurersRepo(int maxSize)
{
    if (maxSize <= 0) {
        Serial.println("Size cannot be negative!");
        this->maxSize = DEFAULT_MAX_LENGTH;
    } else {
        this->maxSize = maxSize;
        // Instantialize array:
        array = new OutsideMeasurer[maxSize];
    }
}

StatusCode OutsideMeasurersRepo::add(OutsideMeasurer& measurer)
{
    if (getIndex(measurer) != -1) {
        Serial.println("Item already added to array");
        return StatusCode::ItemAlreadyAdded;
    }
    if (count == maxSize) {
        Serial.println("Repo is full!");
        return StatusCode::RepoFull;
    }
    // Save the new network:
    array[count] = measurer;
    // Increment the count:
    count++;
    Serial.println("added measurer to repo");
    return StatusCode::NoError;
}
StatusCode OutsideMeasurersRepo::remove(OutsideMeasurer& refMeasurer)
{
    int elementIndex = getIndex(refMeasurer);
    if (elementIndex != -1) {
        shiftLeft(elementIndex);
        count--;
        return StatusCode::NoError;
    } else {
        // error element does not exist:
        Serial.println("Element you are trying to delete does not exist in Repo!");
        return StatusCode::ItemIsNotAdded;
    }
}
const int OutsideMeasurersRepo::getCount() const
{
    return count;
}
const OutsideMeasurer* OutsideMeasurersRepo::getElements() const
{
    return array;
}
OutsideMeasurer& OutsideMeasurersRepo::operator[](size_t index) const
{
    static OutsideMeasurer dummy;
    if (index < 0 || index >= count) {
        Serial.println("Index out of range");
        return dummy;
    }
    return array[index];
}
int OutsideMeasurersRepo::getIndex(OutsideMeasurer& measurer)
{
    int index = -1;
    for (int i = 0; i < count; i++) {
        if (array[i] == measurer) {
            index = i;
        }
    }
    return index;
}

void OutsideMeasurersRepo::shiftLeft(int startIndex)
{
    // check startIndex:
    if (startIndex < 0 || startIndex >= count) {
        Serial.println("Index out of range!");
        return;
    }

    for (int i = startIndex + 1; i < count; i++) {
        array[i - 1] = array[i];
    }

    // Clear the last element:
    array[count - 1] = OutsideMeasurer();
}
