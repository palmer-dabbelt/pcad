// See LICENSE for details

#include "randomize_x_assign.h++"
#include <pcad/hdlast/builtin_wire.h++>
#include <iostream>
using namespace pcad;

std::vector<hdlast::statement::ptr> passes::randomize_x_assign(
    const hdlast::wire::ptr& dest,
    const hdlast::wire::ptr& source
) {
    if (dest->width() != source->width()) {
        std::cerr << "Unable to assign wires of different widths\n";
        abort();
    }
    auto width = dest->width();

    if (dest->depth() != 1 || source->depth() != 1) {
        std::cerr << "Unable to assign wires of non-1 depth\n";
        abort();
    }

    if (width == 1) {
        return {
            std::make_shared<hdlast::assign_statement>(
                std::make_shared<hdlast::wire_statement>(dest),
                std::make_shared<hdlast::wire_statement>(source)
            )
        };
    }

    auto out = std::vector<hdlast::statement::ptr>();
    for (ssize_t i = 0; i < width; ++i) {
        auto buffer_value =
            std::make_shared<hdlast::index_statement>(
                std::make_shared<hdlast::wire_statement>(source),
                std::make_shared<hdlast::wire_statement>(
                    std::make_shared<hdlast::literal>(i, width)
                )
            );
        auto dest_value =
            std::make_shared<hdlast::index_statement>(
                std::make_shared<hdlast::wire_statement>(dest),
                std::make_shared<hdlast::wire_statement>(
                    std::make_shared<hdlast::literal>(i, width)
                )
            );

        auto buffer_value_is_x =
            std::make_shared<hdlast::biop_statement>(
                hdlast::biop_statement::op::EQEQEQ,
                buffer_value,
                std::make_shared<hdlast::wire_statement>(
                    std::make_shared<hdlast::x>()
                )
            );

        out.push_back(
            std::make_shared<hdlast::assign_statement>(
                dest_value,
                std::make_shared<hdlast::trop_statement>(
                    buffer_value_is_x,
                    std::make_shared<hdlast::wire_statement>(
                        std::make_shared<hdlast::random>()
                    ),
                    buffer_value
                )
            )
        );
    }
    return out;
}
