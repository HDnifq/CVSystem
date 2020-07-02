//
//  base64 encoding and decoding with C++.
//  Version: 1.01.00
//

#ifndef BASE64_H_F1FE6B2F_6334_4EB9_9087_636546C741DD
#define BASE64_H_F1FE6B2F_6334_4EB9_9087_636546C741DD

#include <string>
#include <vector>

namespace dxlib {
namespace cvsystem {

class base64
{
  public:
    static std::string encode(unsigned char const*, size_t len);
    static std::vector<unsigned char> decode(std::string const& s);
};

} // namespace cvsystem
} // namespace dxlib

#endif /* BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A */
