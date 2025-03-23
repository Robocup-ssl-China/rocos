#ifndef __ZOS_META_TYPES_H__
#define __ZOS_META_TYPES_H__
#include <functional>
#include "zos/data.h"
namespace zos{
namespace type{
    using socket_callback_type = std::function<void(const void*,size_t)>;
    using callback_type = std::function<void(const zos::Data&)>;
} // namespace zos::type
} // namespace zos
#endif // __ZOS_META_TYPES_H__