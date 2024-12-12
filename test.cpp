// The MIT License (MIT)
//
// Copyright (c) 2024 Howard Hinnant
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Our apologies.  When the previous paragraph was written, lowercase had not yet
// been invented (that would involve another several millennia of evolution).
// We did not mean to shout.

#include "bbi.h"
#include <array>
#include <cstdint>
#include <iostream>

template <class T>
void
test_trivial_special_members()
{
    static_assert(std::is_trivially_destructible_v         <T>);
    static_assert(std::is_trivially_default_constructible_v<T>);
    static_assert(std::is_trivially_copy_constructible_v   <T>);
    static_assert(std::is_trivially_copy_assignable_v      <T>);
    static_assert(std::is_trivially_move_constructible_v   <T>);
    static_assert(std::is_trivially_move_assignable_v      <T>);
}

// SignTag

static_assert(bbi::Signed{} == bbi::Signed{});
static_assert(bbi::Signed{} != bbi::Unsigned{});
static_assert(bbi::Unsigned{} != bbi::Signed{});
static_assert(bbi::Unsigned{} == bbi::Unsigned{});

// Policy

static_assert(bbi::Wrap{} == bbi::Wrap{});
static_assert(bbi::Wrap{} != bbi::Saturate{});
static_assert(bbi::Wrap{} != bbi::Terminate{});
static_assert(bbi::Wrap{} != bbi::Throw{});

static_assert(bbi::Saturate{} != bbi::Wrap{});
static_assert(bbi::Saturate{} == bbi::Saturate{});
static_assert(bbi::Saturate{} != bbi::Terminate{});
static_assert(bbi::Saturate{} != bbi::Throw{});

static_assert(bbi::Terminate{} != bbi::Wrap{});
static_assert(bbi::Terminate{} != bbi::Saturate{});
static_assert(bbi::Terminate{} == bbi::Terminate{});
static_assert(bbi::Terminate{} != bbi::Throw{});

static_assert(bbi::Throw{} != bbi::Wrap{});
static_assert(bbi::Throw{} != bbi::Saturate{});
static_assert(bbi::Throw{} != bbi::Terminate{});
static_assert(bbi::Throw{} == bbi::Throw{});

// ProperIntSize

static_assert(!bbi::ProperIntSize<0>);
static_assert(!bbi::ProperIntSize<1>);
static_assert(!bbi::ProperIntSize<2>);
static_assert(bbi::ProperIntSize<8>);
static_assert(bbi::ProperIntSize<16>);
static_assert(bbi::ProperIntSize<32>);
static_assert(bbi::ProperIntSize<64>);
static_assert(bbi::ProperIntSize<128>);
static_assert(bbi::ProperIntSize<256>);
static_assert(bbi::ProperIntSize<512>);
static_assert(bbi::ProperIntSize<1024>);
static_assert(bbi::ProperIntSize<0x8000'0000>);

// StandardInteger

static_assert(bbi::detail::StandardSignedInteger<std::int8_t>);
static_assert(bbi::detail::StandardSignedInteger<std::int16_t>);
static_assert(bbi::detail::StandardSignedInteger<std::int32_t>);
static_assert(bbi::detail::StandardSignedInteger<std::int64_t>);

static_assert(bbi::detail::StandardUnsignedInteger<std::uint8_t>);
static_assert(bbi::detail::StandardUnsignedInteger<std::uint16_t>);
static_assert(bbi::detail::StandardUnsignedInteger<std::uint32_t>);
static_assert(bbi::detail::StandardUnsignedInteger<std::uint64_t>);

static_assert(!bbi::detail::StandardInteger<bool>);
static_assert(!bbi::detail::StandardInteger<char>);
static_assert(!bbi::detail::StandardInteger<char8_t>);
static_assert(!bbi::detail::StandardInteger<char16_t>);
static_assert(!bbi::detail::StandardInteger<char32_t>);
static_assert(!bbi::detail::StandardInteger<wchar_t>);

// common_type

static_assert(std::is_same_v<std::common_type_t<std::int8_t, bbi::wrap::i8>, bbi::wrap::i8>);
static_assert(std::is_same_v<std::common_type_t<std::int8_t, bbi::wrap::i16>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<std::int8_t, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::int8_t, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::int8_t, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::int8_t, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<std::int8_t, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<std::int8_t, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<std::int16_t, bbi::wrap::i8>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<std::int16_t, bbi::wrap::i16>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<std::int16_t, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::int16_t, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::int16_t, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::int16_t, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<std::int16_t, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<std::int16_t, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<std::int32_t, bbi::wrap::i8>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::int32_t, bbi::wrap::i16>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::int32_t, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::int32_t, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::int32_t, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::int32_t, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<std::int32_t, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<std::int32_t, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<std::int64_t, bbi::wrap::i8>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::int64_t, bbi::wrap::i16>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::int64_t, bbi::wrap::i32>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::int64_t, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::int64_t, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::int64_t, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<std::int64_t, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<std::int64_t, bbi::wrap::i1024>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<std::uint8_t, bbi::wrap::i8>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<std::uint8_t, bbi::wrap::i16>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<std::uint8_t, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::uint8_t, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::uint8_t, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::uint8_t, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<std::uint8_t, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<std::uint8_t, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<std::uint16_t, bbi::wrap::i8>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::uint16_t, bbi::wrap::i16>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::uint16_t, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<std::uint16_t, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::uint16_t, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::uint16_t, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<std::uint16_t, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<std::uint16_t, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<std::uint32_t, bbi::wrap::i8>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::uint32_t, bbi::wrap::i16>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::uint32_t, bbi::wrap::i32>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::uint32_t, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<std::uint32_t, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::uint32_t, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<std::uint32_t, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<std::uint32_t, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<std::uint64_t, bbi::wrap::i8>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::uint64_t, bbi::wrap::i16>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::uint64_t, bbi::wrap::i32>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::uint64_t, bbi::wrap::i64>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::uint64_t, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<std::uint64_t, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<std::uint64_t, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<std::uint64_t, bbi::wrap::i1024>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, std::int8_t>, bbi::wrap::i8>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, std::int16_t>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, std::int32_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, std::int64_t>, bbi::wrap::i64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, std::uint8_t>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, std::uint16_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, std::uint32_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, std::uint64_t>, bbi::wrap::i128>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::i8>, bbi::wrap::i8>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::i16>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::u8>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::u16>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::u32>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::u64>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::u128>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::u256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i8, bbi::wrap::u512>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, std::int8_t>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, std::int16_t>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, std::int32_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, std::int64_t>, bbi::wrap::i64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, std::uint8_t>, bbi::wrap::u8>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, std::uint16_t>, bbi::wrap::u16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, std::uint32_t>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, std::uint64_t>, bbi::wrap::u64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::i8>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::i16>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::u8>, bbi::wrap::u8>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::u16>, bbi::wrap::u16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::u32>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::u64>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::u128>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::u256>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::u512>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u8, bbi::wrap::u1024>, bbi::wrap::u1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, std::int8_t>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, std::int16_t>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, std::int32_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, std::int64_t>, bbi::wrap::i64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, std::uint8_t>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, std::uint16_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, std::uint32_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, std::uint64_t>, bbi::wrap::i128>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::i8>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::i16>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::u8>, bbi::wrap::i16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::u16>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::u32>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::u64>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::u128>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::u256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i16, bbi::wrap::u512>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, std::int8_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, std::int16_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, std::int32_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, std::int64_t>, bbi::wrap::i64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, std::uint8_t>, bbi::wrap::u16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, std::uint16_t>, bbi::wrap::u16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, std::uint32_t>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, std::uint64_t>, bbi::wrap::u64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::i8>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::i16>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::u8>, bbi::wrap::u16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::u16>, bbi::wrap::u16>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::u32>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::u64>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::u128>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::u256>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::u512>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u16, bbi::wrap::u1024>, bbi::wrap::u1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, std::int8_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, std::int16_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, std::int32_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, std::int64_t>, bbi::wrap::i64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, std::uint8_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, std::uint16_t>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, std::uint32_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, std::uint64_t>, bbi::wrap::i128>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::i8>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::i16>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::i32>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::u8>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::u16>, bbi::wrap::i32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::u32>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::u64>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::u128>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::u256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i32, bbi::wrap::u512>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, std::int8_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, std::int16_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, std::int32_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, std::int64_t>, bbi::wrap::i64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, std::uint8_t>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, std::uint16_t>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, std::uint32_t>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, std::uint64_t>, bbi::wrap::u64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::i8>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::i16>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::i32>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::u8>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::u16>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::u32>, bbi::wrap::u32>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::u64>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::u128>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::u256>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::u512>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u32, bbi::wrap::u1024>, bbi::wrap::u1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, std::int8_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, std::int16_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, std::int32_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, std::int64_t>, bbi::wrap::i64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, std::uint8_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, std::uint16_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, std::uint32_t>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, std::uint64_t>, bbi::wrap::i128>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::i8>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::i16>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::i32>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::i64>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::u8>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::u16>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::u32>, bbi::wrap::i64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::u64>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::u128>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::u256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i64, bbi::wrap::u512>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, std::int8_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, std::int16_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, std::int32_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, std::int64_t>, bbi::wrap::i128>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, std::uint8_t>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, std::uint16_t>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, std::uint32_t>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, std::uint64_t>, bbi::wrap::u64>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::i8>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::i16>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::i32>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::i64>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::u8>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::u16>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::u32>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::u64>, bbi::wrap::u64>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::u128>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::u256>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::u512>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u64, bbi::wrap::u1024>, bbi::wrap::u1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, std::int8_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, std::int16_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, std::int32_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, std::int64_t>, bbi::wrap::i128>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, std::uint8_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, std::uint16_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, std::uint32_t>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, std::uint64_t>, bbi::wrap::i128>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::i8>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::i16>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::i32>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::i64>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::i128>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::u8>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::u16>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::u32>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::u64>, bbi::wrap::i128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::u128>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::u256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i128, bbi::wrap::u512>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, std::int8_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, std::int16_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, std::int32_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, std::int64_t>, bbi::wrap::i256>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, std::uint8_t>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, std::uint16_t>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, std::uint32_t>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, std::uint64_t>, bbi::wrap::u128>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::i8>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::i16>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::i32>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::i64>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::i128>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::u8>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::u16>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::u32>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::u64>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::u128>, bbi::wrap::u128>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::u256>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::u512>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u128, bbi::wrap::u1024>, bbi::wrap::u1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, std::int8_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, std::int16_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, std::int32_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, std::int64_t>, bbi::wrap::i256>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, std::uint8_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, std::uint16_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, std::uint32_t>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, std::uint64_t>, bbi::wrap::i256>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::i8>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::i16>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::i32>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::i64>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::i128>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::i256>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::u8>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::u16>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::u32>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::u64>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::u128>, bbi::wrap::i256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::u256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i256, bbi::wrap::u512>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, std::int8_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, std::int16_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, std::int32_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, std::int64_t>, bbi::wrap::i512>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, std::uint8_t>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, std::uint16_t>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, std::uint32_t>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, std::uint64_t>, bbi::wrap::u256>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::i8>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::i16>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::i32>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::i64>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::i128>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::i256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::u8>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::u16>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::u32>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::u64>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::u128>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::u256>, bbi::wrap::u256>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::u512>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u256, bbi::wrap::u1024>, bbi::wrap::u1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, std::int8_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, std::int16_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, std::int32_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, std::int64_t>, bbi::wrap::i512>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, std::uint8_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, std::uint16_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, std::uint32_t>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, std::uint64_t>, bbi::wrap::i512>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::i8>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::i16>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::i32>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::i64>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::i128>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::i256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::i512>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::u8>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::u16>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::u32>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::u64>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::u128>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::u256>, bbi::wrap::i512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i512, bbi::wrap::u512>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, std::int8_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, std::int16_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, std::int32_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, std::int64_t>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, std::uint8_t>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, std::uint16_t>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, std::uint32_t>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, std::uint64_t>, bbi::wrap::u512>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::i8>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::i16>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::i32>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::i64>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::i128>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::i256>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::i512>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::u8>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::u16>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::u32>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::u64>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::u128>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::u256>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::u512>, bbi::wrap::u512>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u512, bbi::wrap::u1024>, bbi::wrap::u1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, std::int8_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, std::int16_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, std::int32_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, std::int64_t>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, std::uint8_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, std::uint16_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, std::uint32_t>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, std::uint64_t>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::i8>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::i16>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::i32>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::i64>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::i128>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::i256>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::i512>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::i1024>, bbi::wrap::i1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::u8>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::u16>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::u32>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::u64>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::u128>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::u256>, bbi::wrap::i1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::i1024, bbi::wrap::u512>, bbi::wrap::i1024>);


static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, std::uint8_t>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, std::uint16_t>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, std::uint32_t>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, std::uint64_t>, bbi::wrap::u1024>);

static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, bbi::wrap::u8>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, bbi::wrap::u16>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, bbi::wrap::u32>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, bbi::wrap::u64>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, bbi::wrap::u128>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, bbi::wrap::u256>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, bbi::wrap::u512>, bbi::wrap::u1024>);
static_assert(std::is_same_v<std::common_type_t<bbi::wrap::u1024, bbi::wrap::u1024>, bbi::wrap::u1024>);

// is_convertible

static_assert(!std::is_convertible_v<bbi::term::i8, bool>);
static_assert(!std::is_convertible_v<bbi::term::i8, char>);
static_assert(!std::is_convertible_v<bbi::term::i8, char8_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, char16_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, char32_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, wchar_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::term::i8, std::uint64_t>);
static_assert( std::is_convertible_v<bbi::term::i8, bbi::term::i8>);
static_assert( std::is_convertible_v<bbi::term::i8, bbi::term::i16>);
static_assert( std::is_convertible_v<bbi::term::i8, bbi::term::i32>);
static_assert( std::is_convertible_v<bbi::term::i8, bbi::term::i64>);
static_assert( std::is_convertible_v<bbi::term::i8, bbi::term::i128>);
static_assert( std::is_convertible_v<bbi::term::i8, bbi::term::i256>);
static_assert( std::is_convertible_v<bbi::term::i8, bbi::term::i512>);
static_assert( std::is_convertible_v<bbi::term::i8, bbi::term::i1024>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::term::u8>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::term::u16>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::term::u32>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::term::u64>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::term::u128>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::term::u256>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::term::u512>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::term::u1024>);
static_assert(!std::is_convertible_v<bbi::term::i8, bbi::sat::i8>);

static_assert(!std::is_convertible_v<bbi::sat::i16, bool>);
static_assert(!std::is_convertible_v<bbi::sat::i16, char>);
static_assert(!std::is_convertible_v<bbi::sat::i16, char8_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, char16_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, char32_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, wchar_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::i8>);
static_assert( std::is_convertible_v<bbi::sat::i16, bbi::sat::i16>);
static_assert( std::is_convertible_v<bbi::sat::i16, bbi::sat::i32>);
static_assert( std::is_convertible_v<bbi::sat::i16, bbi::sat::i64>);
static_assert( std::is_convertible_v<bbi::sat::i16, bbi::sat::i128>);
static_assert( std::is_convertible_v<bbi::sat::i16, bbi::sat::i256>);
static_assert( std::is_convertible_v<bbi::sat::i16, bbi::sat::i512>);
static_assert( std::is_convertible_v<bbi::sat::i16, bbi::sat::i1024>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::u8>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::u16>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::u32>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::u64>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::u128>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::u256>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::u512>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::sat::u1024>);
static_assert(!std::is_convertible_v<bbi::sat::i16, bbi::thrw::i16>);

static_assert(!std::is_convertible_v<bbi::wrap::i32, bool>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, char>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, char8_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, char16_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, char32_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, wchar_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::i8>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::i16>);
static_assert( std::is_convertible_v<bbi::wrap::i32, bbi::wrap::i32>);
static_assert( std::is_convertible_v<bbi::wrap::i32, bbi::wrap::i64>);
static_assert( std::is_convertible_v<bbi::wrap::i32, bbi::wrap::i128>);
static_assert( std::is_convertible_v<bbi::wrap::i32, bbi::wrap::i256>);
static_assert( std::is_convertible_v<bbi::wrap::i32, bbi::wrap::i512>);
static_assert( std::is_convertible_v<bbi::wrap::i32, bbi::wrap::i1024>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::u8>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::u16>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::u32>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::u64>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::u128>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::u256>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::u512>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::wrap::u1024>);
static_assert(!std::is_convertible_v<bbi::wrap::i32, bbi::thrw::i32>);

static_assert(!std::is_convertible_v<bbi::thrw::i64, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, char>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::i32>);
static_assert( std::is_convertible_v<bbi::thrw::i64, bbi::thrw::i64>);
static_assert( std::is_convertible_v<bbi::thrw::i64, bbi::thrw::i128>);
static_assert( std::is_convertible_v<bbi::thrw::i64, bbi::thrw::i256>);
static_assert( std::is_convertible_v<bbi::thrw::i64, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::i64, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::u64>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::u128>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::u256>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::u512>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i64, bbi::sat::i64>);

static_assert(!std::is_convertible_v<bbi::thrw::i128, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, char>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::i64>);
static_assert( std::is_convertible_v<bbi::thrw::i128, bbi::thrw::i128>);
static_assert( std::is_convertible_v<bbi::thrw::i128, bbi::thrw::i256>);
static_assert( std::is_convertible_v<bbi::thrw::i128, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::i128, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::u64>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::u128>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::u256>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::u512>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i128, bbi::sat::i128>);

static_assert(!std::is_convertible_v<bbi::thrw::i256, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, char>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::i64>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::i128>);
static_assert( std::is_convertible_v<bbi::thrw::i256, bbi::thrw::i256>);
static_assert( std::is_convertible_v<bbi::thrw::i256, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::i256, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::u64>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::u128>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::u256>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::u512>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i256, bbi::sat::i256>);

static_assert(!std::is_convertible_v<bbi::thrw::i512, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, char>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::i64>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::i128>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::i256>);
static_assert( std::is_convertible_v<bbi::thrw::i512, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::i512, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::u64>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::u128>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::u256>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::u512>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i512, bbi::sat::i512>);

static_assert(!std::is_convertible_v<bbi::thrw::i1024, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, char>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::i64>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::i128>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::i256>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::u64>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::u128>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::u256>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::u512>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::i1024, bbi::sat::i1024>);


static_assert(!std::is_convertible_v<bbi::term::u8, bool>);
static_assert(!std::is_convertible_v<bbi::term::u8, char>);
static_assert(!std::is_convertible_v<bbi::term::u8, char8_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, char16_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, char32_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, wchar_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::term::u8, bbi::term::i8>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::i16>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::i32>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::i64>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::i128>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::i256>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::i512>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::i1024>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::u8>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::u16>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::u32>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::u64>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::u128>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::u256>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::u512>);
static_assert( std::is_convertible_v<bbi::term::u8, bbi::term::u1024>);
static_assert(!std::is_convertible_v<bbi::term::u8, bbi::sat::u8>);

static_assert(!std::is_convertible_v<bbi::sat::u16, bool>);
static_assert(!std::is_convertible_v<bbi::sat::u16, char>);
static_assert(!std::is_convertible_v<bbi::sat::u16, char8_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, char16_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, char32_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, wchar_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::sat::u16, bbi::sat::i8>);
static_assert(!std::is_convertible_v<bbi::sat::u16, bbi::sat::i16>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::i32>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::i64>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::i128>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::i256>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::i512>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::i1024>);
static_assert(!std::is_convertible_v<bbi::sat::u16, bbi::sat::u8>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::u16>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::u32>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::u64>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::u128>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::u256>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::u512>);
static_assert( std::is_convertible_v<bbi::sat::u16, bbi::sat::u1024>);
static_assert(!std::is_convertible_v<bbi::sat::u16, bbi::thrw::u16>);

