#pragma once

#include "geom.h"

#include <algorithm>
#include <vector>

namespace collision_detector {

enum class ItemType {
	Loot,
	Office
};

struct CollectionResult {
    bool IsCollected(double collect_radius) const {
        return proj_ratio >= 0 && proj_ratio <= 1 && sq_distance <= collect_radius * collect_radius;
    }

    double sq_distance;
    double proj_ratio;
};

CollectionResult TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c);

struct Item {
    Item(unsigned _id, const geom::Point2D& pos, double wdth, ItemType type = ItemType::Loot)
    : id{_id}, item_type{type}, position{pos}, width{wdth} {}

	unsigned id;
	ItemType item_type;
    geom::Point2D position;
    double width;
};

struct Gatherer {
    geom::Point2D start_pos;
    geom::Point2D end_pos;
    double width;
};

class ItemGathererProvider {
protected:
    ~ItemGathererProvider() = default;

public:
    virtual size_t ItemsCount() const = 0;
    virtual Item GetItem(size_t idx) const = 0;
    virtual size_t GatherersCount() const = 0;
    virtual Gatherer GetGatherer(size_t idx) const = 0;
};

struct GatheringEvent {
    size_t item_id;
    size_t gatherer_id;
    double sq_distance;
    double time;
};

class ItemGatherer : public ItemGathererProvider {
public:
	ItemGatherer(std::vector<Item> items,
                 std::vector<Gatherer> gatherers)
        : items_(items)
        , gatherers_(gatherers) {
    }


    size_t ItemsCount() const override {
        return items_.size();
    }

    Item GetItem(size_t idx) const override {
        return items_[idx];
    }

    size_t GatherersCount() const override {
        return gatherers_.size();
    }

    Gatherer GetGatherer(size_t idx) const override {
        return gatherers_[idx];
    }

private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherers_;
};

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider);
}  // namespace collision_detector
