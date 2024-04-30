#ifndef OUTSIDE_MEASURERS_REPO_H
#define OUTSIDE_MEASURERS_REPO_H
#include "Models/OutsideMeasurer.h"
#include "Repos/IRepo.h"
class OutsideMeasurersRepo : public IRepo<OutsideMeasurer> {
public:
    ~OutsideMeasurersRepo();
    OutsideMeasurersRepo() = default;
    void initRepo(int maxSize) override;
    StatusCode add(OutsideMeasurer* item) override;
    StatusCode remove(OutsideMeasurer& refItem) override;
    // const OutsideMeasurer* getElements() const override;
    OutsideMeasurer& operator[](size_t index) const override;
    const int getCount() const override;

private:
    bool ready = false;
    OutsideMeasurer** array;
    int count = 0;
    int maxSize = 0;
    int getIndex(OutsideMeasurer* measurer);
    void shiftLeft(int startIndex);
    const int DEFAULT_MAX_LENGTH = 10;
};
#endif