static_assert(!std::is_convertible_v<bbi::wrap::u32, bool>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, char>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, char8_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, char16_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, char32_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, wchar_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, bbi::wrap::i8>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, bbi::wrap::i16>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, bbi::wrap::i32>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::i64>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::i128>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::i256>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::i512>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::i1024>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, bbi::wrap::u8>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, bbi::wrap::u16>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::u32>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::u64>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::u128>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::u256>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::u512>);
static_assert( std::is_convertible_v<bbi::wrap::u32, bbi::wrap::u1024>);
static_assert(!std::is_convertible_v<bbi::wrap::u32, bbi::thrw::u32>);

static_assert(!std::is_convertible_v<bbi::thrw::u64, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, char>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, bbi::thrw::i64>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::i128>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::i256>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, bbi::thrw::u32>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::u64>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::u128>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::u256>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::u512>);
static_assert( std::is_convertible_v<bbi::thrw::u64, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u64, bbi::sat::u64>);

static_assert(!std::is_convertible_v<bbi::thrw::u128, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, char>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::i64>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::i128>);
static_assert( std::is_convertible_v<bbi::thrw::u128, bbi::thrw::i256>);
static_assert( std::is_convertible_v<bbi::thrw::u128, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::u128, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::thrw::u64>);
static_assert( std::is_convertible_v<bbi::thrw::u128, bbi::thrw::u128>);
static_assert( std::is_convertible_v<bbi::thrw::u128, bbi::thrw::u256>);
static_assert( std::is_convertible_v<bbi::thrw::u128, bbi::thrw::u512>);
static_assert( std::is_convertible_v<bbi::thrw::u128, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u128, bbi::sat::u128>);

static_assert(!std::is_convertible_v<bbi::thrw::u256, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, char>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::i64>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::i128>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::i256>);
static_assert( std::is_convertible_v<bbi::thrw::u256, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::u256, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::u64>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::thrw::u128>);
static_assert( std::is_convertible_v<bbi::thrw::u256, bbi::thrw::u256>);
static_assert( std::is_convertible_v<bbi::thrw::u256, bbi::thrw::u512>);
static_assert( std::is_convertible_v<bbi::thrw::u256, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u256, bbi::sat::u256>);

static_assert(!std::is_convertible_v<bbi::thrw::u512, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, char>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::i64>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::i128>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::i256>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::i512>);
static_assert( std::is_convertible_v<bbi::thrw::u512, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::u64>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::u128>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::thrw::u256>);
static_assert( std::is_convertible_v<bbi::thrw::u512, bbi::thrw::u512>);
static_assert( std::is_convertible_v<bbi::thrw::u512, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u512, bbi::sat::u512>);

static_assert(!std::is_convertible_v<bbi::thrw::u1024, bool>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, char>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, char8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, char16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, char32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, wchar_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, std::int8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, std::int16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, std::int32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, std::int64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, std::uint8_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, std::uint16_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, std::uint32_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, std::uint64_t>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::i8>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::i16>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::i32>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::i64>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::i128>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::i256>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::i512>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::i1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::u8>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::u16>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::u32>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::u64>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::u128>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::u256>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::u512>);
static_assert( std::is_convertible_v<bbi::thrw::u1024, bbi::thrw::u1024>);
static_assert(!std::is_convertible_v<bbi::thrw::u1024, bbi::sat::u1024>);


static_assert(!std::is_convertible_v<bool,          bbi::term::i8>);
static_assert(!std::is_convertible_v<char,          bbi::term::i8>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::i8>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::i8>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::i8>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::i8>);
static_assert( std::is_convertible_v<std::int8_t,   bbi::term::i8>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::i8>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::i8>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::i8>);
static_assert(!std::is_convertible_v<std::uint8_t,  bbi::term::i8>);
static_assert(!std::is_convertible_v<std::uint16_t, bbi::term::i8>);
static_assert(!std::is_convertible_v<std::uint32_t, bbi::term::i8>);
static_assert(!std::is_convertible_v<std::uint64_t, bbi::term::i8>);

static_assert(!std::is_convertible_v<bool,          bbi::term::i16>);
static_assert(!std::is_convertible_v<char,          bbi::term::i16>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::i16>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::i16>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::i16>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::i16>);
static_assert( std::is_convertible_v<std::int8_t,   bbi::term::i16>);
static_assert( std::is_convertible_v<std::int16_t,  bbi::term::i16>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::i16>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::i16>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::i16>);
static_assert(!std::is_convertible_v<std::uint16_t, bbi::term::i16>);
static_assert(!std::is_convertible_v<std::uint32_t, bbi::term::i16>);
static_assert(!std::is_convertible_v<std::uint64_t, bbi::term::i16>);

static_assert(!std::is_convertible_v<bool,          bbi::term::i32>);
static_assert(!std::is_convertible_v<char,          bbi::term::i32>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::i32>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::i32>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::i32>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::i32>);
static_assert( std::is_convertible_v<std::int8_t,   bbi::term::i32>);
static_assert( std::is_convertible_v<std::int16_t,  bbi::term::i32>);
static_assert( std::is_convertible_v<std::int32_t,  bbi::term::i32>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::i32>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::i32>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::i32>);
static_assert(!std::is_convertible_v<std::uint32_t, bbi::term::i32>);
static_assert(!std::is_convertible_v<std::uint64_t, bbi::term::i32>);

static_assert(!std::is_convertible_v<bool,          bbi::term::i64>);
static_assert(!std::is_convertible_v<char,          bbi::term::i64>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::i64>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::i64>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::i64>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::i64>);
static_assert( std::is_convertible_v<std::int8_t,   bbi::term::i64>);
static_assert( std::is_convertible_v<std::int16_t,  bbi::term::i64>);
static_assert( std::is_convertible_v<std::int32_t,  bbi::term::i64>);
static_assert( std::is_convertible_v<std::int64_t,  bbi::term::i64>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::i64>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::i64>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::i64>);
static_assert(!std::is_convertible_v<std::uint64_t, bbi::term::i64>);

static_assert(!std::is_convertible_v<bool,          bbi::term::i128>);
static_assert(!std::is_convertible_v<char,          bbi::term::i128>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::i128>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::i128>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::i128>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::i128>);
static_assert( std::is_convertible_v<std::int8_t,   bbi::term::i128>);
static_assert( std::is_convertible_v<std::int16_t,  bbi::term::i128>);
static_assert( std::is_convertible_v<std::int32_t,  bbi::term::i128>);
static_assert( std::is_convertible_v<std::int64_t,  bbi::term::i128>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::i128>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::i128>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::i128>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::i128>);

static_assert(!std::is_convertible_v<bool,          bbi::term::i256>);
static_assert(!std::is_convertible_v<char,          bbi::term::i256>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::i256>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::i256>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::i256>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::i256>);
static_assert( std::is_convertible_v<std::int8_t,   bbi::term::i256>);
static_assert( std::is_convertible_v<std::int16_t,  bbi::term::i256>);
static_assert( std::is_convertible_v<std::int32_t,  bbi::term::i256>);
static_assert( std::is_convertible_v<std::int64_t,  bbi::term::i256>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::i256>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::i256>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::i256>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::i256>);

static_assert(!std::is_convertible_v<bool,          bbi::term::i512>);
static_assert(!std::is_convertible_v<char,          bbi::term::i512>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::i512>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::i512>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::i512>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::i512>);
static_assert( std::is_convertible_v<std::int8_t,   bbi::term::i512>);
static_assert( std::is_convertible_v<std::int16_t,  bbi::term::i512>);
static_assert( std::is_convertible_v<std::int32_t,  bbi::term::i512>);
static_assert( std::is_convertible_v<std::int64_t,  bbi::term::i512>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::i512>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::i512>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::i512>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::i512>);

static_assert(!std::is_convertible_v<bool,          bbi::term::i1024>);
static_assert(!std::is_convertible_v<char,          bbi::term::i1024>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::i1024>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::i1024>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::i1024>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::i1024>);
static_assert( std::is_convertible_v<std::int8_t,   bbi::term::i1024>);
static_assert( std::is_convertible_v<std::int16_t,  bbi::term::i1024>);
static_assert( std::is_convertible_v<std::int32_t,  bbi::term::i1024>);
static_assert( std::is_convertible_v<std::int64_t,  bbi::term::i1024>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::i1024>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::i1024>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::i1024>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::i1024>);


static_assert(!std::is_convertible_v<bool,          bbi::term::u8>);
static_assert(!std::is_convertible_v<char,          bbi::term::u8>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::u8>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::u8>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::u8>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::u8>);
static_assert(!std::is_convertible_v<std::int8_t,   bbi::term::u8>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::u8>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::u8>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::u8>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::u8>);
static_assert(!std::is_convertible_v<std::uint16_t, bbi::term::u8>);
static_assert(!std::is_convertible_v<std::uint32_t, bbi::term::u8>);
static_assert(!std::is_convertible_v<std::uint64_t, bbi::term::u8>);

static_assert(!std::is_convertible_v<bool,          bbi::term::u16>);
static_assert(!std::is_convertible_v<char,          bbi::term::u16>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::u16>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::u16>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::u16>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::u16>);
static_assert(!std::is_convertible_v<std::int8_t,   bbi::term::u16>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::u16>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::u16>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::u16>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::u16>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::u16>);
static_assert(!std::is_convertible_v<std::uint32_t, bbi::term::u16>);
static_assert(!std::is_convertible_v<std::uint64_t, bbi::term::u16>);

static_assert(!std::is_convertible_v<bool,          bbi::term::u32>);
static_assert(!std::is_convertible_v<char,          bbi::term::u32>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::u32>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::u32>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::u32>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::u32>);
static_assert(!std::is_convertible_v<std::int8_t,   bbi::term::u32>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::u32>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::u32>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::u32>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::u32>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::u32>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::u32>);
static_assert(!std::is_convertible_v<std::uint64_t, bbi::term::u32>);

static_assert(!std::is_convertible_v<bool,          bbi::term::u64>);
static_assert(!std::is_convertible_v<char,          bbi::term::u64>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::u64>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::u64>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::u64>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::u64>);
static_assert(!std::is_convertible_v<std::int8_t,   bbi::term::u64>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::u64>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::u64>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::u64>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::u64>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::u64>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::u64>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::u64>);

static_assert(!std::is_convertible_v<bool,          bbi::term::u128>);
static_assert(!std::is_convertible_v<char,          bbi::term::u128>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::u128>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::u128>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::u128>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::u128>);
static_assert(!std::is_convertible_v<std::int8_t,   bbi::term::u128>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::u128>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::u128>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::u128>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::u128>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::u128>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::u128>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::u128>);

static_assert(!std::is_convertible_v<bool,          bbi::term::u256>);
static_assert(!std::is_convertible_v<char,          bbi::term::u256>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::u256>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::u256>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::u256>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::u256>);
static_assert(!std::is_convertible_v<std::int8_t,   bbi::term::u256>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::u256>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::u256>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::u256>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::u256>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::u256>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::u256>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::u256>);

static_assert(!std::is_convertible_v<bool,          bbi::term::u512>);
static_assert(!std::is_convertible_v<char,          bbi::term::u512>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::u512>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::u512>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::u512>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::u512>);
static_assert(!std::is_convertible_v<std::int8_t,   bbi::term::u512>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::u512>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::u512>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::u512>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::u512>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::u512>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::u512>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::u512>);

static_assert(!std::is_convertible_v<bool,          bbi::term::u1024>);
static_assert(!std::is_convertible_v<char,          bbi::term::u1024>);
static_assert(!std::is_convertible_v<char8_t,       bbi::term::u1024>);
static_assert(!std::is_convertible_v<char16_t,      bbi::term::u1024>);
static_assert(!std::is_convertible_v<char32_t,      bbi::term::u1024>);
static_assert(!std::is_convertible_v<wchar_t,       bbi::term::u1024>);
static_assert(!std::is_convertible_v<std::int8_t,   bbi::term::u1024>);
static_assert(!std::is_convertible_v<std::int16_t,  bbi::term::u1024>);
static_assert(!std::is_convertible_v<std::int32_t,  bbi::term::u1024>);
static_assert(!std::is_convertible_v<std::int64_t,  bbi::term::u1024>);
static_assert( std::is_convertible_v<std::uint8_t,  bbi::term::u1024>);
static_assert( std::is_convertible_v<std::uint16_t, bbi::term::u1024>);
static_assert( std::is_convertible_v<std::uint32_t, bbi::term::u1024>);
static_assert( std::is_convertible_v<std::uint64_t, bbi::term::u1024>);

// is_constructible

static_assert( std::is_constructible_v<bbi::term::i8, bool>);
static_assert( std::is_constructible_v<bbi::term::i8, char>);
static_assert( std::is_constructible_v<bbi::term::i8, char8_t>);
static_assert( std::is_constructible_v<bbi::term::i8, char16_t>);
static_assert( std::is_constructible_v<bbi::term::i8, char32_t>);
static_assert( std::is_constructible_v<bbi::term::i8, wchar_t>);
static_assert( std::is_constructible_v<bbi::term::i8, std::int8_t>);
static_assert( std::is_constructible_v<bbi::term::i8, std::int16_t>);
static_assert( std::is_constructible_v<bbi::term::i8, std::int32_t>);
static_assert( std::is_constructible_v<bbi::term::i8, std::int64_t>);
static_assert( std::is_constructible_v<bbi::term::i8, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::term::i8, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::term::i8, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::term::i8, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::i8>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::i16>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::i32>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::i64>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::i128>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::i256>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::i512>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::i1024>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::u8>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::u16>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::u32>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::u64>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::u128>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::u256>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::u512>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::term::u1024>);
static_assert( std::is_constructible_v<bbi::term::i8, bbi::sat::i8>);

static_assert( std::is_constructible_v<bbi::sat::i16, bool>);
static_assert( std::is_constructible_v<bbi::sat::i16, char>);
static_assert( std::is_constructible_v<bbi::sat::i16, char8_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, char16_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, char32_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, wchar_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, std::int8_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, std::int16_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, std::int32_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, std::int64_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::i8>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::i16>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::i32>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::i64>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::i128>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::i256>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::i512>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::i1024>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::u8>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::u16>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::u32>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::u64>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::u128>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::u256>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::u512>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::sat::u1024>);
static_assert( std::is_constructible_v<bbi::sat::i16, bbi::thrw::i16>);

static_assert( std::is_constructible_v<bbi::wrap::i32, bool>);
static_assert( std::is_constructible_v<bbi::wrap::i32, char>);
static_assert( std::is_constructible_v<bbi::wrap::i32, char8_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, char16_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, char32_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, wchar_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, std::int8_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, std::int16_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, std::int32_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, std::int64_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::i8>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::i16>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::i32>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::i64>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::i128>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::i256>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::i512>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::i1024>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::u8>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::u16>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::u32>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::u64>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::u128>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::u256>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::u512>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::wrap::u1024>);
static_assert( std::is_constructible_v<bbi::wrap::i32, bbi::thrw::i32>);

static_assert( std::is_constructible_v<bbi::thrw::i64, bool>);
static_assert( std::is_constructible_v<bbi::thrw::i64, char>);
static_assert( std::is_constructible_v<bbi::thrw::i64, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::i64, bbi::sat::i64>);

static_assert( std::is_constructible_v<bbi::thrw::i128, bool>);
static_assert( std::is_constructible_v<bbi::thrw::i128, char>);
static_assert( std::is_constructible_v<bbi::thrw::i128, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::i128, bbi::sat::i128>);

static_assert( std::is_constructible_v<bbi::thrw::i256, bool>);
static_assert( std::is_constructible_v<bbi::thrw::i256, char>);
static_assert( std::is_constructible_v<bbi::thrw::i256, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::i256, bbi::sat::i256>);

static_assert( std::is_constructible_v<bbi::thrw::i512, bool>);
static_assert( std::is_constructible_v<bbi::thrw::i512, char>);
static_assert( std::is_constructible_v<bbi::thrw::i512, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::i512, bbi::sat::i512>);

static_assert( std::is_constructible_v<bbi::thrw::i1024, bool>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, char>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::i1024, bbi::sat::i1024>);


static_assert( std::is_constructible_v<bbi::term::u8, bool>);
static_assert( std::is_constructible_v<bbi::term::u8, char>);
static_assert( std::is_constructible_v<bbi::term::u8, char8_t>);
static_assert( std::is_constructible_v<bbi::term::u8, char16_t>);
static_assert( std::is_constructible_v<bbi::term::u8, char32_t>);
static_assert( std::is_constructible_v<bbi::term::u8, wchar_t>);
static_assert( std::is_constructible_v<bbi::term::u8, std::int8_t>);
static_assert( std::is_constructible_v<bbi::term::u8, std::int16_t>);
static_assert( std::is_constructible_v<bbi::term::u8, std::int32_t>);
static_assert( std::is_constructible_v<bbi::term::u8, std::int64_t>);
static_assert( std::is_constructible_v<bbi::term::u8, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::term::u8, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::term::u8, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::term::u8, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::i8>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::i16>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::i32>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::i64>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::i128>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::i256>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::i512>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::i1024>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::u8>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::u16>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::u32>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::u64>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::u128>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::u256>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::u512>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::term::u1024>);
static_assert( std::is_constructible_v<bbi::term::u8, bbi::sat::u8>);

static_assert( std::is_constructible_v<bbi::sat::u16, bool>);
static_assert( std::is_constructible_v<bbi::sat::u16, char>);
static_assert( std::is_constructible_v<bbi::sat::u16, char8_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, char16_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, char32_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, wchar_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, std::int8_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, std::int16_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, std::int32_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, std::int64_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::i8>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::i16>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::i32>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::i64>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::i128>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::i256>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::i512>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::i1024>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::u8>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::u16>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::u32>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::u64>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::u128>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::u256>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::u512>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::sat::u1024>);
static_assert( std::is_constructible_v<bbi::sat::u16, bbi::thrw::u16>);

static_assert( std::is_constructible_v<bbi::wrap::u32, bool>);
static_assert( std::is_constructible_v<bbi::wrap::u32, char>);
static_assert( std::is_constructible_v<bbi::wrap::u32, char8_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, char16_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, char32_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, wchar_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, std::int8_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, std::int16_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, std::int32_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, std::int64_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::i8>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::i16>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::i32>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::i64>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::i128>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::i256>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::i512>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::i1024>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::u8>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::u16>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::u32>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::u64>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::u128>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::u256>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::u512>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::wrap::u1024>);
static_assert( std::is_constructible_v<bbi::wrap::u32, bbi::thrw::u32>);

static_assert( std::is_constructible_v<bbi::thrw::u64, bool>);
static_assert( std::is_constructible_v<bbi::thrw::u64, char>);
static_assert( std::is_constructible_v<bbi::thrw::u64, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::u64, bbi::sat::u64>);

static_assert( std::is_constructible_v<bbi::thrw::u128, bool>);
static_assert( std::is_constructible_v<bbi::thrw::u128, char>);
static_assert( std::is_constructible_v<bbi::thrw::u128, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::u128, bbi::sat::u128>);

static_assert( std::is_constructible_v<bbi::thrw::u256, bool>);
static_assert( std::is_constructible_v<bbi::thrw::u256, char>);
static_assert( std::is_constructible_v<bbi::thrw::u256, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::u256, bbi::sat::u256>);

