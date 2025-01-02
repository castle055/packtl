// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  packtl.cppm
 *! \brief
 *!
 */

export module packtl;
export import std;

export namespace packtl {
  //! @brief Concepts and Operations related to template parameter packs.
  template <typename...>
  struct pack;

  template <std::size_t...>
  struct integer_pack;


  //! <h2> CONCEPTS </h2>
  //! Contains
  template <typename What, typename... Args>
  concept Contains = (std::is_same_v<What, Args> || ...);

  //! <h2> CONDITIONS </h2>
  template <template <typename...> typename, typename...>
  struct is_type;
  template <typename...>
  struct has_item;
  template <typename...>
  struct share_items;

  //! <h2> OPERATORS </h2>
  template <typename...>
  struct get_first;
  template <typename...>
  struct get_last;
  template <typename...>
  struct get_size;
  template <std::size_t, typename...>
  struct get;
  template <typename...>
  struct append;
  template <typename...>
  struct prepend;
  template <std::size_t N, typename... T>
  struct remove_first;
  template <std::size_t N, typename... T>
  struct remove_first_in_pack;
  template <std::size_t N, typename... T>
  struct remove_last;
  template <std::size_t N, typename... T>
  struct shuffle_back;
  template <std::size_t FROM, std::size_t N, typename... T>
  struct slice;
  template <typename...>
  struct take_one_out_w_predicate;
  template <typename...>
  struct take_one_out;
  template <typename...>
  struct substitute;
  template <typename...>
  struct flatten;
  template <typename From, template <typename...> typename To>
  struct swap_pack;

  namespace impl {
    template <typename...>
    struct take_one_out_w_predicate;
    template <typename What, typename From>
    struct take_one_out;
    template <typename...>
    struct substitute;
    template <typename, std::size_t>
    struct flatten;
    template <std::size_t N, typename... T>
    struct remove_first;
  } // namespace impl

  //! is_type
  template <template <typename...> typename, typename...>
  struct is_type: std::false_type {};

  template <template <typename...> typename Pack, class... Args>
  struct is_type<Pack, Pack<Args...>>: std::true_type {};

  //! has_item
  template <typename...>
  struct has_item: std::false_type {};

  template <typename Item, template <typename...> typename Pack, typename... Args>
    requires packtl::Contains<Item, Args...>
  struct has_item<Item, Pack<Args...>>: std::true_type {};

  //! share_items
  template <typename...>
  struct share_items: std::false_type {};

  template <typename Pack1, template <typename...> typename Pack2, typename... Pack2Args>
    requires(has_item<Pack2Args, Pack1>::value || ...)
  struct share_items<Pack1, Pack2<Pack2Args...>>: std::true_type {};

  namespace impl {
    //! take_one_out
    template <typename What, template <typename...> typename From, typename A1, typename... Args>
    struct take_one_out<What, From<A1, Args...>> {
      using type = typename prepend<A1, typename take_one_out<What, From<Args...>>::type>::type;
    };

    template <typename What, template <typename...> typename From, typename A1, typename... Args>
      requires std::is_same_v<What, A1>
    struct take_one_out<What, From<A1, Args...>> {
      using type = From<Args...>;
    };

    //! take_one_out_w_predicate
    template <
      template <typename...>
      typename Predicate,
      typename What,
      template <typename...>
      typename From,
      typename A1,
      typename... Args>
      requires(Predicate<What, A1>::value)
    struct take_one_out_w_predicate<Predicate<What, void>, From<A1, Args...>> {
      using type = From<Args...>;
    };

    template <
      template <typename...>
      typename Predicate,
      typename What,
      template <typename...>
      typename From,
      typename A1,
      typename... Args>
      requires(!Predicate<What, A1>::value)
    struct take_one_out_w_predicate<Predicate<What, void>, From<A1, Args...>> {
      using type = typename prepend<
        A1,
        typename take_one_out_w_predicate<Predicate<What, void>, From<Args...>>::type>::type;
    };

