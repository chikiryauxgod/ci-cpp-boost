#include "HashService.hpp"
#include <boost/uuid/detail/sha1.hpp>
#include <sstream>

std::string HashService::Hash(const std::string& input) const {
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(input.data(), input.size());

    unsigned int digest[5];
    sha1.get_digest(digest);

    std::ostringstream os;
    for (auto d : digest) {
        os << std::hex << d;
    }
    return os.str();
}