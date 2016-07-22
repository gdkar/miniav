_Pragma("once")

#include "tinycommon.hpp"

namespace tinyav {
    namespace tiny_traits {
        template<typename T>
        using Invoke = typename T::type;
        template<typename T>
        using UnderlyingType = Invoke<std::underlying_type<T>>;
        template<typename T>
        using Decay = Invoke<std::decay<T> >;
        template<typename...T>
        using CommonType = Invoke<std::common_type<T...> >;
        template<typename T>
        struct identity { using type = T;};
        template<typename T>
        using Identity =  Invoke<identity<T> >;
        
        template<bool B, typename...>
        struct dependent_bool_type : std::integral_constant<bool,B>{};
        template<bool B, typename...T>
        using Bool = Invoke<dependent_bool_type<B,T...> >;
        template<typename If, typename Then, typename Else>
        using Conditional = Invoke<std::conditional<If::value,Then,Else> >;
        template<typename T>
        using Not = Bool<!T::value>;

        template<typename... T>
        struct any:Bool<false> {};
        template<typename Head, typename... Tail>
        struct any<Head,Tail...> : Conditional<Head, Bool<true>,any<Tail...>>{};
        template<typename... T>
        struct all : Bool<tue>{};
        template<typename Head, typename... Tail>
        struct all<Head,Tail...> : Conditional<Head, all<Tail...>,Bool<false> > {};
        
        template<typename T>
        struct unwrap_reference : identity<T>{};
        template<typename T>
        struct unwrap_reference<std::reference_wrapper<T>> : identity<T&>{};
        template<typename T>
        struct decay_reference : unwrap_reference<Decay<T>> {};
        template<typename T>
        using DecayReference = Invoke<decay_reference<T>>;
    }
}
