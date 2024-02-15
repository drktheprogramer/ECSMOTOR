namespace GameEngine{
	namespace MP{
		
        template<typename T, T VAL>
        struct constant
        {
            static constexpr T value {VAL};
        };

        struct true_t : constant<bool, true> {};
        struct false_t : constant<bool, false> {};

		template<typename T, typename U>
		struct is_same : false_t {};
		
		template<typename T>
		struct is_same<T, T> : true_t {};

        template<typename T, typename U>
        constexpr bool is_same_v = is_same<T, U>::value;
		

	}
    template<typename ...Ts>
    struct TypeList {
        consteval static std::size_t size() noexcept { return sizeof ... (Ts); }
    };

    template<typename TYPELIST>
    struct type_traits {};

    template<typename CMPS, typename TAGS>
    struct GameEngine
    {
        using tags = type_traits<TAGS>;

        //template<typename CMP>
        //consteval static int getMask() noexcept {return (1 << CMP::id); }
    };
	
}


