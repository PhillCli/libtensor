#include <sstream>
#include "version.h"

namespace libutil {


const unsigned version::k_major = 1;
const unsigned version::k_minor = 1;
const char *version::k_status = "trunk";
const char *version::k_authors[] = {
    "Evgeny Epifanovsky"
};


version::version() : m_status(k_status) {

    std::ostringstream ss;
    ss << k_major << "." << k_minor << "-" << m_status;
    m_string = ss.str();

    size_t nauthors = sizeof(k_authors)/sizeof(char*);
    for(size_t i = 0; i < nauthors; i++) {
        m_authors.push_back(std::string(k_authors[i]));
    }
}


} // namespace libutil