static_assert( std::is_constructible_v<bbi::thrw::u512, bool>);
static_assert( std::is_constructible_v<bbi::thrw::u512, char>);
static_assert( std::is_constructible_v<bbi::thrw::u512, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::u512, bbi::sat::u512>);

static_assert( std::is_constructible_v<bbi::thrw::u1024, bool>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, char>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, char8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, char16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, char32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, wchar_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, std::int8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, std::int16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, std::int32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, std::int64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, std::uint8_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, std::uint16_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, std::uint32_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, std::uint64_t>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::i8>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::i16>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::i32>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::i64>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::i128>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::i256>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::i512>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::i1024>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::u8>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::u16>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::u32>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::u64>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::u128>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::u256>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::u512>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::thrw::u1024>);
static_assert( std::is_constructible_v<bbi::thrw::u1024, bbi::sat::u1024>);


static_assert( std::is_constructible_v<bool,          bbi::term::i8>);
static_assert( std::is_constructible_v<char,          bbi::term::i8>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::i8>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::i8>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::i8>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::i8>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::i8>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::i8>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::i8>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::i8>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::i8>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::i8>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::i8>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::i8>);

static_assert( std::is_constructible_v<bool,          bbi::term::i16>);
static_assert( std::is_constructible_v<char,          bbi::term::i16>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::i16>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::i16>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::i16>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::i16>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::i16>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::i16>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::i16>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::i16>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::i16>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::i16>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::i16>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::i16>);

static_assert( std::is_constructible_v<bool,          bbi::term::i32>);
static_assert( std::is_constructible_v<char,          bbi::term::i32>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::i32>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::i32>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::i32>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::i32>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::i32>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::i32>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::i32>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::i32>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::i32>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::i32>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::i32>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::i32>);

static_assert( std::is_constructible_v<bool,          bbi::term::i64>);
static_assert( std::is_constructible_v<char,          bbi::term::i64>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::i64>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::i64>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::i64>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::i64>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::i64>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::i64>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::i64>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::i64>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::i64>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::i64>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::i64>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::i64>);

static_assert( std::is_constructible_v<bool,          bbi::term::i128>);
static_assert( std::is_constructible_v<char,          bbi::term::i128>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::i128>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::i128>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::i128>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::i128>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::i128>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::i128>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::i128>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::i128>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::i128>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::i128>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::i128>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::i128>);

static_assert( std::is_constructible_v<bool,          bbi::term::i256>);
static_assert( std::is_constructible_v<char,          bbi::term::i256>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::i256>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::i256>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::i256>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::i256>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::i256>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::i256>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::i256>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::i256>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::i256>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::i256>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::i256>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::i256>);

static_assert( std::is_constructible_v<bool,          bbi::term::i512>);
static_assert( std::is_constructible_v<char,          bbi::term::i512>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::i512>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::i512>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::i512>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::i512>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::i512>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::i512>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::i512>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::i512>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::i512>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::i512>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::i512>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::i512>);

static_assert( std::is_constructible_v<bool,          bbi::term::i1024>);
static_assert( std::is_constructible_v<char,          bbi::term::i1024>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::i1024>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::i1024>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::i1024>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::i1024>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::i1024>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::i1024>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::i1024>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::i1024>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::i1024>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::i1024>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::i1024>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::i1024>);


static_assert( std::is_constructible_v<bool,          bbi::term::u8>);
static_assert( std::is_constructible_v<char,          bbi::term::u8>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::u8>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::u8>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::u8>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::u8>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::u8>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::u8>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::u8>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::u8>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::u8>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::u8>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::u8>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::u8>);

static_assert( std::is_constructible_v<bool,          bbi::term::u16>);
static_assert( std::is_constructible_v<char,          bbi::term::u16>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::u16>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::u16>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::u16>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::u16>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::u16>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::u16>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::u16>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::u16>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::u16>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::u16>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::u16>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::u16>);

static_assert( std::is_constructible_v<bool,          bbi::term::u32>);
static_assert( std::is_constructible_v<char,          bbi::term::u32>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::u32>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::u32>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::u32>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::u32>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::u32>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::u32>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::u32>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::u32>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::u32>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::u32>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::u32>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::u32>);

static_assert( std::is_constructible_v<bool,          bbi::term::u64>);
static_assert( std::is_constructible_v<char,          bbi::term::u64>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::u64>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::u64>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::u64>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::u64>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::u64>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::u64>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::u64>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::u64>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::u64>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::u64>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::u64>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::u64>);

static_assert( std::is_constructible_v<bool,          bbi::term::u128>);
static_assert( std::is_constructible_v<char,          bbi::term::u128>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::u128>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::u128>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::u128>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::u128>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::u128>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::u128>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::u128>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::u128>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::u128>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::u128>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::u128>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::u128>);

static_assert( std::is_constructible_v<bool,          bbi::term::u256>);
static_assert( std::is_constructible_v<char,          bbi::term::u256>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::u256>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::u256>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::u256>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::u256>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::u256>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::u256>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::u256>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::u256>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::u256>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::u256>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::u256>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::u256>);

static_assert( std::is_constructible_v<bool,          bbi::term::u512>);
static_assert( std::is_constructible_v<char,          bbi::term::u512>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::u512>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::u512>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::u512>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::u512>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::u512>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::u512>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::u512>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::u512>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::u512>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::u512>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::u512>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::u512>);

static_assert( std::is_constructible_v<bool,          bbi::term::u1024>);
static_assert( std::is_constructible_v<char,          bbi::term::u1024>);
static_assert( std::is_constructible_v<char8_t,       bbi::term::u1024>);
static_assert( std::is_constructible_v<char16_t,      bbi::term::u1024>);
static_assert( std::is_constructible_v<char32_t,      bbi::term::u1024>);
static_assert( std::is_constructible_v<wchar_t,       bbi::term::u1024>);
static_assert( std::is_constructible_v<std::int8_t,   bbi::term::u1024>);
static_assert( std::is_constructible_v<std::int16_t,  bbi::term::u1024>);
static_assert( std::is_constructible_v<std::int32_t,  bbi::term::u1024>);
static_assert( std::is_constructible_v<std::int64_t,  bbi::term::u1024>);
static_assert( std::is_constructible_v<std::uint8_t,  bbi::term::u1024>);
static_assert( std::is_constructible_v<std::uint16_t, bbi::term::u1024>);
static_assert( std::is_constructible_v<std::uint32_t, bbi::term::u1024>);
static_assert( std::is_constructible_v<std::uint64_t, bbi::term::u1024>);

template <bbi::SignTag S, unsigned N> struct StandardInt;
template <> struct StandardInt<bbi::Signed,    8> {using type = std::int8_t;};
template <> struct StandardInt<bbi::Signed,   16> {using type = std::int16_t;};
template <> struct StandardInt<bbi::Signed,   32> {using type = std::int32_t;};
template <> struct StandardInt<bbi::Signed,   64> {using type = std::int64_t;};
template <> struct StandardInt<bbi::Unsigned,  8> {using type = std::uint8_t;};
template <> struct StandardInt<bbi::Unsigned, 16> {using type = std::uint16_t;};
template <> struct StandardInt<bbi::Unsigned, 32> {using type = std::uint32_t;};
template <> struct StandardInt<bbi::Unsigned, 64> {using type = std::uint64_t;};

template <bbi::SignTag S, unsigned N>
using StandardInt_t = StandardInt<S, N>::type;

template <unsigned N, bbi::Policy P>
constexpr
auto
make_unsigned_range() noexcept
{
    using U = bbi::Z<bbi::Unsigned, N, P>;
    using PR = std::array<U, 2>;
    auto constexpr M = std::numeric_limits<U>::max();
    if constexpr (N == 8)
        return std::array<PR, 1>{PR{U{0}, M}};
    else
        return std::array<PR, 3>{PR{U{0}, U{0x20}}, PR{(M>>1)-U{0x10}, (M>>1)+U{0x10}},
                                 PR{M-U{0x10}, M}};
}