    template <
      template <typename...>
      typename Predicate,
      template <typename...>
      typename From,
      typename A1,
      typename... Args>
      requires(Predicate<A1>::value)
    struct take_one_out_w_predicate<Predicate<void>, From<A1, Args...>> {
      using type = From<Args...>;
    };

    template <
      template <typename...>
      typename Predicate,
      template <typename...>
      typename From,
      typename A1,
      typename... Args>
      requires(!Predicate<A1>::value)
    struct take_one_out_w_predicate<Predicate<void>, From<A1, Args...>> {
      using type = typename prepend<
        A1,
        typename take_one_out_w_predicate<Predicate<void>, From<Args...>>::type>::type;
    };


    //! substitute
    template <typename What, typename As, template <typename> typename From, typename A1>
      requires std::same_as<What, A1>
    struct substitute<What, As, From<A1>> {
      using type = As;
    };

    template <typename What, typename As, template <typename> typename From, typename A1>
      requires(!std::same_as<What, A1>)
    struct substitute<What, As, From<A1>> {
      using type = A1;
    };

    template <
      typename What,
      typename As,
      template <typename, typename...>
      typename From,
      typename A1,
      typename... Args>
      requires std::same_as<What, A1>
    struct substitute<What, As, From<A1, Args...>> {
      using type =
        typename prepend<As, From<typename substitute<What, As, From<Args...>>::type>>::type;
    };

    template <
      typename What,
      typename As,
      template <typename, typename...>
      typename From,
      typename A1,
      typename... Args>
      requires(!std::same_as<What, A1>)
    struct substitute<What, As, From<A1, Args...>> {
      using type =
        typename prepend<A1, From<typename substitute<What, As, From<Args...>>::type>>::type;
    };

    //! flatten
    template <template <typename...> typename Pack, typename... Args>
    struct flatten<Pack<Args...>, 0> {
      using type = Pack<Args...>;
    };

    template <template <typename...> typename Pack, typename P1, typename... PRest, std::size_t N>
      requires(is_type<Pack, P1>::value && N > 0)
    struct flatten<Pack<P1, PRest...>, N> {
    private:
      using appended = typename append<Pack<PRest...>>::template to<P1>;

    public:
      using type = typename packtl::flatten<typename appended::type>::type;
    };

    template <template <typename...> typename Pack, typename P1, typename... PRest, std::size_t N>
      requires(not is_type<Pack, P1>::value && N > 0)
    struct flatten<Pack<P1, PRest...>, N> {
      using type = typename flatten<Pack<PRest..., P1>, N - 1>::type;
    };

    //! remove_first
    template <std::size_t N>
    struct remove_first<N> {
      using type = pack<>;
    };

    template <std::size_t N, typename First, typename... T>
      requires(N == 0)
    struct remove_first<N, First, T...> {
      using type = pack<First, T...>;
    };

    template <std::size_t N, typename First, typename... T>
      requires(N > 0)
    struct remove_first<N, First, T...> {
      using type = typename remove_first<N - 1, T...>::type;
    };

  } // namespace impl

  //! get_first
  template <typename First, typename... Rest>
  struct get_first<First, Rest...> {
    using type = First;
  };

  template <template <typename...> typename Pack, typename First, typename... Rest>
  struct get_first<Pack<First, Rest...>> {
    using type = First;
  };

  //! get_last
  template <typename... T>
    requires(sizeof...(T) > 0)
  struct get_last<T...> {
    using type = typename get<sizeof...(T) - 1, T...>::type;
  };

  template <template <typename...> typename Pack, typename... T>
    requires(sizeof...(T) > 0)
  struct get_last<Pack<T...>> {
    using type = typename get_last<T...>::type;
  };

  //! get_size
  template <>
  struct get_size<> {
    static constexpr std::size_t value = 0UL;
  };

