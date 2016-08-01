// See LICENSE for details

#ifndef PCAD__PASSES__INFER_DECOUPLED_IO_H
#define PCAD__PASSES__INFER_DECOUPLED_IO_H

#include <pcad/hdlast/module.h++>
#include <pcad/hdlast/port.h++>
#include <memory>

namespace pcad {
    namespace passes {
        class decoupled_io {
        public:
            typedef std::shared_ptr<decoupled_io> ptr;

        private:
            const std::string _base_name;
            const std::vector<hdlast::port::ptr> _ports;
            const hdlast::port_direction _direction;

        public:
            decoupled_io(const decltype(_base_name)& base_name,
                         const decltype(_ports)& ports,
                         const decltype(_direction)& direction)
            : _base_name(base_name),
              _ports(ports),
              _direction(direction)
            {}

        public:
            const decltype(_base_name)& base_name(void) const { return _base_name; }
            const decltype(_ports)& ports(void) const { return _ports; }
            const decltype(_direction)& direction(void) const { return _direction; }
        };

        /* Looks at a module and tries to figure out which decoupled IOs exist
         * within that module.  This currently just does name mangling and
         * doesn't verify _anything_, so be careful! */
        std::vector<decoupled_io::ptr>
        infer_decoupled_io(const hdlast::module::ptr& module);
    }
}

#endif