template <unsigned N, bbi::Policy P>
constexpr
auto
make_signed_range() noexcept
{
    using S = bbi::Z<bbi::Signed, N, P>;
    using PR = std::array<S, 2>;
    auto constexpr m = std::numeric_limits<S>::min();
    auto constexpr M = std::numeric_limits<S>::max();
    if constexpr (N == 8)
        return std::array<PR, 1>{PR{m, M}};
    else
        return std::array<PR, 3>{PR{m, m+S{0x20}}, PR{S{-0x10}, S{0x10}},
                                 PR{M-S{0x20}, M}};
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
auto
make_range() noexcept
{
    if constexpr (S{} == bbi::Signed{})
        return make_signed_range<N, P>();
    else
        return make_unsigned_range<N, P>();
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_bit_not()
{
    using X = bbi::Z<S, N, P>;
    using I = StandardInt_t<S, N>;
    auto constexpr rngx = make_range<S, N, P>();
    for (auto ix = 0u; ix < rngx.size(); ++ix)
    {
        auto r = rngx[ix][0];
        while (true)
        {
            X x{r};
            I i{r};
            auto x1 = ~x;
            I i1 = ~i;
            if (x1 != i1)
            {
                std::cout << "~" << bbi::to_string<X>() << "{" << x << "} is " << x1
                          << " but should be " << i1+0 << '\n';
                return;
            }
            if (r == rngx[ix][1])
                break;
            ++r;
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_bit_and()
{
    using Z = bbi::Z<S, N, P>;
    using I = StandardInt_t<S, N>;
    auto constexpr rng = make_range<S, N, P>();
    for (auto rx = 0u; rx < rng.size(); ++rx)
    {
        auto i = rng[rx][0];
        while (true)
        {
            for (auto ry = 0u; ry < rng.size(); ++ry)
            {
                auto j = rng[ry][0];
                while (true)
                {
                    Z zx{i};
                    Z zy{j};
                    I ix{i};
                    I iy{j};
                    auto z1 = zx & zy;
                    I i1 = ix & iy;
                    if (z1 != i1)
                    {
                        std::cout << bbi::to_string<Z>() << "{" << zx << "} & "
                                  << bbi::to_string<Z>() << "{" << zy
                                  << "} is " << z1
                                  << " but should be " << i1+0 << '\n';
                        return;
                    }
                    if (j == rng[ry][1])
                        break;
                    ++j;
                }
            }
            if (i == rng[rx][1])
                break;
            ++i;
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_bit_or()
{
    using Z = bbi::Z<S, N, P>;
    using I = StandardInt_t<S, N>;
    auto constexpr rng = make_range<S, N, P>();
    for (auto rx = 0u; rx < rng.size(); ++rx)
    {
        auto i = rng[rx][0];
        while (true)
        {
            for (auto ry = 0u; ry < rng.size(); ++ry)
            {
                auto j = rng[ry][0];
                while (true)
                {
                    Z zx{i};
                    Z zy{j};
                    I ix{i};
                    I iy{j};
                    auto z1 = zx | zy;
                    I i1 = ix | iy;
                    if (z1 != i1)
                    {
                        std::cout << bbi::to_string<Z>() << "{" << zx << "} & "
                                  << bbi::to_string<Z>() << "{" << zy
                                  << "} is " << z1
                                  << " but should be " << i1+0 << '\n';
                        return;
                    }
                    if (j == rng[ry][1])
                        break;
                    ++j;
                }
            }
            if (i == rng[rx][1])
                break;
            ++i;
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_bit_xor()
{
    using Z = bbi::Z<S, N, P>;
    using I = StandardInt_t<S, N>;
    auto constexpr rng = make_range<S, N, P>();
    for (auto rx = 0u; rx < rng.size(); ++rx)
    {
        auto i = rng[rx][0];
        while (true)
        {
            for (auto ry = 0u; ry < rng.size(); ++ry)
            {
                auto j = rng[ry][0];
                while (true)
                {
                    Z zx{i};
                    Z zy{j};
                    I ix{i};
                    I iy{j};
                    auto z1 = zx ^ zy;
                    I i1 = ix ^ iy;
                    if (z1 != i1)
                    {
                        std::cout << bbi::to_string<Z>() << "{" << zx << "} & "
                                  << bbi::to_string<Z>() << "{" << zy
                                  << "} is " << z1
                                  << " but should be " << i1+0 << '\n';
                        return;
                    }
                    if (j == rng[ry][1])
                        break;
                    ++j;
                }
            }
            if (i == rng[rx][1])
                break;
            ++i;
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_leftshift()
{
    using Z = bbi::Z<S, N, P>;
    using I = int;
    if constexpr (P{} == bbi::Wrap{})
    {
        auto constexpr rngx = make_range<S, N, P>();
        for (auto rx = 0u; rx < rngx.size(); ++rx)
        {
            auto i = rngx[rx][0];
            while (true)
            {
                auto constexpr rngy = make_range<bbi::Signed, 32, bbi::Wrap>();
                for (auto ry = 0u; ry < rngy.size(); ++ry)
                {
                    I j{rngy[ry][0]};
                    while (true)
                    {
                        auto z = i << j;
                        auto j0 = j & (N-1);
                        StandardInt_t<S, N> i0{i};
                        StandardInt_t<S, N> i1 = i0 << j0;
                        if (z != i1)
                        {
                            std::cout << bbi::to_string<Z>() << "{" << i << "} << "
                                      << j << " is " << z
                                      << " but should be " << i1+0 << '\n';
                            return;
                        }
                        if (j == rngy[ry][1])
                            break;
                        ++j;
                    }
                }
                if (i == rngx[rx][1])
                    break;
                ++i;
            } 
        }
    }
    else
    {
        auto constexpr rngx = make_range<S, N, P>();
        for (auto rx = 0u; rx < rngx.size(); ++rx)
        {
            auto i = rngx[rx][0];
            while (true)
            {
                auto constexpr rngy = make_range<bbi::Signed, 32, bbi::Wrap>();
                for (auto ry = 0u; ry < rngy.size(); ++ry)
                {
                    I j{rngy[ry][0]};
                    while (true)
                    {
                        try
                        {
                            auto z = i << j;
                            StandardInt_t<S, N> i0{i};
                            StandardInt_t<S, N> i1{};
                            if constexpr (P{} == bbi::Saturate{})
                            {
                                if (j < 0)
                                {
                                    // i1 = i0 >> -j;  // but saturated
                                    if (-j >= N)
                                    {
                                        if (z < Z{})
                                            i1 = ~i1;
                                        else
                                            i1 = 0;
                                    }
                                    else
                                        i1 = i0 >> -j;
                                }
                                else if (j >= N)
                                {
                                    i1 = 0;
                                }
                                else
                                {
                                    i1 = i0 << j;
                                }
                            }
                            else if constexpr (P{} == bbi::Throw{})
                            {
                                if (j < 0 || j >= N)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << i << "} << "
                                              << j+0
                                              << " is " << z
                                              << " but should have thrown an exception"
                                              << " because " << j+0 << " is out of range\n";
                                    return;
                                }
                                i1 = i0 << j;
                            }
                            if (z != i1)
                            {
                                std::cout << bbi::to_string<Z>() << "{" << i << "} << "
                                          << j+0 << " is " << z
                                          << " but should be " << i1+0 << '\n';
                                return;
                            }
                        }
                        catch (std::exception const&)
                        {
                            if constexpr (P{} == bbi::Throw{})
                            {
                                if (0 <= j && j < N)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << i << "} + "
                                              << j+0
                                              << " threw an exception but should not have "
                                                 "because the second argument is within range\n";
                                    return;
                                }
                            }
                            else
                            {
                                std::cout << bbi::to_string<Z>() << "{" << i << "} + "
                                          << j+0
                                          << " threw an exception but should not have because "
                                             "this policy does not throw\n";
                                return;
                            }
                        }
                        if (j == rngy[ry][1])
                            break;
                        ++j;
                    }
                }
                if (i == rngx[rx][1])
                    break;
                ++i;
            } 
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_rightshift()
{
    using Z = bbi::Z<S, N, P>;
    using I = int;
    if constexpr (P{} == bbi::Wrap{})
    {
        auto constexpr rngx = make_range<S, N, P>();
        for (auto rx = 0u; rx < rngx.size(); ++rx)
        {
            auto i = rngx[rx][0];
            while (true)
            {
                auto constexpr rngy = make_range<bbi::Signed, 32, bbi::Wrap>();
                for (auto ry = 0u; ry < rngy.size(); ++ry)
                {
                    I j{rngy[ry][0]};
                    while (true)
                    {
                        auto z = i >> j;
                        auto j0 = j & (N-1);
                        StandardInt_t<S, N> i0{i};
                        StandardInt_t<S, N> i1 = i0 >> j0;
                        if (z != i1)
                        {
                            std::cout << bbi::to_string<Z>() << "{" << i << "} >> "
                                      << j << " is " << z
                                      << " but should be " << i1+0 << '\n';
                            return;
                        }
                        if (j == rngy[ry][1])
                            break;
                        ++j;
                    }
                }
                if (i == rngx[rx][1])
                    break;
                ++i;
            } 
        }
    }
    else
    {
        auto constexpr rngx = make_range<S, N, P>();
        for (auto rx = 0u; rx < rngx.size(); ++rx)
        {
            auto i = rngx[rx][0];
            while (true)
            {
                auto constexpr rngy = make_range<bbi::Signed, 32, bbi::Wrap>();
                for (auto ry = 0u; ry < rngy.size(); ++ry)
                {
                    I j{rngy[ry][0]};
                    while (true)
                    {
                        try
                        {
                            auto z = i << j;
                            StandardInt_t<S, N> i0{i};
                            StandardInt_t<S, N> i1{};
                            if constexpr (P{} == bbi::Saturate{})
                            {
                                if (j < 0)
                                {
                                    // i1 = i0 >> -j;  // but saturated
                                    if (-j >= N)
                                    {
                                        if (z < Z{})
                                            i1 = ~i1;
                                        else
                                            i1 = 0;
                                    }
                                    else
                                        i1 = i0 >> -j;
                                }
                                else if (j >= N)
                                {
                                    i1 = 0;
                                }
                                else
                                {
                                    i1 = i0 << j;
                                }
                            }
                            else if constexpr (P{} == bbi::Throw{})
                            {
                                if (j < 0 || j >= N)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << i << "} << "
                                              << j+0
                                              << " is " << z
                                              << " but should have thrown an exception"
                                              << " because " << j+0 << " is out of range\n";
                                    return;
                                }
                                i1 = i0 << j;
                            }
                            if (z != i1)
                            {
                                std::cout << bbi::to_string<Z>() << "{" << i << "} << "
                                          << j+0 << " is " << z
                                          << " but should be " << i1+0 << '\n';
                                return;
                            }
                        }
                        catch (std::exception const&)
                        {
                            if constexpr (P{} == bbi::Throw{})
                            {
                                if (0 <= j && j < N)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << i << "} + "
                                              << j+0
                                              << " threw an exception but should not have "
                                                 "because the second argument is within range\n";
                                    return;
                                }
                            }
                            else
                            {
                                std::cout << bbi::to_string<Z>() << "{" << i << "} + "
                                          << j+0
                                          << " threw an exception but should not have because "
                                             "this policy does not throw\n";
                                return;
                            }
                        }
                        if (j == rngy[ry][1])
                            break;
                        ++j;
                    }
                }
                if (i == rngx[rx][1])
                    break;
                ++i;
            } 
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P, std::integral I>
constexpr
void
convert_with_int()
{
    using Z = bbi::Z<S, N, P>;
    using K = StandardInt_t<S, N>;
    auto constexpr is_signedI = std::is_signed_v<I>;
    auto constexpr rngy = make_range<
                              std::conditional_t<is_signedI, bbi::Signed, bbi::Unsigned>,
                              sizeof(I)*8, bbi::Wrap>();
    for (auto ry = 0u; ry < rngy.size(); ++ry)
    {
        I i{rngy[ry][0]};
        while (true)
        {
            if constexpr (P{} == bbi::Wrap{})
            {
                Z z_from_i_test{i};
                I i_from_z_test{z_from_i_test};
                K z_from_i_expected(i);
                I i_from_z_expected(z_from_i_expected);
                if (z_from_i_test != z_from_i_expected)
                {
                    std::cout << bbi::to_string<Z>() << "{" << i+0 << "} should be "
                              << z_from_i_expected+0 << " but is " << z_from_i_test << '\n';
                    return;
                }
                if (i_from_z_test != i_from_z_expected)
                {
                    std::cout << bbi::to_string<I>() << "(" << bbi::to_string<Z>()
                              << "{" << i+0 << "}) should be " << i_from_z_expected+0
                              << " but is " << i_from_z_test+0 << '\n';
                    return;
                }
            }
            else if constexpr (P{} == bbi::Saturate{})
            {
                Z z_from_i_test{i};
                I i_from_z_test{z_from_i_test};
                K z_from_i_expected(i);
                if (i > std::numeric_limits<Z>::max())
                    z_from_i_expected = I{std::numeric_limits<Z>::max()};
                else if (i < std::numeric_limits<Z>::min())
                    z_from_i_expected = K(I{std::numeric_limits<Z>::min()});
                I i_from_z_expected(z_from_i_expected);
                if (z_from_i_test != z_from_i_expected)
                {
                    std::cout << bbi::to_string<Z>() << "{" << i+0 << "} should be "
                              << z_from_i_expected+0 << " but is " << z_from_i_test << '\n';
                    return;
                }
                if (i_from_z_test != i_from_z_expected)
                {
                    std::cout << bbi::to_string<I>() << "(" << bbi::to_string<Z>()
                              << "{" << i+0 << "}) should be " << i_from_z_expected+0
                              << " but is " << i_from_z_test+0 << '\n';
                    return;
                }
            }
            else if constexpr (P{} == bbi::Throw{})
            {
                try
                {
                    Z z_from_i_test{i};
                    I i_from_z_test{z_from_i_test};
                    K z_from_i_expected(i);
                    I i_from_z_expected(z_from_i_expected);
                    if (i > std::numeric_limits<Z>::max())
                    {
                        std::cout << bbi::to_string<Z>() << "{" << i+0 << "} should "
                                  "have thrown an exception but did not\n";
                        return;
                    }
                    else if (i < std::numeric_limits<Z>::min())
                    {
                        std::cout << bbi::to_string<Z>() << "{" << i+0 << "} should "
                                  "have thrown an exception but did not\n";
                        return;
                    }
                    if (z_from_i_test != z_from_i_expected)
                    {
                        std::cout << bbi::to_string<Z>() << "{" << i+0 << "} should be "
                                  << z_from_i_expected+0 << " but is " << z_from_i_test << '\n';
                        return;
                    }
                    if (i_from_z_test != i_from_z_expected)
                    {
                        std::cout << bbi::to_string<I>() << "(" << bbi::to_string<Z>()
                                  << "{" << i+0 << "}) should be " << i_from_z_expected+0
                                  << " but is " << i_from_z_test+0 << '\n';
                        return;
                    }
                }
                catch (std::exception const&)
                {
                    if (std::numeric_limits<Z>::min() <= i &&
                        i <= std::numeric_limits<Z>::max())
                    {
                        std::cout << bbi::to_string<Z>() << "{" << i+0 << "} threw an "
                                  "exception but should not have\n";
                        return;
                    }
                }
            }
            if (i == rngy[ry][1])
                break;
            ++i;
        }
    }
}

template <class Z1, class Z2>
constexpr
void
convert_from_Z()
{
    using I1 = StandardInt_t<typename Z1::sign, Z1::size>;
    using I2 = StandardInt_t<typename Z2::sign, Z2::size>;
    using P1 = typename Z1::policy;
    auto constexpr rngy = make_range<typename Z2::sign, Z2::size, typename Z2::policy>();
    for (auto ry = 0u; ry < rngy.size(); ++ry)
    {
        auto z2{rngy[ry][0]};
        while (true)
        {
            if constexpr (P1{} == bbi::Wrap{})
            {
                Z1 z1_from_z2_test{z2};
                I1 z1_from_z2_expected = I1(I2{z2});
                if (z1_from_z2_test != z1_from_z2_expected)
                {
                    std::cout << bbi::to_string<Z1>() << "{"
                              << bbi::to_string<Z2>() << "{" << z2 << "}} should be "
                              << z1_from_z2_expected+0 << " but is " << z1_from_z2_test << '\n';
                    return;
                }
            }
            else if constexpr (P1{} == bbi::Saturate{})
            {
                Z1 z1_from_z2_test{z2};
                I1 z1_from_z2_expected = I1(I2{z2});
                if (I2{z2} > std::numeric_limits<Z1>::max())
                    z1_from_z2_expected = std::numeric_limits<I1>::max();
                else if (I2{z2} < std::numeric_limits<Z1>::min())
                    z1_from_z2_expected = std::numeric_limits<I1>::min();
                if (z1_from_z2_test != z1_from_z2_expected)
                {
                    std::cout << bbi::to_string<Z1>() << "{"
                              << bbi::to_string<Z2>() << "{" << z2 << "}} should be "
                              << z1_from_z2_expected+0 << " but is " << z1_from_z2_test << '\n';
                    return;
                }
            }
            else if constexpr (P1{} == bbi::Throw{})
            {
                try
                {
                    Z1 z1_from_z2_test{z2};
                    I1 z1_from_z2_expected = I1(I2{z2});
                    if (I2{z2} < std::numeric_limits<Z1>::min() ||
                        I2{z2} > std::numeric_limits<Z1>::max())
                    {
                        std::cout << bbi::to_string<Z1>() << "{"
                                  << bbi::to_string<Z2>() << "{" << z2 << "}} should have "
                                  "thrown an exception but did not\n";
                    }
                    if (z1_from_z2_test != z1_from_z2_expected)
                    {
                        std::cout << bbi::to_string<Z1>() << "{"
                                  << bbi::to_string<Z2>() << "{" << z2 << "}} should be "
                                  << z1_from_z2_expected+0 << " but is " << z1_from_z2_test << '\n';
                        return;
                    }
                }
                catch (std::exception const&)
                {
                    if (I2{z2} >= std::numeric_limits<Z1>::min() &&
                        I2{z2} < std::numeric_limits<Z1>::max())
                    {
                        std::cout << bbi::to_string<Z1>() << "{"
                                  << bbi::to_string<Z2>() << "{" << z2 << "}} threw an "
                                  "exception but should not have\n";
                    }
                }
            }
            if (z2 == rngy[ry][1])
                break;
            ++z2;
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_add()
{
    using Z = bbi::Z<S, N, P>;
    using I2 = StandardInt_t<bbi::Signed, 2*N>;
    auto constexpr rng = make_range<S, N, P>();
    for (auto rx = 0u; rx < rng.size(); ++rx)
    {
        auto i = rng[rx][0];
        while (true)
        {
            for (auto ry = 0u; ry < rng.size(); ++ry)
            {
                auto j = rng[ry][0];
                while (true)
                {
                    Z zx{i};
                    Z zy{j};
                    I2 ix{i};
                    I2 iy{j};
                    I2 i1 = ix + iy;
                    try
                    {
                        auto z1 = zx + zy;
                        if (z1 != i1)
                        {
                            if constexpr (P{} == bbi::Wrap{})
                            {
                                StandardInt_t<S, N> i0(i1);
                                if (z1 != i0)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} + "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i0+0 << '\n';
                                    return;
                                }
                            }
                            else if constexpr (P{} == bbi::Saturate{})
                            {
                                auto m = I2{std::numeric_limits<Z>::min()};
                                auto M = I2{std::numeric_limits<Z>::max()};
                                if (i1 < m)
                                {
                                    if (z1 != std::numeric_limits<Z>::min())
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} + "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should be " << m+0 << '\n';
                                        return;
                                    }
                                }
                                else if (i1 > M)
                                {
                                    if (z1 != std::numeric_limits<Z>::max())
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} + "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should be " << M+0 << '\n';
                                        return;
                                    }
                                }
                                else
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} + "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1+0 << '\n';
                                    return;
                                }
                            }
                            else if constexpr (P{} == bbi::Throw{})
                            {
                                auto m = I2{std::numeric_limits<Z>::min()};
                                auto M = I2{std::numeric_limits<Z>::max()};
                                if (i1 < m || i1 > M)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} + "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should have thrown an exception "
                                                 "because the exact result is " << i1+0 << '\n';
                                    return;
                                }
                                else
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} + "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1+0 << '\n';
                                    return;
                                }
                            }
                        }
                    }
                    catch (std::exception const&)
                    {
                        if constexpr (P{} == bbi::Throw{})
                        {
                            auto m = I2{std::numeric_limits<Z>::min()};
                            auto M = I2{std::numeric_limits<Z>::max()};
                            if (m <= i1 && i1 <= M)
                            {
                                std::cout << bbi::to_string<Z>() << "{" << zx << "} + "
                                          << bbi::to_string<Z>() << "{" << zy
                                          << "} threw an exception but should not have "
                                             "because the exact result is " << i1+0 << '\n';
                                return;
                            }
                        }
                        else
                        {
                            std::cout << bbi::to_string<Z>() << "{" << zx << "} + "
                                      << bbi::to_string<Z>() << "{" << zy
                                      << "} threw an exception but should not have because "
                                         "this policy does not throw\n";
                            return;
                        }
                    }
                    if (j == rng[ry][1])
                        break;
                    ++j;
                }
            }
            if (i == rng[rx][1])
                break;
            ++i;
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_sub()
{
    using Z = bbi::Z<S, N, P>;
    using I2 = StandardInt_t<bbi::Signed, 2*N>;
    auto constexpr rng = make_range<S, N, P>();
    for (auto rx = 0u; rx < rng.size(); ++rx)
    {
        auto i = rng[rx][0];
        while (true)
        {
            for (auto ry = 0u; ry < rng.size(); ++ry)
            {
                auto j = rng[ry][0];
                while (true)
                {
                    Z zx{i};
                    Z zy{j};
                    I2 ix{i};
                    I2 iy{j};
                    I2 i1 = ix - iy;
                    try
                    {
                        auto z1 = zx - zy;
                        if (z1 != i1)
                        {
                            if constexpr (P{} == bbi::Wrap{})
                            {
                                StandardInt_t<S, N> i0(i1);
                                if (z1 != i0)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} - "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i0+0 << '\n';
                                    return;
                                }
                            }
                            else if constexpr (P{} == bbi::Saturate{})
                            {
                                auto m = I2{std::numeric_limits<Z>::min()};
                                auto M = I2{std::numeric_limits<Z>::max()};
                                if (i1 < m)
                                {
                                    if (z1 != std::numeric_limits<Z>::min())
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} - "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should be " << m+0 << '\n';
                                        return;
                                    }
                                }
                                else if (i1 > M)
                                {
                                    if (z1 != std::numeric_limits<Z>::max())
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} - "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should be " << M+0 << '\n';
                                        return;
                                    }
                                }
                                else
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} - "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1+0 << '\n';
                                    return;
                                }
                            }
                            else if constexpr (P{} == bbi::Throw{})
                            {
                                auto m = I2{std::numeric_limits<Z>::min()};
                                auto M = I2{std::numeric_limits<Z>::max()};
                                if (i1 < m || i1 > M)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} - "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should have thrown an exception "
                                                 "because the exact result is " << i1+0 << '\n';
                                    return;
                                }
                                else
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} - "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1+0 << '\n';
                                    return;
                                }
                            }
                        }
                    }
                    catch (std::exception const&)
                    {
                        if constexpr (P{} == bbi::Throw{})
                        {
                            auto m = I2{std::numeric_limits<Z>::min()};
                            auto M = I2{std::numeric_limits<Z>::max()};
                            if (m <= i1 && i1 <= M)
                            {
                                std::cout << bbi::to_string<Z>() << "{" << zx << "} - "
                                          << bbi::to_string<Z>() << "{" << zy
                                          << "} threw an exception but should not have "
                                             "because the exact result is " << i1+0 << '\n';
                                return;
                            }
                        }
                        else
                        {
                            std::cout << bbi::to_string<Z>() << "{" << zx << "} - "
                                      << bbi::to_string<Z>() << "{" << zy
                                      << "} threw an exception but should not have because "
                                         "this policy does not throw\n";
                            return;
                        }
                    }
                    if (j == rng[ry][1])
                        break;
                    ++j;
                }
            }
            if (i == rng[rx][1])
                break;
            ++i;
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
constexpr
void
test_mul()
{
    using Z = bbi::Z<S, N, P>;
    using I2 = StandardInt_t<S, 2*N>;
    auto constexpr rng = make_range<S, N, P>();
    for (auto rx = 0u; rx < rng.size(); ++rx)
    {
        auto i = rng[rx][0];
        while (true)
        {
            for (auto ry = 0u; ry < rng.size(); ++ry)
            {
                auto j = rng[ry][0];
                while (true)
                {
                    Z zx{i};
                    Z zy{j};
                    I2 ix{i};
                    I2 iy{j};
                    I2 i1 = ix * iy;
                    try
                    {
                        auto z1 = zx * zy;
                        if (z1 != i1)
                        {
                            if constexpr (P{} == bbi::Wrap{})
                            {
                                StandardInt_t<S, N> i0(i1);
                                if (z1 != i0)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} * "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i0+0 << '\n';
                                    return;
                                }
                            }
                            else if constexpr (P{} == bbi::Saturate{})
                            {
                                auto m = I2{std::numeric_limits<Z>::min()};
                                auto M = I2{std::numeric_limits<Z>::max()};
                                if (i1 < m)
                                {
                                    if (z1 != std::numeric_limits<Z>::min())
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} * "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should be " << m+0 << '\n';
                                        return;
                                    }
                                }
                                else if (i1 > M)
                                {
                                    if (z1 != std::numeric_limits<Z>::max())
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} * "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should be " << M+0 << '\n';
                                        return;
                                    }
                                }
                                else
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} * "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1+0 << '\n';
                                    return;
                                }
                            }
                            else if constexpr (P{} == bbi::Throw{})
                            {
                                auto m = I2{std::numeric_limits<Z>::min()};
                                auto M = I2{std::numeric_limits<Z>::max()};
                                if (i1 < m || i1 > M)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} * "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should have thrown an exception "
                                                 "because the exact result is " << i1 << '\n';
                                    return;
                                }
                                else
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} * "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1 << '\n';
                                    return;
                                }
                            }
                        }
                    }
                    catch (std::exception const&)
                    {
                        if constexpr (P{} == bbi::Throw{})
                        {
                            auto m = I2{std::numeric_limits<Z>::min()};
                            auto M = I2{std::numeric_limits<Z>::max()};
                            if (m <= i1 && i1 <= M)
                            {
                                std::cout << bbi::to_string<Z>() << "{" << zx << "} * "
                                          << bbi::to_string<Z>() << "{" << zy
                                          << "} threw an exception but should not have "
                                             "because the exact result is " << i1+0 << '\n';
                                return;
                            }
                        }
                        else
                        {
                            std::cout << bbi::to_string<Z>() << "{" << zx << "} * "
                                      << bbi::to_string<Z>() << "{" << zy
                                      << "} threw an exception but should not have because "
                                         "this policy does not throw\n";
                            return;
                        }
                    }
                    if (j == rng[ry][1])
                        break;
                    ++j;
                }
            }
            if (i == rng[rx][1])
                break;
            ++i;
        }
    }
}

template <bbi::SignTag S, unsigned N, bbi::Policy P>
void
test_div()
{
    using Z = bbi::Z<S, N, P>;
    using I = StandardInt_t<S, N>;
    auto constexpr rng = make_range<S, N, P>();
    for (auto rx = 0u; rx < rng.size(); ++rx)
    {
        auto i = rng[rx][0];
        while (true)
        {
            for (auto ry = 0u; ry < rng.size(); ++ry)
            {
                auto j = rng[ry][0];
                while (true)
                {
                    Z zx{i};
                    Z zy{j};
                    I ix{i};
                    I iy{j};
                    I i1{};
                    if (iy != 0)
                        i1 = ix / iy;
                    try
                    {
                        auto z1 = zx / zy;
                        if (iy == 0)
                        {
                            if constexpr (P{} == bbi::Saturate{})
                            {
                                auto m = I{std::numeric_limits<Z>::min()};
                                auto M = I{std::numeric_limits<Z>::max()};
                                if (ix < 0)
                                {
                                    if (z1 != std::numeric_limits<Z>::min())
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should be " << m+0 << '\n';
                                        return;
                                    }
                                }
                                else  // ix >= 0
                                {
                                    if (z1 != std::numeric_limits<Z>::max())
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should be " << M+0 << '\n';
                                        return;
                                    }
                                }
                            }
                            else if constexpr (P{} == bbi::Throw{})
                            {
                                std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                          << bbi::to_string<Z>() << "{" << zy
                                          << "} is " << z1
                                          << " but should have thrown an exception "
                                             "because of division by 0\n";
                                return;
                            }
                        }
                        else  // iy != 0
                        {
                            if constexpr (P{} == bbi::Wrap{})
                            {
                                if (z1 != i1)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1+0 << '\n';
                                    return;
                                }
                            }
                            else if constexpr (P{} == bbi::Saturate{})
                            {
                                auto m = I{std::numeric_limits<Z>::min()};
                                auto M = I{std::numeric_limits<Z>::max()};
                                if constexpr (S{} == bbi::Signed{})
                                {
                                    if (ix == m && iy == -1)
                                    {
                                        if (z1 != M)
                                        {
                                            std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                                      << bbi::to_string<Z>() << "{" << zy
                                                      << "} is " << z1
                                                      << " but should be " << M+0 << '\n';
                                            return;
                                        }
                                        goto next;
                                    }
                                }
                                if (z1 != i1)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1+0 << '\n';
                                    return;
                                }
                            }
                            else if constexpr (P{} == bbi::Throw{})
                            {
                                auto m = I{std::numeric_limits<Z>::min()};
                                if constexpr (S{} == bbi::Signed{})
                                {
                                    if (ix == m && iy == -1)
                                    {
                                        std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                                  << bbi::to_string<Z>() << "{" << zy
                                                  << "} is " << z1
                                                  << " but should have thrown an exception\n";
                                        return;
                                    }
                                }
                                if (z1 != i1)
                                {
                                    std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                              << bbi::to_string<Z>() << "{" << zy
                                              << "} is " << z1
                                              << " but should be " << i1+0 << '\n';
                                    return;
                                }
                            }
                        }
                    }
                    catch (std::exception const&)
                    {
                        if constexpr (P{} == bbi::Throw{})
                        {
                            auto m = I{std::numeric_limits<Z>::min()};
                            if (!((ix == m && iy == -1) || (iy == 0)))
                            {
                                std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                          << bbi::to_string<Z>() << "{" << zy
                                          << "} threw an exception but should not have "
                                             "because the exact result is " << i1+0 << '\n';
                                return;
                            }
                        }
                        else
                        {
                            std::cout << bbi::to_string<Z>() << "{" << zx << "} / "
                                      << bbi::to_string<Z>() << "{" << zy
                                      << "} threw an exception but should not have because "
                                         "this policy does not throw\n";
                            return;
                        }
                    }
                next:
                    if (j == rng[ry][1])
                        break;
                    ++j;
                }
            }
            if (i == rng[rx][1])
                break;
            ++i;
        }
    }
}