  template <typename First, typename... Rest>
  struct get_size<First, Rest...> {
    static constexpr std::size_t value = 1UL + sizeof...(Rest);
  };

  template <template <typename...> typename Pack>
  struct get_size<Pack<>> {
    static constexpr std::size_t value = 0UL;
  };

  template <template <typename...> typename Pack, typename First, typename... Rest>
  struct get_size<Pack<First, Rest...>> {
    static constexpr std::size_t value = 1UL + sizeof...(Rest);
  };

  //! get
  template <typename First, typename... Rest>
  struct get<0, First, Rest...> {
    using type = First;
  };

  template <std::size_t I, typename First, typename... Rest>
  struct get<I, First, Rest...> {
    using type = typename get<(I - 1), Rest...>::type;
  };

  template <template <typename...> typename Pack, typename First, typename... Rest>
  struct get<0, Pack<First, Rest...>> {
    using type = First;
  };

  template <std::size_t I, template <typename...> typename Pack, typename First, typename... Rest>
  struct get<I, Pack<First, Rest...>> {
    using type = typename get<(I - 1), Pack<Rest...>>::type;
  };

  template <template <std::size_t...> typename Pack, std::size_t First, std::size_t... Rest>
  struct get<0, Pack<First, Rest...>> {
    static constexpr std::size_t value = First;
  };

  template <
    std::size_t I,
    template <std::size_t...>
    typename Pack,
    std::size_t First,
    std::size_t... Rest>
  struct get<I, Pack<First, Rest...>> {
    static constexpr std::size_t value = get<(I - 1), Pack<Rest...>>::value;
  };


  //! append
  template <template <typename...> typename Pack1, typename... Args1>
  struct append<Pack1<Args1...>> {
    template <typename... T>
    struct to;

    template <template <typename...> typename Pack2, typename... Args2>
    struct to<Pack2<Args2...>> {
      using type = Pack2<Args2..., Args1...>;
    };
  };

  template <template <std::size_t...> typename Pack1, std::size_t... Args1>
  struct append<Pack1<Args1...>> {
    template <typename... T>
    struct to;

    template <template <std::size_t...> typename Pack2, std::size_t... Args2>
    struct to<Pack2<Args2...>> {
      using type = Pack2<Args2..., Args1...>;
    };
  };

  template <typename... What>
  struct append {
    template <typename... T>
    struct to;

    template <template <typename...> typename Pack, typename... Args>
    struct to<Pack<Args...>> {
      using type = Pack<Args..., What...>;
    };
  };

  template <typename What, template <typename...> typename From, typename... Args>
  struct append<What, From<Args...>> {
    using type = From<Args..., What>;
  };


  //! prepend
  template <typename... What>
  struct prepend {
    template <typename... T>
    struct to;

    template <template <typename...> typename Pack, typename... Args>
    struct to<Pack<Args...>> {
      using type = Pack<What..., Args...>;
    };
  };

  template <typename What, template <typename...> typename From, typename... Args>
  struct prepend<What, From<Args...>> {
    using type = From<What, Args...>;
  };


  //! take_one_out_w_predicate
  template <
    template <typename...>
    typename BoolPredicate,
    typename PredicateArg1,
    template <typename...>
    typename From,
    typename... Args>
  struct take_one_out_w_predicate<BoolPredicate<PredicateArg1, void>, From<Args...>> {
    using type = typename impl::
      take_one_out_w_predicate<BoolPredicate<PredicateArg1, void>, From<Args...>>::type;
  };

  template <
    template <typename...>
    typename BoolPredicate,
    template <typename...>
    typename From,
    typename... Args>
  struct take_one_out_w_predicate<BoolPredicate<void>, From<Args...>> {
    using type = typename impl::take_one_out_w_predicate<BoolPredicate<void>, From<Args...>>::type;
  };

  //! take_one_out
  template <typename What, template <typename...> typename From, typename... Args>
    requires(!Contains<What, Args...>)
  struct take_one_out<What, From<Args...>> {
    using type = From<Args...>;
  };

