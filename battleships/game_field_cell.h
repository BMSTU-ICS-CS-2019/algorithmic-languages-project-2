#pragma once


#include <set>
#include <cstddef>
#include <cstdint>
#include <utility>
#include "direction.h"
#include "ship_position.h"

using std::set;

namespace battleships {

    class GameFieldCell {

    public:
        virtual ~GameFieldCell() = default;

        /**
         * @brief Checks if thi cell has been discovered.
         *
         * @return {@code true} if this cell has been discovered and {@code false} otherwise
         */
        [[nodiscard]] virtual bool is_discovered() const = 0;

        [[nodiscard]] virtual bool is_empty() const = 0;

        virtual void discover() = 0;
    };

    class AbstractGameFieldCell : public GameFieldCell {

    protected:
        bool discovered_ = false;
    public:

        [[nodiscard]] bool is_discovered() const override {
            return discovered_;
        }

        void discover() override {
            discovered_ = true;
        }
    };

    class EmptyGameFieldCell final : public AbstractGameFieldCell {

    public:
        [[nodiscard]] bool is_empty() const override {
            return true;
        }
    };

    class ShipGameFieldCell final : public AbstractGameFieldCell {

    protected:
        const size_t ship_size_ = 1;
        const ShipPosition position_ = NONE;

    public:
        explicit ShipGameFieldCell(const size_t &ship_size, const ShipPosition &position) :
                ship_size_(ship_size), position_(position) {}

        [[nodiscard]] bool is_empty() const override {
            return false;
        }

        [[nodiscard]] size_t get_ship_size() const {
            return ship_size_;
        }

        [[nodiscard]] ShipPosition get_position() const {
            return position_;
        }
    };
}