int
main()
{
    test_trivial_special_members<bbi::Signed>();
    test_trivial_special_members<bbi::Unsigned>();

    test_trivial_special_members<bbi::Wrap>();
    test_trivial_special_members<bbi::Saturate>();
    test_trivial_special_members<bbi::Terminate>();
    test_trivial_special_members<bbi::Throw>();

    test_trivial_special_members<bbi::wrap::i8>();
    test_trivial_special_members<bbi::wrap::i16>();
    test_trivial_special_members<bbi::wrap::i32>();
    test_trivial_special_members<bbi::wrap::i64>();
    test_trivial_special_members<bbi::wrap::i128>();
    test_trivial_special_members<bbi::wrap::i256>();
    test_trivial_special_members<bbi::wrap::i512>();
    test_trivial_special_members<bbi::wrap::i1024>();

    test_trivial_special_members<bbi::thrw::u8>();
    test_trivial_special_members<bbi::thrw::u16>();
    test_trivial_special_members<bbi::thrw::u32>();
    test_trivial_special_members<bbi::thrw::u64>();
    test_trivial_special_members<bbi::thrw::u128>();
    test_trivial_special_members<bbi::thrw::u256>();
    test_trivial_special_members<bbi::thrw::u512>();
    test_trivial_special_members<bbi::thrw::u1024>();


    test_bit_not<bbi::Signed, 8, bbi::Wrap>();
    test_bit_not<bbi::Signed, 16, bbi::Wrap>();
    test_bit_not<bbi::Signed, 32, bbi::Wrap>();
    test_bit_not<bbi::Signed, 64, bbi::Wrap>();

    test_bit_not<bbi::Unsigned, 8, bbi::Wrap>();
    test_bit_not<bbi::Unsigned, 16, bbi::Wrap>();
    test_bit_not<bbi::Unsigned, 32, bbi::Wrap>();
    test_bit_not<bbi::Unsigned, 64, bbi::Wrap>();

    test_bit_not<bbi::Signed, 8, bbi::Saturate>();
    test_bit_not<bbi::Signed, 16, bbi::Saturate>();
    test_bit_not<bbi::Signed, 32, bbi::Saturate>();
    test_bit_not<bbi::Signed, 64, bbi::Saturate>();

    test_bit_not<bbi::Unsigned, 8, bbi::Saturate>();
    test_bit_not<bbi::Unsigned, 16, bbi::Saturate>();
    test_bit_not<bbi::Unsigned, 32, bbi::Saturate>();
    test_bit_not<bbi::Unsigned, 64, bbi::Saturate>();

    test_bit_not<bbi::Signed, 8, bbi::Terminate>();
    test_bit_not<bbi::Signed, 16, bbi::Terminate>();
    test_bit_not<bbi::Signed, 32, bbi::Terminate>();
    test_bit_not<bbi::Signed, 64, bbi::Terminate>();

    test_bit_not<bbi::Unsigned, 8, bbi::Terminate>();
    test_bit_not<bbi::Unsigned, 16, bbi::Terminate>();
    test_bit_not<bbi::Unsigned, 32, bbi::Terminate>();
    test_bit_not<bbi::Unsigned, 64, bbi::Terminate>();

    test_bit_not<bbi::Signed, 8, bbi::Throw>();
    test_bit_not<bbi::Signed, 16, bbi::Throw>();
    test_bit_not<bbi::Signed, 32, bbi::Throw>();
    test_bit_not<bbi::Signed, 64, bbi::Throw>();

    test_bit_not<bbi::Unsigned, 8, bbi::Throw>();
    test_bit_not<bbi::Unsigned, 16, bbi::Throw>();
    test_bit_not<bbi::Unsigned, 32, bbi::Throw>();
    test_bit_not<bbi::Unsigned, 64, bbi::Throw>();


    test_bit_and<bbi::Signed, 8, bbi::Wrap>();
    test_bit_and<bbi::Signed, 16, bbi::Wrap>();
    test_bit_and<bbi::Signed, 32, bbi::Wrap>();
    test_bit_and<bbi::Signed, 64, bbi::Wrap>();

    test_bit_and<bbi::Unsigned, 8, bbi::Wrap>();
    test_bit_and<bbi::Unsigned, 16, bbi::Wrap>();
    test_bit_and<bbi::Unsigned, 32, bbi::Wrap>();
    test_bit_and<bbi::Unsigned, 64, bbi::Wrap>();

    test_bit_and<bbi::Signed, 8, bbi::Saturate>();
    test_bit_and<bbi::Signed, 16, bbi::Saturate>();
    test_bit_and<bbi::Signed, 32, bbi::Saturate>();
    test_bit_and<bbi::Signed, 64, bbi::Saturate>();

    test_bit_and<bbi::Unsigned, 8, bbi::Saturate>();
    test_bit_and<bbi::Unsigned, 16, bbi::Saturate>();
    test_bit_and<bbi::Unsigned, 32, bbi::Saturate>();
    test_bit_and<bbi::Unsigned, 64, bbi::Saturate>();

    test_bit_and<bbi::Signed, 8, bbi::Terminate>();
    test_bit_and<bbi::Signed, 16, bbi::Terminate>();
    test_bit_and<bbi::Signed, 32, bbi::Terminate>();
    test_bit_and<bbi::Signed, 64, bbi::Terminate>();

    test_bit_and<bbi::Unsigned, 8, bbi::Terminate>();
    test_bit_and<bbi::Unsigned, 16, bbi::Terminate>();
    test_bit_and<bbi::Unsigned, 32, bbi::Terminate>();
    test_bit_and<bbi::Unsigned, 64, bbi::Terminate>();

    test_bit_and<bbi::Signed, 8, bbi::Throw>();
    test_bit_and<bbi::Signed, 16, bbi::Throw>();
    test_bit_and<bbi::Signed, 32, bbi::Throw>();
    test_bit_and<bbi::Signed, 64, bbi::Throw>();

    test_bit_and<bbi::Unsigned, 8, bbi::Throw>();
    test_bit_and<bbi::Unsigned, 16, bbi::Throw>();
    test_bit_and<bbi::Unsigned, 32, bbi::Throw>();
    test_bit_and<bbi::Unsigned, 64, bbi::Throw>();


    test_bit_or<bbi::Signed, 8, bbi::Wrap>();
    test_bit_or<bbi::Signed, 16, bbi::Wrap>();
    test_bit_or<bbi::Signed, 32, bbi::Wrap>();
    test_bit_or<bbi::Signed, 64, bbi::Wrap>();

    test_bit_or<bbi::Unsigned, 8, bbi::Wrap>();
    test_bit_or<bbi::Unsigned, 16, bbi::Wrap>();
    test_bit_or<bbi::Unsigned, 32, bbi::Wrap>();
    test_bit_or<bbi::Unsigned, 64, bbi::Wrap>();

    test_bit_or<bbi::Signed, 8, bbi::Saturate>();
    test_bit_or<bbi::Signed, 16, bbi::Saturate>();
    test_bit_or<bbi::Signed, 32, bbi::Saturate>();
    test_bit_or<bbi::Signed, 64, bbi::Saturate>();

    test_bit_or<bbi::Unsigned, 8, bbi::Saturate>();
    test_bit_or<bbi::Unsigned, 16, bbi::Saturate>();
    test_bit_or<bbi::Unsigned, 32, bbi::Saturate>();
    test_bit_or<bbi::Unsigned, 64, bbi::Saturate>();

    test_bit_or<bbi::Signed, 8, bbi::Terminate>();
    test_bit_or<bbi::Signed, 16, bbi::Terminate>();
    test_bit_or<bbi::Signed, 32, bbi::Terminate>();
    test_bit_or<bbi::Signed, 64, bbi::Terminate>();

    test_bit_or<bbi::Unsigned, 8, bbi::Terminate>();
    test_bit_or<bbi::Unsigned, 16, bbi::Terminate>();
    test_bit_or<bbi::Unsigned, 32, bbi::Terminate>();
    test_bit_or<bbi::Unsigned, 64, bbi::Terminate>();

    test_bit_or<bbi::Signed, 8, bbi::Throw>();
    test_bit_or<bbi::Signed, 16, bbi::Throw>();
    test_bit_or<bbi::Signed, 32, bbi::Throw>();
    test_bit_or<bbi::Signed, 64, bbi::Throw>();

    test_bit_or<bbi::Unsigned, 8, bbi::Throw>();
    test_bit_or<bbi::Unsigned, 16, bbi::Throw>();
    test_bit_or<bbi::Unsigned, 32, bbi::Throw>();
    test_bit_or<bbi::Unsigned, 64, bbi::Throw>();


    test_bit_xor<bbi::Signed, 8, bbi::Wrap>();
    test_bit_xor<bbi::Signed, 16, bbi::Wrap>();
    test_bit_xor<bbi::Signed, 32, bbi::Wrap>();
    test_bit_xor<bbi::Signed, 64, bbi::Wrap>();

    test_bit_xor<bbi::Unsigned, 8, bbi::Wrap>();
    test_bit_xor<bbi::Unsigned, 16, bbi::Wrap>();
    test_bit_xor<bbi::Unsigned, 32, bbi::Wrap>();
    test_bit_xor<bbi::Unsigned, 64, bbi::Wrap>();

    test_bit_xor<bbi::Signed, 8, bbi::Saturate>();
    test_bit_xor<bbi::Signed, 16, bbi::Saturate>();
    test_bit_xor<bbi::Signed, 32, bbi::Saturate>();
    test_bit_xor<bbi::Signed, 64, bbi::Saturate>();

    test_bit_xor<bbi::Unsigned, 8, bbi::Saturate>();
    test_bit_xor<bbi::Unsigned, 16, bbi::Saturate>();
    test_bit_xor<bbi::Unsigned, 32, bbi::Saturate>();
    test_bit_xor<bbi::Unsigned, 64, bbi::Saturate>();

    test_bit_xor<bbi::Signed, 8, bbi::Terminate>();
    test_bit_xor<bbi::Signed, 16, bbi::Terminate>();
    test_bit_xor<bbi::Signed, 32, bbi::Terminate>();
    test_bit_xor<bbi::Signed, 64, bbi::Terminate>();

    test_bit_xor<bbi::Unsigned, 8, bbi::Terminate>();
    test_bit_xor<bbi::Unsigned, 16, bbi::Terminate>();
    test_bit_xor<bbi::Unsigned, 32, bbi::Terminate>();
    test_bit_xor<bbi::Unsigned, 64, bbi::Terminate>();

    test_bit_xor<bbi::Signed, 8, bbi::Throw>();
    test_bit_xor<bbi::Signed, 16, bbi::Throw>();
    test_bit_xor<bbi::Signed, 32, bbi::Throw>();
    test_bit_xor<bbi::Signed, 64, bbi::Throw>();

    test_bit_xor<bbi::Unsigned, 8, bbi::Throw>();
    test_bit_xor<bbi::Unsigned, 16, bbi::Throw>();
    test_bit_xor<bbi::Unsigned, 32, bbi::Throw>();
    test_bit_xor<bbi::Unsigned, 64, bbi::Throw>();


    test_leftshift<bbi::Unsigned, 8, bbi::Wrap>();
    test_leftshift<bbi::Unsigned, 16, bbi::Wrap>();
    test_leftshift<bbi::Unsigned, 32, bbi::Wrap>();
    test_leftshift<bbi::Unsigned, 64, bbi::Wrap>();

    test_leftshift<bbi::Signed, 8, bbi::Wrap>();
    test_leftshift<bbi::Signed, 16, bbi::Wrap>();
    test_leftshift<bbi::Signed, 32, bbi::Wrap>();
    test_leftshift<bbi::Signed, 64, bbi::Wrap>();

    test_leftshift<bbi::Unsigned, 8, bbi::Saturate>();
    test_leftshift<bbi::Unsigned, 16, bbi::Saturate>();
    test_leftshift<bbi::Unsigned, 32, bbi::Saturate>();
    test_leftshift<bbi::Unsigned, 64, bbi::Saturate>();

    test_leftshift<bbi::Signed, 8, bbi::Saturate>();
    test_leftshift<bbi::Signed, 16, bbi::Saturate>();
    test_leftshift<bbi::Signed, 32, bbi::Saturate>();
    test_leftshift<bbi::Signed, 64, bbi::Saturate>();

    test_leftshift<bbi::Unsigned, 8, bbi::Throw>();
    test_leftshift<bbi::Unsigned, 16, bbi::Throw>();
    test_leftshift<bbi::Unsigned, 32, bbi::Throw>();
    test_leftshift<bbi::Unsigned, 64, bbi::Throw>();

    test_leftshift<bbi::Signed, 8, bbi::Throw>();
    test_leftshift<bbi::Signed, 16, bbi::Throw>();
    test_leftshift<bbi::Signed, 32, bbi::Throw>();
    test_leftshift<bbi::Signed, 64, bbi::Throw>();


    test_rightshift<bbi::Unsigned, 8, bbi::Wrap>();
    test_rightshift<bbi::Unsigned, 16, bbi::Wrap>();
    test_rightshift<bbi::Unsigned, 32, bbi::Wrap>();
    test_rightshift<bbi::Unsigned, 64, bbi::Wrap>();

    test_rightshift<bbi::Signed, 8, bbi::Wrap>();
    test_rightshift<bbi::Signed, 16, bbi::Wrap>();
    test_rightshift<bbi::Signed, 32, bbi::Wrap>();
    test_rightshift<bbi::Signed, 64, bbi::Wrap>();

    test_rightshift<bbi::Unsigned, 8, bbi::Saturate>();
    test_rightshift<bbi::Unsigned, 16, bbi::Saturate>();
    test_rightshift<bbi::Unsigned, 32, bbi::Saturate>();
    test_rightshift<bbi::Unsigned, 64, bbi::Saturate>();

    test_rightshift<bbi::Signed, 8, bbi::Saturate>();
    test_rightshift<bbi::Signed, 16, bbi::Saturate>();
    test_rightshift<bbi::Signed, 32, bbi::Saturate>();
    test_rightshift<bbi::Signed, 64, bbi::Saturate>();

    test_rightshift<bbi::Unsigned, 8, bbi::Throw>();
    test_rightshift<bbi::Unsigned, 16, bbi::Throw>();
    test_rightshift<bbi::Unsigned, 32, bbi::Throw>();
    test_rightshift<bbi::Unsigned, 64, bbi::Throw>();

    test_rightshift<bbi::Signed, 8, bbi::Throw>();
    test_rightshift<bbi::Signed, 16, bbi::Throw>();
    test_rightshift<bbi::Signed, 32, bbi::Throw>();
    test_rightshift<bbi::Signed, 64, bbi::Throw>();


    convert_with_int<bbi::Signed, 8, bbi::Wrap, std::int8_t>();
    convert_with_int<bbi::Signed, 8, bbi::Wrap, std::int16_t>();
    convert_with_int<bbi::Signed, 8, bbi::Wrap, std::int32_t>();
    convert_with_int<bbi::Signed, 8, bbi::Wrap, std::int64_t>();

    convert_with_int<bbi::Signed, 8, bbi::Wrap, std::uint8_t>();
    convert_with_int<bbi::Signed, 8, bbi::Wrap, std::uint16_t>();
    convert_with_int<bbi::Signed, 8, bbi::Wrap, std::uint32_t>();
    convert_with_int<bbi::Signed, 8, bbi::Wrap, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 8, bbi::Wrap, std::int8_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Wrap, std::int16_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Wrap, std::int32_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Wrap, std::int64_t>();

    convert_with_int<bbi::Unsigned, 8, bbi::Wrap, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Wrap, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Wrap, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Wrap, std::uint64_t>();

    convert_with_int<bbi::Signed, 16, bbi::Wrap, std::int8_t>();
    convert_with_int<bbi::Signed, 16, bbi::Wrap, std::int16_t>();
    convert_with_int<bbi::Signed, 16, bbi::Wrap, std::int32_t>();
    convert_with_int<bbi::Signed, 16, bbi::Wrap, std::int64_t>();

    convert_with_int<bbi::Signed, 16, bbi::Wrap, std::uint8_t>();
    convert_with_int<bbi::Signed, 16, bbi::Wrap, std::uint16_t>();
    convert_with_int<bbi::Signed, 16, bbi::Wrap, std::uint32_t>();
    convert_with_int<bbi::Signed, 16, bbi::Wrap, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 16, bbi::Wrap, std::int8_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Wrap, std::int16_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Wrap, std::int32_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Wrap, std::int64_t>();

    convert_with_int<bbi::Unsigned, 16, bbi::Wrap, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Wrap, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Wrap, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Wrap, std::uint64_t>();

    convert_with_int<bbi::Signed, 32, bbi::Wrap, std::int8_t>();
    convert_with_int<bbi::Signed, 32, bbi::Wrap, std::int16_t>();
    convert_with_int<bbi::Signed, 32, bbi::Wrap, std::int32_t>();
    convert_with_int<bbi::Signed, 32, bbi::Wrap, std::int64_t>();

    convert_with_int<bbi::Signed, 32, bbi::Wrap, std::uint8_t>();
    convert_with_int<bbi::Signed, 32, bbi::Wrap, std::uint16_t>();
    convert_with_int<bbi::Signed, 32, bbi::Wrap, std::uint32_t>();
    convert_with_int<bbi::Signed, 32, bbi::Wrap, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 32, bbi::Wrap, std::int8_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Wrap, std::int16_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Wrap, std::int32_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Wrap, std::int64_t>();

    convert_with_int<bbi::Unsigned, 32, bbi::Wrap, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Wrap, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Wrap, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Wrap, std::uint64_t>();

    convert_with_int<bbi::Signed, 64, bbi::Wrap, std::int8_t>();
    convert_with_int<bbi::Signed, 64, bbi::Wrap, std::int16_t>();
    convert_with_int<bbi::Signed, 64, bbi::Wrap, std::int32_t>();
    convert_with_int<bbi::Signed, 64, bbi::Wrap, std::int64_t>();

    convert_with_int<bbi::Signed, 64, bbi::Wrap, std::uint8_t>();
    convert_with_int<bbi::Signed, 64, bbi::Wrap, std::uint16_t>();
    convert_with_int<bbi::Signed, 64, bbi::Wrap, std::uint32_t>();
    convert_with_int<bbi::Signed, 64, bbi::Wrap, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 64, bbi::Wrap, std::int8_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Wrap, std::int16_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Wrap, std::int32_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Wrap, std::int64_t>();

    convert_with_int<bbi::Unsigned, 64, bbi::Wrap, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Wrap, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Wrap, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Wrap, std::uint64_t>();


    convert_with_int<bbi::Signed, 8, bbi::Saturate, std::int8_t>();
    convert_with_int<bbi::Signed, 8, bbi::Saturate, std::int16_t>();
    convert_with_int<bbi::Signed, 8, bbi::Saturate, std::int32_t>();
    convert_with_int<bbi::Signed, 8, bbi::Saturate, std::int64_t>();

    convert_with_int<bbi::Signed, 8, bbi::Saturate, std::uint8_t>();
    convert_with_int<bbi::Signed, 8, bbi::Saturate, std::uint16_t>();
    convert_with_int<bbi::Signed, 8, bbi::Saturate, std::uint32_t>();
    convert_with_int<bbi::Signed, 8, bbi::Saturate, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 8, bbi::Saturate, std::int8_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Saturate, std::int16_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Saturate, std::int32_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Saturate, std::int64_t>();

    convert_with_int<bbi::Unsigned, 8, bbi::Saturate, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Saturate, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Saturate, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Saturate, std::uint64_t>();

    convert_with_int<bbi::Signed, 16, bbi::Saturate, std::int8_t>();
    convert_with_int<bbi::Signed, 16, bbi::Saturate, std::int16_t>();
    convert_with_int<bbi::Signed, 16, bbi::Saturate, std::int32_t>();
    convert_with_int<bbi::Signed, 16, bbi::Saturate, std::int64_t>();

    convert_with_int<bbi::Signed, 16, bbi::Saturate, std::uint8_t>();
    convert_with_int<bbi::Signed, 16, bbi::Saturate, std::uint16_t>();
    convert_with_int<bbi::Signed, 16, bbi::Saturate, std::uint32_t>();
    convert_with_int<bbi::Signed, 16, bbi::Saturate, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 16, bbi::Saturate, std::int8_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Saturate, std::int16_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Saturate, std::int32_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Saturate, std::int64_t>();

    convert_with_int<bbi::Unsigned, 16, bbi::Saturate, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Saturate, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Saturate, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Saturate, std::uint64_t>();

    convert_with_int<bbi::Signed, 32, bbi::Saturate, std::int8_t>();
    convert_with_int<bbi::Signed, 32, bbi::Saturate, std::int16_t>();
    convert_with_int<bbi::Signed, 32, bbi::Saturate, std::int32_t>();
    convert_with_int<bbi::Signed, 32, bbi::Saturate, std::int64_t>();

    convert_with_int<bbi::Signed, 32, bbi::Saturate, std::uint8_t>();
    convert_with_int<bbi::Signed, 32, bbi::Saturate, std::uint16_t>();
    convert_with_int<bbi::Signed, 32, bbi::Saturate, std::uint32_t>();
    convert_with_int<bbi::Signed, 32, bbi::Saturate, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 32, bbi::Saturate, std::int8_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Saturate, std::int16_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Saturate, std::int32_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Saturate, std::int64_t>();

    convert_with_int<bbi::Unsigned, 32, bbi::Saturate, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Saturate, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Saturate, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Saturate, std::uint64_t>();

    convert_with_int<bbi::Signed, 64, bbi::Saturate, std::int8_t>();
    convert_with_int<bbi::Signed, 64, bbi::Saturate, std::int16_t>();
    convert_with_int<bbi::Signed, 64, bbi::Saturate, std::int32_t>();
    convert_with_int<bbi::Signed, 64, bbi::Saturate, std::int64_t>();

    convert_with_int<bbi::Signed, 64, bbi::Saturate, std::uint8_t>();
    convert_with_int<bbi::Signed, 64, bbi::Saturate, std::uint16_t>();
    convert_with_int<bbi::Signed, 64, bbi::Saturate, std::uint32_t>();
    convert_with_int<bbi::Signed, 64, bbi::Saturate, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 64, bbi::Saturate, std::int8_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Saturate, std::int16_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Saturate, std::int32_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Saturate, std::int64_t>();

    convert_with_int<bbi::Unsigned, 64, bbi::Saturate, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Saturate, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Saturate, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Saturate, std::uint64_t>();


    convert_with_int<bbi::Signed, 8, bbi::Throw, std::int8_t>();
    convert_with_int<bbi::Signed, 8, bbi::Throw, std::int16_t>();
    convert_with_int<bbi::Signed, 8, bbi::Throw, std::int32_t>();
    convert_with_int<bbi::Signed, 8, bbi::Throw, std::int64_t>();

    convert_with_int<bbi::Signed, 8, bbi::Throw, std::uint8_t>();
    convert_with_int<bbi::Signed, 8, bbi::Throw, std::uint16_t>();
    convert_with_int<bbi::Signed, 8, bbi::Throw, std::uint32_t>();
    convert_with_int<bbi::Signed, 8, bbi::Throw, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 8, bbi::Throw, std::int8_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Throw, std::int16_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Throw, std::int32_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Throw, std::int64_t>();

    convert_with_int<bbi::Unsigned, 8, bbi::Throw, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Throw, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Throw, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 8, bbi::Throw, std::uint64_t>();

    convert_with_int<bbi::Signed, 16, bbi::Throw, std::int8_t>();
    convert_with_int<bbi::Signed, 16, bbi::Throw, std::int16_t>();
    convert_with_int<bbi::Signed, 16, bbi::Throw, std::int32_t>();
    convert_with_int<bbi::Signed, 16, bbi::Throw, std::int64_t>();

    convert_with_int<bbi::Signed, 16, bbi::Throw, std::uint8_t>();
    convert_with_int<bbi::Signed, 16, bbi::Throw, std::uint16_t>();
    convert_with_int<bbi::Signed, 16, bbi::Throw, std::uint32_t>();
    convert_with_int<bbi::Signed, 16, bbi::Throw, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 16, bbi::Throw, std::int8_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Throw, std::int16_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Throw, std::int32_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Throw, std::int64_t>();

    convert_with_int<bbi::Unsigned, 16, bbi::Throw, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Throw, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Throw, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 16, bbi::Throw, std::uint64_t>();

    convert_with_int<bbi::Signed, 32, bbi::Throw, std::int8_t>();
    convert_with_int<bbi::Signed, 32, bbi::Throw, std::int16_t>();
    convert_with_int<bbi::Signed, 32, bbi::Throw, std::int32_t>();
    convert_with_int<bbi::Signed, 32, bbi::Throw, std::int64_t>();

    convert_with_int<bbi::Signed, 32, bbi::Throw, std::uint8_t>();
    convert_with_int<bbi::Signed, 32, bbi::Throw, std::uint16_t>();
    convert_with_int<bbi::Signed, 32, bbi::Throw, std::uint32_t>();
    convert_with_int<bbi::Signed, 32, bbi::Throw, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 32, bbi::Throw, std::int8_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Throw, std::int16_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Throw, std::int32_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Throw, std::int64_t>();

    convert_with_int<bbi::Unsigned, 32, bbi::Throw, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Throw, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Throw, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 32, bbi::Throw, std::uint64_t>();

    convert_with_int<bbi::Signed, 64, bbi::Throw, std::int8_t>();
    convert_with_int<bbi::Signed, 64, bbi::Throw, std::int16_t>();
    convert_with_int<bbi::Signed, 64, bbi::Throw, std::int32_t>();
    convert_with_int<bbi::Signed, 64, bbi::Throw, std::int64_t>();

    convert_with_int<bbi::Signed, 64, bbi::Throw, std::uint8_t>();
    convert_with_int<bbi::Signed, 64, bbi::Throw, std::uint16_t>();
    convert_with_int<bbi::Signed, 64, bbi::Throw, std::uint32_t>();
    convert_with_int<bbi::Signed, 64, bbi::Throw, std::uint64_t>();

    convert_with_int<bbi::Unsigned, 64, bbi::Throw, std::int8_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Throw, std::int16_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Throw, std::int32_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Throw, std::int64_t>();

    convert_with_int<bbi::Unsigned, 64, bbi::Throw, std::uint8_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Throw, std::uint16_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Throw, std::uint32_t>();
    convert_with_int<bbi::Unsigned, 64, bbi::Throw, std::uint64_t>();


    convert_from_Z<bbi::wrap::i8, bbi::wrap::i8>();
    convert_from_Z<bbi::wrap::i8, bbi::wrap::i16>();
    convert_from_Z<bbi::wrap::i8, bbi::wrap::i32>();
    convert_from_Z<bbi::wrap::i8, bbi::wrap::i64>();

    convert_from_Z<bbi::wrap::i8, bbi::wrap::u8>();
    convert_from_Z<bbi::wrap::i8, bbi::wrap::u16>();
    convert_from_Z<bbi::wrap::i8, bbi::wrap::u32>();
    convert_from_Z<bbi::wrap::i8, bbi::wrap::u64>();

    convert_from_Z<bbi::wrap::i8, bbi::sat::i8>();
    convert_from_Z<bbi::wrap::i8, bbi::sat::i16>();
    convert_from_Z<bbi::wrap::i8, bbi::sat::i32>();
    convert_from_Z<bbi::wrap::i8, bbi::sat::i64>();

    convert_from_Z<bbi::wrap::i8, bbi::sat::u8>();
    convert_from_Z<bbi::wrap::i8, bbi::sat::u16>();
    convert_from_Z<bbi::wrap::i8, bbi::sat::u32>();
    convert_from_Z<bbi::wrap::i8, bbi::sat::u64>();

    convert_from_Z<bbi::wrap::i8, bbi::thrw::i8>();
    convert_from_Z<bbi::wrap::i8, bbi::thrw::i16>();
    convert_from_Z<bbi::wrap::i8, bbi::thrw::i32>();
    convert_from_Z<bbi::wrap::i8, bbi::thrw::i64>();

    convert_from_Z<bbi::wrap::i8, bbi::thrw::u8>();
    convert_from_Z<bbi::wrap::i8, bbi::thrw::u16>();
    convert_from_Z<bbi::wrap::i8, bbi::thrw::u32>();
    convert_from_Z<bbi::wrap::i8, bbi::thrw::u64>();

    convert_from_Z<bbi::wrap::i16, bbi::wrap::i8>();
    convert_from_Z<bbi::wrap::i16, bbi::wrap::i16>();
    convert_from_Z<bbi::wrap::i16, bbi::wrap::i32>();
    convert_from_Z<bbi::wrap::i16, bbi::wrap::i64>();

    convert_from_Z<bbi::wrap::i16, bbi::wrap::u8>();
    convert_from_Z<bbi::wrap::i16, bbi::wrap::u16>();
    convert_from_Z<bbi::wrap::i16, bbi::wrap::u32>();
    convert_from_Z<bbi::wrap::i16, bbi::wrap::u64>();

    convert_from_Z<bbi::wrap::i16, bbi::sat::i8>();
    convert_from_Z<bbi::wrap::i16, bbi::sat::i16>();
    convert_from_Z<bbi::wrap::i16, bbi::sat::i32>();
    convert_from_Z<bbi::wrap::i16, bbi::sat::i64>();

    convert_from_Z<bbi::wrap::i16, bbi::sat::u8>();
    convert_from_Z<bbi::wrap::i16, bbi::sat::u16>();
    convert_from_Z<bbi::wrap::i16, bbi::sat::u32>();
    convert_from_Z<bbi::wrap::i16, bbi::sat::u64>();

    convert_from_Z<bbi::wrap::i16, bbi::thrw::i8>();
    convert_from_Z<bbi::wrap::i16, bbi::thrw::i16>();
    convert_from_Z<bbi::wrap::i16, bbi::thrw::i32>();
    convert_from_Z<bbi::wrap::i16, bbi::thrw::i64>();

    convert_from_Z<bbi::wrap::i16, bbi::thrw::u8>();
    convert_from_Z<bbi::wrap::i16, bbi::thrw::u16>();
    convert_from_Z<bbi::wrap::i16, bbi::thrw::u32>();
    convert_from_Z<bbi::wrap::i16, bbi::thrw::u64>();

    convert_from_Z<bbi::wrap::i32, bbi::wrap::i8>();
    convert_from_Z<bbi::wrap::i32, bbi::wrap::i16>();
    convert_from_Z<bbi::wrap::i32, bbi::wrap::i32>();
    convert_from_Z<bbi::wrap::i32, bbi::wrap::i64>();

    convert_from_Z<bbi::wrap::i32, bbi::wrap::u8>();
    convert_from_Z<bbi::wrap::i32, bbi::wrap::u16>();
    convert_from_Z<bbi::wrap::i32, bbi::wrap::u32>();
    convert_from_Z<bbi::wrap::i32, bbi::wrap::u64>();

    convert_from_Z<bbi::wrap::i32, bbi::sat::i8>();
    convert_from_Z<bbi::wrap::i32, bbi::sat::i16>();
    convert_from_Z<bbi::wrap::i32, bbi::sat::i32>();
    convert_from_Z<bbi::wrap::i32, bbi::sat::i64>();

    convert_from_Z<bbi::wrap::i32, bbi::sat::u8>();
    convert_from_Z<bbi::wrap::i32, bbi::sat::u16>();
    convert_from_Z<bbi::wrap::i32, bbi::sat::u32>();
    convert_from_Z<bbi::wrap::i32, bbi::sat::u64>();

    convert_from_Z<bbi::wrap::i32, bbi::thrw::i8>();
    convert_from_Z<bbi::wrap::i32, bbi::thrw::i16>();
    convert_from_Z<bbi::wrap::i32, bbi::thrw::i32>();
    convert_from_Z<bbi::wrap::i32, bbi::thrw::i64>();

    convert_from_Z<bbi::wrap::i32, bbi::thrw::u8>();
    convert_from_Z<bbi::wrap::i32, bbi::thrw::u16>();
    convert_from_Z<bbi::wrap::i32, bbi::thrw::u32>();
    convert_from_Z<bbi::wrap::i32, bbi::thrw::u64>();

    convert_from_Z<bbi::wrap::i64, bbi::wrap::i8>();
    convert_from_Z<bbi::wrap::i64, bbi::wrap::i16>();
    convert_from_Z<bbi::wrap::i64, bbi::wrap::i32>();
    convert_from_Z<bbi::wrap::i64, bbi::wrap::i64>();

    convert_from_Z<bbi::wrap::i64, bbi::wrap::u8>();
    convert_from_Z<bbi::wrap::i64, bbi::wrap::u16>();
    convert_from_Z<bbi::wrap::i64, bbi::wrap::u32>();
    convert_from_Z<bbi::wrap::i64, bbi::wrap::u64>();

    convert_from_Z<bbi::wrap::i64, bbi::sat::i8>();
    convert_from_Z<bbi::wrap::i64, bbi::sat::i16>();
    convert_from_Z<bbi::wrap::i64, bbi::sat::i32>();
    convert_from_Z<bbi::wrap::i64, bbi::sat::i64>();

    convert_from_Z<bbi::wrap::i64, bbi::sat::u8>();
    convert_from_Z<bbi::wrap::i64, bbi::sat::u16>();
    convert_from_Z<bbi::wrap::i64, bbi::sat::u32>();
    convert_from_Z<bbi::wrap::i64, bbi::sat::u64>();

    convert_from_Z<bbi::wrap::i64, bbi::thrw::i8>();
    convert_from_Z<bbi::wrap::i64, bbi::thrw::i16>();
    convert_from_Z<bbi::wrap::i64, bbi::thrw::i32>();
    convert_from_Z<bbi::wrap::i64, bbi::thrw::i64>();

    convert_from_Z<bbi::wrap::i64, bbi::thrw::u8>();
    convert_from_Z<bbi::wrap::i64, bbi::thrw::u16>();
    convert_from_Z<bbi::wrap::i64, bbi::thrw::u32>();
    convert_from_Z<bbi::wrap::i64, bbi::thrw::u64>();

    convert_from_Z<bbi::wrap::u8, bbi::wrap::i8>();
    convert_from_Z<bbi::wrap::u8, bbi::wrap::i16>();
    convert_from_Z<bbi::wrap::u8, bbi::wrap::i32>();
    convert_from_Z<bbi::wrap::u8, bbi::wrap::i64>();

    convert_from_Z<bbi::wrap::u8, bbi::wrap::u8>();
    convert_from_Z<bbi::wrap::u8, bbi::wrap::u16>();
    convert_from_Z<bbi::wrap::u8, bbi::wrap::u32>();
    convert_from_Z<bbi::wrap::u8, bbi::wrap::u64>();

    convert_from_Z<bbi::wrap::u8, bbi::sat::i8>();
    convert_from_Z<bbi::wrap::u8, bbi::sat::i16>();
    convert_from_Z<bbi::wrap::u8, bbi::sat::i32>();
    convert_from_Z<bbi::wrap::u8, bbi::sat::i64>();

    convert_from_Z<bbi::wrap::u8, bbi::sat::u8>();
    convert_from_Z<bbi::wrap::u8, bbi::sat::u16>();
    convert_from_Z<bbi::wrap::u8, bbi::sat::u32>();
    convert_from_Z<bbi::wrap::u8, bbi::sat::u64>();

    convert_from_Z<bbi::wrap::u8, bbi::thrw::i8>();
    convert_from_Z<bbi::wrap::u8, bbi::thrw::i16>();
    convert_from_Z<bbi::wrap::u8, bbi::thrw::i32>();
    convert_from_Z<bbi::wrap::u8, bbi::thrw::i64>();

    convert_from_Z<bbi::wrap::u8, bbi::thrw::u8>();
    convert_from_Z<bbi::wrap::u8, bbi::thrw::u16>();
    convert_from_Z<bbi::wrap::u8, bbi::thrw::u32>();
    convert_from_Z<bbi::wrap::u8, bbi::thrw::u64>();

    convert_from_Z<bbi::wrap::u16, bbi::wrap::i8>();
    convert_from_Z<bbi::wrap::u16, bbi::wrap::i16>();
    convert_from_Z<bbi::wrap::u16, bbi::wrap::i32>();
    convert_from_Z<bbi::wrap::u16, bbi::wrap::i64>();

    convert_from_Z<bbi::wrap::u16, bbi::wrap::u8>();
    convert_from_Z<bbi::wrap::u16, bbi::wrap::u16>();
    convert_from_Z<bbi::wrap::u16, bbi::wrap::u32>();
    convert_from_Z<bbi::wrap::u16, bbi::wrap::u64>();

    convert_from_Z<bbi::wrap::u16, bbi::sat::i8>();
    convert_from_Z<bbi::wrap::u16, bbi::sat::i16>();
    convert_from_Z<bbi::wrap::u16, bbi::sat::i32>();
    convert_from_Z<bbi::wrap::u16, bbi::sat::i64>();

    convert_from_Z<bbi::wrap::u16, bbi::sat::u8>();
    convert_from_Z<bbi::wrap::u16, bbi::sat::u16>();
    convert_from_Z<bbi::wrap::u16, bbi::sat::u32>();
    convert_from_Z<bbi::wrap::u16, bbi::sat::u64>();

    convert_from_Z<bbi::wrap::u16, bbi::thrw::i8>();
    convert_from_Z<bbi::wrap::u16, bbi::thrw::i16>();
    convert_from_Z<bbi::wrap::u16, bbi::thrw::i32>();
    convert_from_Z<bbi::wrap::u16, bbi::thrw::i64>();

    convert_from_Z<bbi::wrap::u16, bbi::thrw::u8>();
    convert_from_Z<bbi::wrap::u16, bbi::thrw::u16>();
    convert_from_Z<bbi::wrap::u16, bbi::thrw::u32>();
    convert_from_Z<bbi::wrap::u16, bbi::thrw::u64>();

    convert_from_Z<bbi::wrap::u32, bbi::wrap::i8>();
    convert_from_Z<bbi::wrap::u32, bbi::wrap::i16>();
    convert_from_Z<bbi::wrap::u32, bbi::wrap::i32>();
    convert_from_Z<bbi::wrap::u32, bbi::wrap::i64>();

    convert_from_Z<bbi::wrap::u32, bbi::wrap::u8>();
    convert_from_Z<bbi::wrap::u32, bbi::wrap::u16>();
    convert_from_Z<bbi::wrap::u32, bbi::wrap::u32>();
    convert_from_Z<bbi::wrap::u32, bbi::wrap::u64>();

    convert_from_Z<bbi::wrap::u32, bbi::sat::i8>();
    convert_from_Z<bbi::wrap::u32, bbi::sat::i16>();
    convert_from_Z<bbi::wrap::u32, bbi::sat::i32>();
    convert_from_Z<bbi::wrap::u32, bbi::sat::i64>();

    convert_from_Z<bbi::wrap::u32, bbi::sat::u8>();
    convert_from_Z<bbi::wrap::u32, bbi::sat::u16>();
    convert_from_Z<bbi::wrap::u32, bbi::sat::u32>();
    convert_from_Z<bbi::wrap::u32, bbi::sat::u64>();

    convert_from_Z<bbi::wrap::u32, bbi::thrw::i8>();
    convert_from_Z<bbi::wrap::u32, bbi::thrw::i16>();
    convert_from_Z<bbi::wrap::u32, bbi::thrw::i32>();
    convert_from_Z<bbi::wrap::u32, bbi::thrw::i64>();

    convert_from_Z<bbi::wrap::u32, bbi::thrw::u8>();
    convert_from_Z<bbi::wrap::u32, bbi::thrw::u16>();
    convert_from_Z<bbi::wrap::u32, bbi::thrw::u32>();
    convert_from_Z<bbi::wrap::u32, bbi::thrw::u64>();

    convert_from_Z<bbi::wrap::u64, bbi::wrap::i8>();
    convert_from_Z<bbi::wrap::u64, bbi::wrap::i16>();
    convert_from_Z<bbi::wrap::u64, bbi::wrap::i32>();
    convert_from_Z<bbi::wrap::u64, bbi::wrap::i64>();

    convert_from_Z<bbi::wrap::u64, bbi::wrap::u8>();
    convert_from_Z<bbi::wrap::u64, bbi::wrap::u16>();
    convert_from_Z<bbi::wrap::u64, bbi::wrap::u32>();
    convert_from_Z<bbi::wrap::u64, bbi::wrap::u64>();

    convert_from_Z<bbi::wrap::u64, bbi::sat::i8>();
    convert_from_Z<bbi::wrap::u64, bbi::sat::i16>();
    convert_from_Z<bbi::wrap::u64, bbi::sat::i32>();
    convert_from_Z<bbi::wrap::u64, bbi::sat::i64>();

    convert_from_Z<bbi::wrap::u64, bbi::sat::u8>();
    convert_from_Z<bbi::wrap::u64, bbi::sat::u16>();
    convert_from_Z<bbi::wrap::u64, bbi::sat::u32>();
    convert_from_Z<bbi::wrap::u64, bbi::sat::u64>();

    convert_from_Z<bbi::wrap::u64, bbi::thrw::i8>();
    convert_from_Z<bbi::wrap::u64, bbi::thrw::i16>();
    convert_from_Z<bbi::wrap::u64, bbi::thrw::i32>();
    convert_from_Z<bbi::wrap::u64, bbi::thrw::i64>();

    convert_from_Z<bbi::wrap::u64, bbi::thrw::u8>();
    convert_from_Z<bbi::wrap::u64, bbi::thrw::u16>();
    convert_from_Z<bbi::wrap::u64, bbi::thrw::u32>();
    convert_from_Z<bbi::wrap::u64, bbi::thrw::u64>();


    convert_from_Z<bbi::sat::i8, bbi::wrap::i8>();
    convert_from_Z<bbi::sat::i8, bbi::wrap::i16>();
    convert_from_Z<bbi::sat::i8, bbi::wrap::i32>();
    convert_from_Z<bbi::sat::i8, bbi::wrap::i64>();

    convert_from_Z<bbi::sat::i8, bbi::wrap::u8>();
    convert_from_Z<bbi::sat::i8, bbi::wrap::u16>();
    convert_from_Z<bbi::sat::i8, bbi::wrap::u32>();
    convert_from_Z<bbi::sat::i8, bbi::wrap::u64>();

    convert_from_Z<bbi::sat::i8, bbi::sat::i8>();
    convert_from_Z<bbi::sat::i8, bbi::sat::i16>();
    convert_from_Z<bbi::sat::i8, bbi::sat::i32>();
    convert_from_Z<bbi::sat::i8, bbi::sat::i64>();

    convert_from_Z<bbi::sat::i8, bbi::sat::u8>();
    convert_from_Z<bbi::sat::i8, bbi::sat::u16>();
    convert_from_Z<bbi::sat::i8, bbi::sat::u32>();
    convert_from_Z<bbi::sat::i8, bbi::sat::u64>();

    convert_from_Z<bbi::sat::i8, bbi::thrw::i8>();
    convert_from_Z<bbi::sat::i8, bbi::thrw::i16>();
    convert_from_Z<bbi::sat::i8, bbi::thrw::i32>();
    convert_from_Z<bbi::sat::i8, bbi::thrw::i64>();

    convert_from_Z<bbi::sat::i8, bbi::thrw::u8>();
    convert_from_Z<bbi::sat::i8, bbi::thrw::u16>();
    convert_from_Z<bbi::sat::i8, bbi::thrw::u32>();
    convert_from_Z<bbi::sat::i8, bbi::thrw::u64>();

    convert_from_Z<bbi::sat::i16, bbi::wrap::i8>();
    convert_from_Z<bbi::sat::i16, bbi::wrap::i16>();
    convert_from_Z<bbi::sat::i16, bbi::wrap::i32>();
    convert_from_Z<bbi::sat::i16, bbi::wrap::i64>();

    convert_from_Z<bbi::sat::i16, bbi::wrap::u8>();
    convert_from_Z<bbi::sat::i16, bbi::wrap::u16>();
    convert_from_Z<bbi::sat::i16, bbi::wrap::u32>();
    convert_from_Z<bbi::sat::i16, bbi::wrap::u64>();

    convert_from_Z<bbi::sat::i16, bbi::sat::i8>();
    convert_from_Z<bbi::sat::i16, bbi::sat::i16>();
    convert_from_Z<bbi::sat::i16, bbi::sat::i32>();
    convert_from_Z<bbi::sat::i16, bbi::sat::i64>();

    convert_from_Z<bbi::sat::i16, bbi::sat::u8>();
    convert_from_Z<bbi::sat::i16, bbi::sat::u16>();
    convert_from_Z<bbi::sat::i16, bbi::sat::u32>();
    convert_from_Z<bbi::sat::i16, bbi::sat::u64>();

    convert_from_Z<bbi::sat::i16, bbi::thrw::i8>();
    convert_from_Z<bbi::sat::i16, bbi::thrw::i16>();
    convert_from_Z<bbi::sat::i16, bbi::thrw::i32>();
    convert_from_Z<bbi::sat::i16, bbi::thrw::i64>();

    convert_from_Z<bbi::sat::i16, bbi::thrw::u8>();
    convert_from_Z<bbi::sat::i16, bbi::thrw::u16>();
    convert_from_Z<bbi::sat::i16, bbi::thrw::u32>();
    convert_from_Z<bbi::sat::i16, bbi::thrw::u64>();

    convert_from_Z<bbi::sat::i32, bbi::wrap::i8>();
    convert_from_Z<bbi::sat::i32, bbi::wrap::i16>();
    convert_from_Z<bbi::sat::i32, bbi::wrap::i32>();
    convert_from_Z<bbi::sat::i32, bbi::wrap::i64>();

    convert_from_Z<bbi::sat::i32, bbi::wrap::u8>();
    convert_from_Z<bbi::sat::i32, bbi::wrap::u16>();
    convert_from_Z<bbi::sat::i32, bbi::wrap::u32>();
    convert_from_Z<bbi::sat::i32, bbi::wrap::u64>();

    convert_from_Z<bbi::sat::i32, bbi::sat::i8>();
    convert_from_Z<bbi::sat::i32, bbi::sat::i16>();
    convert_from_Z<bbi::sat::i32, bbi::sat::i32>();
    convert_from_Z<bbi::sat::i32, bbi::sat::i64>();

    convert_from_Z<bbi::sat::i32, bbi::sat::u8>();
    convert_from_Z<bbi::sat::i32, bbi::sat::u16>();
    convert_from_Z<bbi::sat::i32, bbi::sat::u32>();
    convert_from_Z<bbi::sat::i32, bbi::sat::u64>();

    convert_from_Z<bbi::sat::i32, bbi::thrw::i8>();
    convert_from_Z<bbi::sat::i32, bbi::thrw::i16>();
    convert_from_Z<bbi::sat::i32, bbi::thrw::i32>();
    convert_from_Z<bbi::sat::i32, bbi::thrw::i64>();

    convert_from_Z<bbi::sat::i32, bbi::thrw::u8>();
    convert_from_Z<bbi::sat::i32, bbi::thrw::u16>();
    convert_from_Z<bbi::sat::i32, bbi::thrw::u32>();
    convert_from_Z<bbi::sat::i32, bbi::thrw::u64>();

    convert_from_Z<bbi::sat::i64, bbi::wrap::i8>();
    convert_from_Z<bbi::sat::i64, bbi::wrap::i16>();
    convert_from_Z<bbi::sat::i64, bbi::wrap::i32>();
    convert_from_Z<bbi::sat::i64, bbi::wrap::i64>();

    convert_from_Z<bbi::sat::i64, bbi::wrap::u8>();
    convert_from_Z<bbi::sat::i64, bbi::wrap::u16>();
    convert_from_Z<bbi::sat::i64, bbi::wrap::u32>();
    convert_from_Z<bbi::sat::i64, bbi::wrap::u64>();

    convert_from_Z<bbi::sat::i64, bbi::sat::i8>();
    convert_from_Z<bbi::sat::i64, bbi::sat::i16>();
    convert_from_Z<bbi::sat::i64, bbi::sat::i32>();
    convert_from_Z<bbi::sat::i64, bbi::sat::i64>();

    convert_from_Z<bbi::sat::i64, bbi::sat::u8>();
    convert_from_Z<bbi::sat::i64, bbi::sat::u16>();
    convert_from_Z<bbi::sat::i64, bbi::sat::u32>();
    convert_from_Z<bbi::sat::i64, bbi::sat::u64>();

    convert_from_Z<bbi::sat::i64, bbi::thrw::i8>();
    convert_from_Z<bbi::sat::i64, bbi::thrw::i16>();
    convert_from_Z<bbi::sat::i64, bbi::thrw::i32>();
    convert_from_Z<bbi::sat::i64, bbi::thrw::i64>();

    convert_from_Z<bbi::sat::i64, bbi::thrw::u8>();
    convert_from_Z<bbi::sat::i64, bbi::thrw::u16>();
    convert_from_Z<bbi::sat::i64, bbi::thrw::u32>();
    convert_from_Z<bbi::sat::i64, bbi::thrw::u64>();

    convert_from_Z<bbi::sat::u8, bbi::wrap::i8>();
    convert_from_Z<bbi::sat::u8, bbi::wrap::i16>();
    convert_from_Z<bbi::sat::u8, bbi::wrap::i32>();
    convert_from_Z<bbi::sat::u8, bbi::wrap::i64>();

    convert_from_Z<bbi::sat::u8, bbi::wrap::u8>();
    convert_from_Z<bbi::sat::u8, bbi::wrap::u16>();
    convert_from_Z<bbi::sat::u8, bbi::wrap::u32>();
    convert_from_Z<bbi::sat::u8, bbi::wrap::u64>();

    convert_from_Z<bbi::sat::u8, bbi::sat::i8>();
    convert_from_Z<bbi::sat::u8, bbi::sat::i16>();
    convert_from_Z<bbi::sat::u8, bbi::sat::i32>();
    convert_from_Z<bbi::sat::u8, bbi::sat::i64>();

    convert_from_Z<bbi::sat::u8, bbi::sat::u8>();
    convert_from_Z<bbi::sat::u8, bbi::sat::u16>();
    convert_from_Z<bbi::sat::u8, bbi::sat::u32>();
    convert_from_Z<bbi::sat::u8, bbi::sat::u64>();

    convert_from_Z<bbi::sat::u8, bbi::thrw::i8>();
    convert_from_Z<bbi::sat::u8, bbi::thrw::i16>();
    convert_from_Z<bbi::sat::u8, bbi::thrw::i32>();
    convert_from_Z<bbi::sat::u8, bbi::thrw::i64>();

    convert_from_Z<bbi::sat::u8, bbi::thrw::u8>();
    convert_from_Z<bbi::sat::u8, bbi::thrw::u16>();
    convert_from_Z<bbi::sat::u8, bbi::thrw::u32>();
    convert_from_Z<bbi::sat::u8, bbi::thrw::u64>();

    convert_from_Z<bbi::sat::u16, bbi::wrap::i8>();
    convert_from_Z<bbi::sat::u16, bbi::wrap::i16>();
    convert_from_Z<bbi::sat::u16, bbi::wrap::i32>();
    convert_from_Z<bbi::sat::u16, bbi::wrap::i64>();

    convert_from_Z<bbi::sat::u16, bbi::wrap::u8>();
    convert_from_Z<bbi::sat::u16, bbi::wrap::u16>();
    convert_from_Z<bbi::sat::u16, bbi::wrap::u32>();
    convert_from_Z<bbi::sat::u16, bbi::wrap::u64>();

    convert_from_Z<bbi::sat::u16, bbi::sat::i8>();
    convert_from_Z<bbi::sat::u16, bbi::sat::i16>();
    convert_from_Z<bbi::sat::u16, bbi::sat::i32>();
    convert_from_Z<bbi::sat::u16, bbi::sat::i64>();

    convert_from_Z<bbi::sat::u16, bbi::sat::u8>();
    convert_from_Z<bbi::sat::u16, bbi::sat::u16>();
    convert_from_Z<bbi::sat::u16, bbi::sat::u32>();
    convert_from_Z<bbi::sat::u16, bbi::sat::u64>();

    convert_from_Z<bbi::sat::u16, bbi::thrw::i8>();
    convert_from_Z<bbi::sat::u16, bbi::thrw::i16>();
    convert_from_Z<bbi::sat::u16, bbi::thrw::i32>();
    convert_from_Z<bbi::sat::u16, bbi::thrw::i64>();

    convert_from_Z<bbi::sat::u16, bbi::thrw::u8>();
    convert_from_Z<bbi::sat::u16, bbi::thrw::u16>();
    convert_from_Z<bbi::sat::u16, bbi::thrw::u32>();
    convert_from_Z<bbi::sat::u16, bbi::thrw::u64>();

    convert_from_Z<bbi::sat::u32, bbi::wrap::i8>();
    convert_from_Z<bbi::sat::u32, bbi::wrap::i16>();
    convert_from_Z<bbi::sat::u32, bbi::wrap::i32>();
    convert_from_Z<bbi::sat::u32, bbi::wrap::i64>();

    convert_from_Z<bbi::sat::u32, bbi::wrap::u8>();
    convert_from_Z<bbi::sat::u32, bbi::wrap::u16>();
    convert_from_Z<bbi::sat::u32, bbi::wrap::u32>();
    convert_from_Z<bbi::sat::u32, bbi::wrap::u64>();

    convert_from_Z<bbi::sat::u32, bbi::sat::i8>();
    convert_from_Z<bbi::sat::u32, bbi::sat::i16>();
    convert_from_Z<bbi::sat::u32, bbi::sat::i32>();
    convert_from_Z<bbi::sat::u32, bbi::sat::i64>();

    convert_from_Z<bbi::sat::u32, bbi::sat::u8>();
    convert_from_Z<bbi::sat::u32, bbi::sat::u16>();
    convert_from_Z<bbi::sat::u32, bbi::sat::u32>();
    convert_from_Z<bbi::sat::u32, bbi::sat::u64>();

    convert_from_Z<bbi::sat::u32, bbi::thrw::i8>();
    convert_from_Z<bbi::sat::u32, bbi::thrw::i16>();
    convert_from_Z<bbi::sat::u32, bbi::thrw::i32>();
    convert_from_Z<bbi::sat::u32, bbi::thrw::i64>();

    convert_from_Z<bbi::sat::u32, bbi::thrw::u8>();
    convert_from_Z<bbi::sat::u32, bbi::thrw::u16>();
    convert_from_Z<bbi::sat::u32, bbi::thrw::u32>();
    convert_from_Z<bbi::sat::u32, bbi::thrw::u64>();

    convert_from_Z<bbi::sat::u64, bbi::wrap::i8>();
    convert_from_Z<bbi::sat::u64, bbi::wrap::i16>();
    convert_from_Z<bbi::sat::u64, bbi::wrap::i32>();
    convert_from_Z<bbi::sat::u64, bbi::wrap::i64>();

    convert_from_Z<bbi::sat::u64, bbi::wrap::u8>();
    convert_from_Z<bbi::sat::u64, bbi::wrap::u16>();
    convert_from_Z<bbi::sat::u64, bbi::wrap::u32>();
    convert_from_Z<bbi::sat::u64, bbi::wrap::u64>();

    convert_from_Z<bbi::sat::u64, bbi::sat::i8>();
    convert_from_Z<bbi::sat::u64, bbi::sat::i16>();
    convert_from_Z<bbi::sat::u64, bbi::sat::i32>();
    convert_from_Z<bbi::sat::u64, bbi::sat::i64>();

    convert_from_Z<bbi::sat::u64, bbi::sat::u8>();
    convert_from_Z<bbi::sat::u64, bbi::sat::u16>();
    convert_from_Z<bbi::sat::u64, bbi::sat::u32>();
    convert_from_Z<bbi::sat::u64, bbi::sat::u64>();

    convert_from_Z<bbi::sat::u64, bbi::thrw::i8>();
    convert_from_Z<bbi::sat::u64, bbi::thrw::i16>();
    convert_from_Z<bbi::sat::u64, bbi::thrw::i32>();
    convert_from_Z<bbi::sat::u64, bbi::thrw::i64>();

    convert_from_Z<bbi::sat::u64, bbi::thrw::u8>();
    convert_from_Z<bbi::sat::u64, bbi::thrw::u16>();
    convert_from_Z<bbi::sat::u64, bbi::thrw::u32>();
    convert_from_Z<bbi::sat::u64, bbi::thrw::u64>();


    convert_from_Z<bbi::thrw::i8, bbi::wrap::i8>();
    convert_from_Z<bbi::thrw::i8, bbi::wrap::i16>();
    convert_from_Z<bbi::thrw::i8, bbi::wrap::i32>();
    convert_from_Z<bbi::thrw::i8, bbi::wrap::i64>();

    convert_from_Z<bbi::thrw::i8, bbi::wrap::u8>();
    convert_from_Z<bbi::thrw::i8, bbi::wrap::u16>();
    convert_from_Z<bbi::thrw::i8, bbi::wrap::u32>();
    convert_from_Z<bbi::thrw::i8, bbi::wrap::u64>();

    convert_from_Z<bbi::thrw::i8, bbi::sat::i8>();
    convert_from_Z<bbi::thrw::i8, bbi::sat::i16>();
    convert_from_Z<bbi::thrw::i8, bbi::sat::i32>();
    convert_from_Z<bbi::thrw::i8, bbi::sat::i64>();

    convert_from_Z<bbi::thrw::i8, bbi::sat::u8>();
    convert_from_Z<bbi::thrw::i8, bbi::sat::u16>();
    convert_from_Z<bbi::thrw::i8, bbi::sat::u32>();
    convert_from_Z<bbi::thrw::i8, bbi::sat::u64>();

    convert_from_Z<bbi::thrw::i8, bbi::thrw::i8>();
    convert_from_Z<bbi::thrw::i8, bbi::thrw::i16>();
    convert_from_Z<bbi::thrw::i8, bbi::thrw::i32>();
    convert_from_Z<bbi::thrw::i8, bbi::thrw::i64>();

    convert_from_Z<bbi::thrw::i8, bbi::thrw::u8>();
    convert_from_Z<bbi::thrw::i8, bbi::thrw::u16>();
    convert_from_Z<bbi::thrw::i8, bbi::thrw::u32>();
    convert_from_Z<bbi::thrw::i8, bbi::thrw::u64>();

    convert_from_Z<bbi::thrw::i16, bbi::wrap::i8>();
    convert_from_Z<bbi::thrw::i16, bbi::wrap::i16>();
    convert_from_Z<bbi::thrw::i16, bbi::wrap::i32>();
    convert_from_Z<bbi::thrw::i16, bbi::wrap::i64>();

    convert_from_Z<bbi::thrw::i16, bbi::wrap::u8>();
    convert_from_Z<bbi::thrw::i16, bbi::wrap::u16>();
    convert_from_Z<bbi::thrw::i16, bbi::wrap::u32>();
    convert_from_Z<bbi::thrw::i16, bbi::wrap::u64>();

    convert_from_Z<bbi::thrw::i16, bbi::sat::i8>();
    convert_from_Z<bbi::thrw::i16, bbi::sat::i16>();
    convert_from_Z<bbi::thrw::i16, bbi::sat::i32>();
    convert_from_Z<bbi::thrw::i16, bbi::sat::i64>();

    convert_from_Z<bbi::thrw::i16, bbi::sat::u8>();
    convert_from_Z<bbi::thrw::i16, bbi::sat::u16>();
    convert_from_Z<bbi::thrw::i16, bbi::sat::u32>();
    convert_from_Z<bbi::thrw::i16, bbi::sat::u64>();

    convert_from_Z<bbi::thrw::i16, bbi::thrw::i8>();
    convert_from_Z<bbi::thrw::i16, bbi::thrw::i16>();
    convert_from_Z<bbi::thrw::i16, bbi::thrw::i32>();
    convert_from_Z<bbi::thrw::i16, bbi::thrw::i64>();

    convert_from_Z<bbi::thrw::i16, bbi::thrw::u8>();
    convert_from_Z<bbi::thrw::i16, bbi::thrw::u16>();
    convert_from_Z<bbi::thrw::i16, bbi::thrw::u32>();
    convert_from_Z<bbi::thrw::i16, bbi::thrw::u64>();

    convert_from_Z<bbi::thrw::i32, bbi::wrap::i8>();
    convert_from_Z<bbi::thrw::i32, bbi::wrap::i16>();
    convert_from_Z<bbi::thrw::i32, bbi::wrap::i32>();
    convert_from_Z<bbi::thrw::i32, bbi::wrap::i64>();

    convert_from_Z<bbi::thrw::i32, bbi::wrap::u8>();
    convert_from_Z<bbi::thrw::i32, bbi::wrap::u16>();
    convert_from_Z<bbi::thrw::i32, bbi::wrap::u32>();
    convert_from_Z<bbi::thrw::i32, bbi::wrap::u64>();

    convert_from_Z<bbi::thrw::i32, bbi::sat::i8>();
    convert_from_Z<bbi::thrw::i32, bbi::sat::i16>();
    convert_from_Z<bbi::thrw::i32, bbi::sat::i32>();
    convert_from_Z<bbi::thrw::i32, bbi::sat::i64>();

    convert_from_Z<bbi::thrw::i32, bbi::sat::u8>();
    convert_from_Z<bbi::thrw::i32, bbi::sat::u16>();
    convert_from_Z<bbi::thrw::i32, bbi::sat::u32>();
    convert_from_Z<bbi::thrw::i32, bbi::sat::u64>();

    convert_from_Z<bbi::thrw::i32, bbi::thrw::i8>();
    convert_from_Z<bbi::thrw::i32, bbi::thrw::i16>();
    convert_from_Z<bbi::thrw::i32, bbi::thrw::i32>();
    convert_from_Z<bbi::thrw::i32, bbi::thrw::i64>();

    convert_from_Z<bbi::thrw::i32, bbi::thrw::u8>();
    convert_from_Z<bbi::thrw::i32, bbi::thrw::u16>();
    convert_from_Z<bbi::thrw::i32, bbi::thrw::u32>();
    convert_from_Z<bbi::thrw::i32, bbi::thrw::u64>();

    convert_from_Z<bbi::thrw::i64, bbi::wrap::i8>();
    convert_from_Z<bbi::thrw::i64, bbi::wrap::i16>();
    convert_from_Z<bbi::thrw::i64, bbi::wrap::i32>();
    convert_from_Z<bbi::thrw::i64, bbi::wrap::i64>();

    convert_from_Z<bbi::thrw::i64, bbi::wrap::u8>();
    convert_from_Z<bbi::thrw::i64, bbi::wrap::u16>();
    convert_from_Z<bbi::thrw::i64, bbi::wrap::u32>();
    convert_from_Z<bbi::thrw::i64, bbi::wrap::u64>();

    convert_from_Z<bbi::thrw::i64, bbi::sat::i8>();
    convert_from_Z<bbi::thrw::i64, bbi::sat::i16>();
    convert_from_Z<bbi::thrw::i64, bbi::sat::i32>();
    convert_from_Z<bbi::thrw::i64, bbi::sat::i64>();

    convert_from_Z<bbi::thrw::i64, bbi::sat::u8>();
    convert_from_Z<bbi::thrw::i64, bbi::sat::u16>();
    convert_from_Z<bbi::thrw::i64, bbi::sat::u32>();
    convert_from_Z<bbi::thrw::i64, bbi::sat::u64>();

    convert_from_Z<bbi::thrw::i64, bbi::thrw::i8>();
    convert_from_Z<bbi::thrw::i64, bbi::thrw::i16>();
    convert_from_Z<bbi::thrw::i64, bbi::thrw::i32>();
    convert_from_Z<bbi::thrw::i64, bbi::thrw::i64>();

    convert_from_Z<bbi::thrw::i64, bbi::thrw::u8>();
    convert_from_Z<bbi::thrw::i64, bbi::thrw::u16>();
    convert_from_Z<bbi::thrw::i64, bbi::thrw::u32>();
    convert_from_Z<bbi::thrw::i64, bbi::thrw::u64>();

    convert_from_Z<bbi::thrw::u8, bbi::wrap::i8>();
    convert_from_Z<bbi::thrw::u8, bbi::wrap::i16>();
    convert_from_Z<bbi::thrw::u8, bbi::wrap::i32>();
    convert_from_Z<bbi::thrw::u8, bbi::wrap::i64>();

    convert_from_Z<bbi::thrw::u8, bbi::wrap::u8>();
    convert_from_Z<bbi::thrw::u8, bbi::wrap::u16>();
    convert_from_Z<bbi::thrw::u8, bbi::wrap::u32>();
    convert_from_Z<bbi::thrw::u8, bbi::wrap::u64>();

    convert_from_Z<bbi::thrw::u8, bbi::sat::i8>();
    convert_from_Z<bbi::thrw::u8, bbi::sat::i16>();
    convert_from_Z<bbi::thrw::u8, bbi::sat::i32>();
    convert_from_Z<bbi::thrw::u8, bbi::sat::i64>();

    convert_from_Z<bbi::thrw::u8, bbi::sat::u8>();
    convert_from_Z<bbi::thrw::u8, bbi::sat::u16>();
    convert_from_Z<bbi::thrw::u8, bbi::sat::u32>();
    convert_from_Z<bbi::thrw::u8, bbi::sat::u64>();

    convert_from_Z<bbi::thrw::u8, bbi::thrw::i8>();
    convert_from_Z<bbi::thrw::u8, bbi::thrw::i16>();
    convert_from_Z<bbi::thrw::u8, bbi::thrw::i32>();
    convert_from_Z<bbi::thrw::u8, bbi::thrw::i64>();

    convert_from_Z<bbi::thrw::u8, bbi::thrw::u8>();
    convert_from_Z<bbi::thrw::u8, bbi::thrw::u16>();
    convert_from_Z<bbi::thrw::u8, bbi::thrw::u32>();
    convert_from_Z<bbi::thrw::u8, bbi::thrw::u64>();

    convert_from_Z<bbi::thrw::u16, bbi::wrap::i8>();
    convert_from_Z<bbi::thrw::u16, bbi::wrap::i16>();
    convert_from_Z<bbi::thrw::u16, bbi::wrap::i32>();
    convert_from_Z<bbi::thrw::u16, bbi::wrap::i64>();

    convert_from_Z<bbi::thrw::u16, bbi::wrap::u8>();
    convert_from_Z<bbi::thrw::u16, bbi::wrap::u16>();
    convert_from_Z<bbi::thrw::u16, bbi::wrap::u32>();
    convert_from_Z<bbi::thrw::u16, bbi::wrap::u64>();

    convert_from_Z<bbi::thrw::u16, bbi::sat::i8>();
    convert_from_Z<bbi::thrw::u16, bbi::sat::i16>();
    convert_from_Z<bbi::thrw::u16, bbi::sat::i32>();
    convert_from_Z<bbi::thrw::u16, bbi::sat::i64>();

    convert_from_Z<bbi::thrw::u16, bbi::sat::u8>();
    convert_from_Z<bbi::thrw::u16, bbi::sat::u16>();
    convert_from_Z<bbi::thrw::u16, bbi::sat::u32>();
    convert_from_Z<bbi::thrw::u16, bbi::sat::u64>();

    convert_from_Z<bbi::thrw::u16, bbi::thrw::i8>();
    convert_from_Z<bbi::thrw::u16, bbi::thrw::i16>();
    convert_from_Z<bbi::thrw::u16, bbi::thrw::i32>();
    convert_from_Z<bbi::thrw::u16, bbi::thrw::i64>();

    convert_from_Z<bbi::thrw::u16, bbi::thrw::u8>();
    convert_from_Z<bbi::thrw::u16, bbi::thrw::u16>();
    convert_from_Z<bbi::thrw::u16, bbi::thrw::u32>();
    convert_from_Z<bbi::thrw::u16, bbi::thrw::u64>();

    convert_from_Z<bbi::thrw::u32, bbi::wrap::i8>();
    convert_from_Z<bbi::thrw::u32, bbi::wrap::i16>();
    convert_from_Z<bbi::thrw::u32, bbi::wrap::i32>();
    convert_from_Z<bbi::thrw::u32, bbi::wrap::i64>();

    convert_from_Z<bbi::thrw::u32, bbi::wrap::u8>();
    convert_from_Z<bbi::thrw::u32, bbi::wrap::u16>();
    convert_from_Z<bbi::thrw::u32, bbi::wrap::u32>();
    convert_from_Z<bbi::thrw::u32, bbi::wrap::u64>();

    convert_from_Z<bbi::thrw::u32, bbi::sat::i8>();
    convert_from_Z<bbi::thrw::u32, bbi::sat::i16>();
    convert_from_Z<bbi::thrw::u32, bbi::sat::i32>();
    convert_from_Z<bbi::thrw::u32, bbi::sat::i64>();

    convert_from_Z<bbi::thrw::u32, bbi::sat::u8>();
    convert_from_Z<bbi::thrw::u32, bbi::sat::u16>();
    convert_from_Z<bbi::thrw::u32, bbi::sat::u32>();
    convert_from_Z<bbi::thrw::u32, bbi::sat::u64>();

    convert_from_Z<bbi::thrw::u32, bbi::thrw::i8>();
    convert_from_Z<bbi::thrw::u32, bbi::thrw::i16>();
    convert_from_Z<bbi::thrw::u32, bbi::thrw::i32>();
    convert_from_Z<bbi::thrw::u32, bbi::thrw::i64>();

    convert_from_Z<bbi::thrw::u32, bbi::thrw::u8>();
    convert_from_Z<bbi::thrw::u32, bbi::thrw::u16>();
    convert_from_Z<bbi::thrw::u32, bbi::thrw::u32>();
    convert_from_Z<bbi::thrw::u32, bbi::thrw::u64>();

    convert_from_Z<bbi::thrw::u64, bbi::wrap::i8>();
    convert_from_Z<bbi::thrw::u64, bbi::wrap::i16>();
    convert_from_Z<bbi::thrw::u64, bbi::wrap::i32>();
    convert_from_Z<bbi::thrw::u64, bbi::wrap::i64>();

    convert_from_Z<bbi::thrw::u64, bbi::wrap::u8>();
    convert_from_Z<bbi::thrw::u64, bbi::wrap::u16>();
    convert_from_Z<bbi::thrw::u64, bbi::wrap::u32>();
    convert_from_Z<bbi::thrw::u64, bbi::wrap::u64>();

    convert_from_Z<bbi::thrw::u64, bbi::sat::i8>();
    convert_from_Z<bbi::thrw::u64, bbi::sat::i16>();
    convert_from_Z<bbi::thrw::u64, bbi::sat::i32>();
    convert_from_Z<bbi::thrw::u64, bbi::sat::i64>();

    convert_from_Z<bbi::thrw::u64, bbi::sat::u8>();
    convert_from_Z<bbi::thrw::u64, bbi::sat::u16>();
    convert_from_Z<bbi::thrw::u64, bbi::sat::u32>();
    convert_from_Z<bbi::thrw::u64, bbi::sat::u64>();

    convert_from_Z<bbi::thrw::u64, bbi::thrw::i8>();
    convert_from_Z<bbi::thrw::u64, bbi::thrw::i16>();
    convert_from_Z<bbi::thrw::u64, bbi::thrw::i32>();
    convert_from_Z<bbi::thrw::u64, bbi::thrw::i64>();

    convert_from_Z<bbi::thrw::u64, bbi::thrw::u8>();
    convert_from_Z<bbi::thrw::u64, bbi::thrw::u16>();
    convert_from_Z<bbi::thrw::u64, bbi::thrw::u32>();
    convert_from_Z<bbi::thrw::u64, bbi::thrw::u64>();


    test_add<bbi::Signed, 8, bbi::Wrap>();
    test_add<bbi::Signed, 16, bbi::Wrap>();
    test_add<bbi::Signed, 32, bbi::Wrap>();

    test_add<bbi::Unsigned, 8, bbi::Wrap>();
    test_add<bbi::Unsigned, 16, bbi::Wrap>();
    test_add<bbi::Unsigned, 32, bbi::Wrap>();

    test_add<bbi::Signed, 8, bbi::Saturate>();
    test_add<bbi::Signed, 16, bbi::Saturate>();
    test_add<bbi::Signed, 32, bbi::Saturate>();

    test_add<bbi::Unsigned, 8, bbi::Saturate>();
    test_add<bbi::Unsigned, 16, bbi::Saturate>();
    test_add<bbi::Unsigned, 32, bbi::Saturate>();

    test_add<bbi::Signed, 8, bbi::Throw>();
    test_add<bbi::Signed, 16, bbi::Throw>();
    test_add<bbi::Signed, 32, bbi::Throw>();

    test_add<bbi::Unsigned, 8, bbi::Throw>();
    test_add<bbi::Unsigned, 16, bbi::Throw>();
    test_add<bbi::Unsigned, 32, bbi::Throw>();


    test_sub<bbi::Signed, 8, bbi::Wrap>();
    test_sub<bbi::Signed, 16, bbi::Wrap>();
    test_sub<bbi::Signed, 32, bbi::Wrap>();

    test_sub<bbi::Unsigned, 8, bbi::Wrap>();
    test_sub<bbi::Unsigned, 16, bbi::Wrap>();
    test_sub<bbi::Unsigned, 32, bbi::Wrap>();

    test_sub<bbi::Signed, 8, bbi::Saturate>();
    test_sub<bbi::Signed, 16, bbi::Saturate>();
    test_sub<bbi::Signed, 32, bbi::Saturate>();

    test_sub<bbi::Unsigned, 8, bbi::Saturate>();
    test_sub<bbi::Unsigned, 16, bbi::Saturate>();
    test_sub<bbi::Unsigned, 32, bbi::Saturate>();

    test_sub<bbi::Signed, 8, bbi::Throw>();
    test_sub<bbi::Signed, 16, bbi::Throw>();
    test_sub<bbi::Signed, 32, bbi::Throw>();

    test_sub<bbi::Unsigned, 8, bbi::Throw>();
    test_sub<bbi::Unsigned, 16, bbi::Throw>();
    test_sub<bbi::Unsigned, 32, bbi::Throw>();


    test_mul<bbi::Signed, 8, bbi::Wrap>();
    test_mul<bbi::Signed, 16, bbi::Wrap>();
    test_mul<bbi::Signed, 32, bbi::Wrap>();

    test_mul<bbi::Unsigned, 8, bbi::Wrap>();
    test_mul<bbi::Unsigned, 16, bbi::Wrap>();
    test_mul<bbi::Unsigned, 32, bbi::Wrap>();

    test_mul<bbi::Signed, 8, bbi::Saturate>();
    test_mul<bbi::Signed, 16, bbi::Saturate>();
    test_mul<bbi::Signed, 32, bbi::Saturate>();

    test_mul<bbi::Unsigned, 8, bbi::Saturate>();
    test_mul<bbi::Unsigned, 16, bbi::Saturate>();
    test_mul<bbi::Unsigned, 32, bbi::Saturate>();

    test_mul<bbi::Signed, 8, bbi::Throw>();
    test_mul<bbi::Signed, 16, bbi::Throw>();
    test_mul<bbi::Signed, 32, bbi::Throw>();

    test_mul<bbi::Unsigned, 8, bbi::Throw>();
    test_mul<bbi::Unsigned, 16, bbi::Throw>();
    test_mul<bbi::Unsigned, 32, bbi::Throw>();


    test_div<bbi::Unsigned, 8, bbi::Wrap>();
    test_div<bbi::Unsigned, 16, bbi::Wrap>();
    test_div<bbi::Unsigned, 32, bbi::Wrap>();
    test_div<bbi::Unsigned, 64, bbi::Wrap>();

    test_div<bbi::Signed, 8, bbi::Wrap>();
    test_div<bbi::Signed, 16, bbi::Wrap>();
    test_div<bbi::Signed, 32, bbi::Wrap>();
    test_div<bbi::Signed, 64, bbi::Wrap>();

    test_div<bbi::Unsigned, 8, bbi::Saturate>();
    test_div<bbi::Unsigned, 16, bbi::Saturate>();
    test_div<bbi::Unsigned, 32, bbi::Saturate>();
    test_div<bbi::Unsigned, 64, bbi::Saturate>();

    test_div<bbi::Signed, 8, bbi::Saturate>();
    test_div<bbi::Signed, 16, bbi::Saturate>();
    test_div<bbi::Signed, 32, bbi::Saturate>();
    test_div<bbi::Signed, 64, bbi::Saturate>();

    test_div<bbi::Unsigned, 8, bbi::Throw>();
    test_div<bbi::Unsigned, 16, bbi::Throw>();
    test_div<bbi::Unsigned, 32, bbi::Throw>();
    test_div<bbi::Unsigned, 64, bbi::Throw>();

    test_div<bbi::Signed, 8, bbi::Throw>();
    test_div<bbi::Signed, 16, bbi::Throw>();
    test_div<bbi::Signed, 32, bbi::Throw>();
    test_div<bbi::Signed, 64, bbi::Throw>();
}
