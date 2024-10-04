# getDesktopVolume-sample-by-IAudioMeterInformation

IAudioMeterInformation の GetPeakValue() を使用してデスクトップ上の音量を取得するサンプル。

ビルド結果として生成されるmain.exeを実行すると、その時点でのデスクトップ上の音量が 0 - 1 の間のfloatで表示されます。

## 想定環境

- OS：Windows (10で検証済み)
- ビルド環境：VC Code
- コンパイラ：MinGW-w64
  - 環境変数のパスに以下が通してあること
    - C:\mingw64\bin

## 注意点

`.vscode/c_cpp_properties.json` に記載されているMinGW-w64のパスは、インストールしたものに合わせる必要があります。

``` json
            "includePath": [
                "${workspaceFolder}/**",
                "C:/mingw64/include",
                "C:/mingw64/lib/gcc/x86_64-w64-mingw32/14.2.0/include"
            ],
```

## 備考

MinGW には IAudioMeterInformation の完全な構造定義がないようです。
PortAudio リポジトリから IAudioMeterInformation の完全な定義をコピーし、必要な __CRT_UUID_DECL を追加して、コンパイル エラー「_GUID const& __mingw_uuidof への未定義の参照」を回避しています。

出典：
https://stackoverflow.com/questions/62012071/why-is-iaudiometerinformation-an-incomplete-type

以下のコードが上記の説明に該当します。

``` cpp
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
```

このコードが無い場合、main関数内の以下のコードでコンパイルエラーが発生します。

``` cpp
    hr = pMeterInfo->GetPeakValue(&peakValue);
    pMeterInfo->Release();
```

エラーの表示例

``` sh
main.cpp:47:47: error: invalid use of incomplete type 'IAudioMeterInformation' {aka 'struct IAudioMeterInformation'}
     hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
          ~~~~~~~~~~~
```