  template <typename What, template <typename...> typename From, typename... Args>
    requires Contains<What, Args...>
  struct take_one_out<What, From<Args...>> {
    using type = typename impl::take_one_out<What, From<Args...>>::type;
  };


  //! substitute
  template <typename What, typename As, template <typename...> typename From, typename... Args>
    requires Contains<What, Args...>
  struct substitute<What, As, From<Args...>> {
    using type = typename impl::substitute<What, As, From<Args...>>::type;
  };


  template <typename What, typename As, typename From>
    requires std::same_as<What, From>
  struct substitute<What, As, From> {
    using type = As;
  };

  template <typename What, typename As, typename From>
    requires(!std::same_as<What, From>)
  struct substitute<What, As, From> {
    using type = From;
  };

  //! flatten
  template <template <typename...> typename Pack, typename... Args>
  struct flatten<Pack<Args...>> {
    using type = typename impl::flatten<Pack<Args...>, sizeof...(Args)>::type;
  };

  //! swap_pack
  template <template <typename...> typename From, template <typename...> typename To, typename... T>
  struct swap_pack<From<T...>, To> {
    using type = To<T...>;
  };

  //! remove_first
  template <std::size_t N, typename... T>
    requires(N < (sizeof...(T)))
  struct remove_first<N, T...> {
    using type = typename impl::remove_first<N, T...>::type;
  };

  template <std::size_t N, typename... T>
    requires(N >= (sizeof...(T)))
  struct remove_first<N, T...> {
    using type = pack<>;
  };

  //! remove_first_in_pack
  template <std::size_t N, template <typename...> typename Pack, typename... T>
    requires(N <= (sizeof...(T) + 1))
  struct remove_first_in_pack<N, Pack<T...>> {
  private:
    using result_pack = typename impl::remove_first<N, T...>::type;

  public:
    using type = typename swap_pack<result_pack, Pack>::type;
  };

  //! remove_last
  template <std::size_t N, typename... T>
  struct remove_last {
  private:
    using shuffled = typename shuffle_back<sizeof...(T) - N, T...>::type;
    using removed  = typename remove_first_in_pack<N, shuffled>::type;

  public:
    using type = removed;
  };

  template <std::size_t N, template <typename...> typename Pack, typename... T>
  struct remove_last<N, Pack<T...>> {
  private:
    using result_pack = typename remove_last<N, T...>::type;

  public:
    using type = typename swap_pack<result_pack, Pack>::type;
  };

  //! shuffle_back
  template <std::size_t N, typename First, typename... T>
    requires(N == 0)
  struct shuffle_back<N, First, T...> {
    using type = pack<First, T...>;
  };

  template <std::size_t N, typename First, typename... T>
    requires(N != 0)
  struct shuffle_back<N, First, T...> {
    using type = typename shuffle_back<N - 1, T..., First>::type;
  };

  template <std::size_t N, template <typename...> typename Pack, typename... T>
  struct shuffle_back<N, Pack<T...>> {
  private:
    using result_pack = typename shuffle_back<N, T...>::type;

  public:
    using type = typename swap_pack<result_pack, Pack>::type;
  };

  //! slice
  template <std::size_t FROM, std::size_t N, typename First, typename... T>
    requires(FROM >= 0 && FROM + N <= (sizeof...(T) + 1))
  struct slice<FROM, N, First, T...> {
  private:
    using without_head = typename remove_first<FROM, First, T...>::type;
    using without_tail =
      typename remove_first<get_size<without_head>::value - N, without_head>::type;

  public:
    using type = without_tail;
  };

  template <std::size_t FROM, std::size_t N, template <typename...> typename Pack, typename... T>
  struct slice<FROM, N, Pack<T...>> {
  private:
    using result_pack = typename slice<FROM, N, T...>::type;

  public:
    using type = typename swap_pack<result_pack, Pack>::type;
  };
} // namespace packtl
