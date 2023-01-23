#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "utilities.h"
#include <TlHelp32.h>
#include <iphlpapi.h>
#include <signal.h>


namespace utilities
{
	__forceinline void strip_string(std::string& str)
	{
		str.erase(std::remove_if(str.begin(), str.end(), [](int c) {return !(c > 32 && c < 127); }), str.end());
	}

	__forceinline std::string get_hwid()
	{
		std::string result = _S("");

		HANDLE hDevice = li(CreateFileA)(_S("\\\\.\\PhysicalDrive0"), NULL, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)nullptr, OPEN_EXISTING, NULL, (HANDLE)nullptr);

		if (hDevice == INVALID_HANDLE_VALUE) return result;

		STORAGE_PROPERTY_QUERY storagePropertyQuery;
		ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
		storagePropertyQuery.PropertyId = StorageDeviceProperty;
		storagePropertyQuery.QueryType = PropertyStandardQuery;

		STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };
		DWORD dwBytesReturned = 0;

		li(DeviceIoControl)
			(
				hDevice,
				IOCTL_STORAGE_QUERY_PROPERTY,
				&storagePropertyQuery,
				sizeof(STORAGE_PROPERTY_QUERY),
				&storageDescriptorHeader,
				sizeof(STORAGE_DESCRIPTOR_HEADER),
				&dwBytesReturned,
				nullptr
				);

		const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
		BYTE* pOutBuffer = new BYTE[dwOutBufferSize];
		ZeroMemory(pOutBuffer, dwOutBufferSize);

		li(DeviceIoControl)
			(
				hDevice,
				IOCTL_STORAGE_QUERY_PROPERTY,
				&storagePropertyQuery,
				sizeof(STORAGE_PROPERTY_QUERY),
				pOutBuffer,
				dwOutBufferSize,
				&dwBytesReturned,
				nullptr
				);

		STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pOutBuffer;

		if (pDeviceDescriptor->SerialNumberOffset)
		{
			result += std::string((char*)(pOutBuffer + pDeviceDescriptor->SerialNumberOffset));
		}

		if (pDeviceDescriptor->ProductRevisionOffset)
		{
			result += std::string((char*)(pOutBuffer + pDeviceDescriptor->ProductRevisionOffset));
		}

		if (pDeviceDescriptor->ProductIdOffset)
		{
			result += std::string((char*)(pOutBuffer + pDeviceDescriptor->ProductIdOffset));
		}

		uint32_t regs[4];
		__cpuid((int*)regs, 0);

		std::string vendor;

		vendor += std::string((char*)&regs[1], 4);
		vendor += std::string((char*)&regs[3], 4);
		vendor += std::string((char*)&regs[2], 4);

		result += std::string(vendor);

		strip_string(result);

		delete[] pOutBuffer;
		li(CloseHandle)(hDevice);

		result = result;

		return result;
	}

	__forceinline std::string MAC()
	{
		PIP_ADAPTER_INFO AdapterInfo;
		DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
		char* mac_addr = (char*)malloc(18);

		AdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
		if (AdapterInfo == NULL)
		{
			//	printf("Error allocating memory needed to call GetAdaptersinfo\n");
			free(mac_addr);
			return NULL; // it is safe to call free(NULL)
		}

		// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable

			// Contains pointer to current adapter info
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do {
			// technically should look at pAdapterInfo->AddressLength
			//   and not assume it is 6.
			sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			// print them all, return the last one.
			// return mac_addr;

			printf("\n");
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);

		free(AdapterInfo);
		return mac_addr; // caller must free.
	}
}
