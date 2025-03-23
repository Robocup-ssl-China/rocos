#ifndef __ZOS_META_H__
#define __ZOS_META_H__
namespace zos{
namespace concepts{
template<typename T,typename... Args>
concept are_convertiable = std::conjunction_v<std::is_convertible<Args,T>...>;
template<typename T>
concept Serializable = requires(T t,void* p,size_t size){
    { t.ByteSize() } -> std::convertible_to<std::size_t>;
    { t.SerializeToArray(p,size) } -> std::convertible_to<bool>;
};
} // namespace zos::concepts

#define ALIAS_TEMPLATE_FUNCTION(highLevelF, lowLevelF) \
template<typename... Args> \
inline auto highLevelF(Args&&... args) -> decltype(lowLevelF(std::forward<Args>(args)...)) { \
    return lowLevelF(std::forward<Args>(args)...); \
}
} // namespace zos

#endif // __ZOS_META_H__
