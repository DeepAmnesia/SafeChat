#pragma once
#include <windows.h>
#include <aes.h>
#include <modes.h>
#include <base64.h>
#include "lazy_importer.hpp"
#pragma comment(lib, "cryptlib.lib")

namespace aes
{
	extern __forceinline std::string encrypt(const std::string& str, const std::string& cipher_key, const std::string& iv_key);
	extern __forceinline std::string decrypt(const std::string& str, const std::string& cipher_key, const std::string& iv_key);
}