#include "Repos/OutsideMeasurersRepo.h"

OutsideMeasurersRepo::~OutsideMeasurersRepo()
{
    for (int i = 0; i < count; i++) {
        delete &array[i];
    }
    delete[] array;
}

void OutsideMeasurersRepo::initRepo(int maxSize)
{
    if (maxSize <= 0) {
        Serial.println("Size cannot be negative!");
        this->maxSize = DEFAULT_MAX_LENGTH;
    } else {
        this->maxSize = maxSize;
    }
    // Instantialize array:
    array = new OutsideMeasurer*[maxSize];
    ready = true;
}
StatusCode OutsideMeasurersRepo::add(OutsideMeasurer* measurer)
{
    if (!ready) {
        return StatusCode::NotInitialized;
    }
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
    Serial.printf("Current count: %i\r\n", count);
    return StatusCode::NoError;
}
StatusCode OutsideMeasurersRepo::remove(OutsideMeasurer& refMeasurer)
{
    if (!ready) {
        return StatusCode::NotInitialized;
    }
    int elementIndex = getIndex(&refMeasurer);
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
// const OutsideMeasurer* OutsideMeasurersRepo::getElements() const
// {
//     return array;
// }
OutsideMeasurer& OutsideMeasurersRepo::operator[](size_t index) const
{

    static OutsideMeasurer dummy;
    if (index < 0 || index >= count || !ready) {
        Serial.println("Index out of range");
        return dummy;
    }
    return *array[index];
}
int OutsideMeasurersRepo::getIndex(OutsideMeasurer* measurer)
{
    if (!ready) {
        return -1;
    }
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
    array[count - 1] = nullptr;
}
