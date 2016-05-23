// WLANfixeri.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <netcon.h>

HRESULT DisableEnableConnections(BOOL bEnable)
{
    HRESULT hr = E_FAIL;
    CoInitialize(NULL);
    INetConnectionManager *pNetConnectionManager = NULL;
 hr = CoCreateInstance(CLSID_ConnectionManager,
                          NULL,
                          CLSCTX_LOCAL_SERVER | CLSCTX_NO_CODE_DOWNLOAD,
                          IID_INetConnectionManager,
                          reinterpret_cast<LPVOID *>(&pNetConnectionManager)
                         );
    if (SUCCEEDED(hr))
    {
        /*
  Get an enumurator for the set of connections on the system
  */
        IEnumNetConnection* pEnumNetConnection;
        pNetConnectionManager->EnumConnections(NCME_DEFAULT, &pEnumNetConnection);
        ULONG ulCount = 0;
        BOOL fFound = FALSE;
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);       
        HRESULT hrT = S_OK;
        /*  
   Enumurate through the list of adapters on the system and look for the one we want
   NOTE: To include per-user RAS connections in the list, you need to set the COM
   Proxy Blanket on all the interfaces. This is not needed for All-user RAS
   connections or LAN connections.
  */
        do
        {
            NETCON_PROPERTIES* pProps = NULL;
            INetConnection *   pConn;

            /*
   Find the next (or first connection)
   */
            hrT = pEnumNetConnection->Next(1, &pConn, &ulCount);
            if (SUCCEEDED(hrT) && 1 == ulCount)
            {
    /*
    Get the connection properties
    */
                hrT = pConn->GetProperties(&pProps);
    if (S_OK == hrT)                {
     printf("* %S\n", pProps->pszwName);
     if (bEnable)
     {
      printf("      Enabling adapter: %S...\n",pProps->pszwName);
      hr = pConn->Connect();
     }
     else
     {
      printf("      Disabling adapter: %S...\n",pProps->pszwName);
      hr = pConn->Disconnect();
     }
                    CoTaskMemFree (pProps->pszwName);
                    CoTaskMemFree (pProps->pszwDeviceName);
                    CoTaskMemFree (pProps);
                }
                pConn->Release();
                pConn = NULL;
            }

        } while (SUCCEEDED(hrT) && 1 == ulCount && !fFound);

        if (FAILED(hrT))
        {
            hr = hrT;
        }
        pEnumNetConnection->Release();
    }
    if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_RETRY))
    {
        printf("Could not enable or disable connection (0x%08x)\r\n", hr);
    }
    pNetConnectionManager->Release();
    CoUninitialize();
 return hr;
}