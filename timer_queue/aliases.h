#pragma once

namespace baio  = boost::asio;
namespace bcont = boost::container;
namespace bsys  = boost::system;

////////////////////////////////////////////////////////////////////////////////
namespace xlib // x3me library
{

template <auto Size, typename Signature>
using inplace_fn =
    x3me::utils::inplace_fn<x3me::utils::inplace_params<Size>, Signature>;

} // namespace xlib
