#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>

__CRT_UUID_DECL(IAudioMeterInformation, 0xC02216F6, 0x8C67, 0x4B5B, 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64);

// Define the IAudioMeterInformation interface
MIDL_INTERFACE("C02216F6-8C67-4B5B-9D00-D008E73E0064")
IAudioMeterInformation : public IUnknown
{
public:
    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPeakValue(
        /* [out] */ float *pfPeak) = 0;

    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(
        /* [out] */ UINT *pnChannelCount) = 0;

    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(
        /* [in] */ UINT32 u32ChannelCount,
        /* [size_is][out] */ float *afPeakValues) = 0;

    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE QueryHardwareSupport(
        /* [out] */ DWORD *pdwHardwareSupportMask) = 0;
};

int main() {
    HRESULT hr;
    CoInitializeEx(nullptr, COINIT_MULTITHREADED); // Initialize COM for multi-threaded applications

    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) {
        std::cerr << "Failed to create device enumerator: " << std::hex << hr << std::endl;
        return -1;
    }

    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr)) {
        std::cerr << "Failed to get default audio endpoint: " << std::hex << hr << std::endl;
        pEnumerator->Release();
        return -1;
    }

    IAudioMeterInformation* pMeterInfo = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_INPROC_SERVER, nullptr, (void**)&pMeterInfo);
    if (FAILED(hr)) {
        std::cerr << "Failed to activate audio meter information: " << std::hex << hr << std::endl;
        pDevice->Release();
        pEnumerator->Release();
        return -1;
    }

    float peakValue = 0.0f;
    hr = pMeterInfo->GetPeakValue(&peakValue);
    if (SUCCEEDED(hr)) {
        std::cout << "Current peak value: " << peakValue << std::endl;
    } else {
        std::cerr << "Failed to get peak value: " << std::hex << hr << std::endl;
    }

    pMeterInfo->Release();
    pDevice->Release();
    pEnumerator->Release();
    CoUninitialize();

    return 0;
}