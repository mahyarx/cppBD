// bd-1.cpp : mahyar@tajdini.net
// www.TajDini.net

#include "stdafx.h"
#include<io.h>
#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
//wmi definations 
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include <comutil.h>
#include <string>

#pragma comment(lib, "wbemuuid.lib")
string msg;
string wmix()
{
	HRESULT hres;

	// Initialize COM.
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << "Failed to initialize COM library. "
			<< "Error code = 0x"
			<< hex << hres << endl;
		return "1";              // Program has failed.
	}

	// Initialize 
	hres = CoInitializeSecurity(
		NULL,
		-1,      // COM negotiates service                  
		NULL,    // Authentication services
		NULL,    // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
		RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
		NULL,             // Authentication info 
		EOAC_NONE,        // Additional capabilities
		NULL              // Reserved
		);


	if (FAILED(hres))
	{
		cout << "Failed to initialize security. "
			<< "Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return "1";          // Program has failed.
	}

	// Obtain the initial locator to Windows Management
	// on a particular host computer.
	IWbemLocator *pLoc = 0;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object. "
			<< "Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return "1";       // Program has failed.
	}

	IWbemServices *pSvc = 0;

	// Connect to the root\cimv2 namespace with the
	// current user and obtain pointer pSvc
	// to make IWbemServices calls.

	hres = pLoc->ConnectServer(

		_bstr_t(L"ROOT\\CIMV2"), // WMI namespace
		NULL,                    // User name
		NULL,                    // User password
		0,                       // Locale
		NULL,                    // Security flags                 
		0,                       // Authority       
		0,                       // Context object
		&pSvc                    // IWbemServices proxy
		);

	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x"
			<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return "1";                // Program has failed.
	}

	cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

	// Set the IWbemServices proxy so that impersonation
	// of the user (client) occurs.
	hres = CoSetProxyBlanket(

		pSvc,                         // the proxy to set
		RPC_C_AUTHN_WINNT,            // authentication service
		RPC_C_AUTHZ_NONE,             // authorization service
		NULL,                         // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,       // authentication level
		RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
		NULL,                         // client identity 
		EOAC_NONE                     // proxy capabilities     
		);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return "1";               // Program has failed.
	}


	// Use the IWbemServices pointer to make requests of WMI. 
	// Make requests here:

	// For example, query for all the running processes
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_Process"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << "Query for processes failed. "
			<< "Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return "1";               // Program has failed.
	}
	else
	{
		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while (pEnumerator)
		{
			hres = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn)
			{
				break;
			}

			VARIANT vtProp;
			// Mahyar :) Get the value of the Name property
			hres = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
			wcout << "Process Name : " << vtProp.bstrVal << endl;
			int len = wcslen(vtProp.bstrVal);
			char* msg1 = new char[len + 1];
			wcstombs(msg1, vtProp.bstrVal, len + 1);
			msg = msg + msg1;
			VariantClear(&vtProp);
		}

	}

	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	CoUninitialize();

	return msg;   // Program successfully completed.
}



int main(int argc, char *argv[])
{
	wmix();
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;
	char *message;
	string msgx;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);

	while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
	{
		puts("Connection accepted");

		//Reply to the client
		//message = "Mahyar: Hello Client , I have received your connection. But I have to go now, bye\n";
		msgx = wmix();
		message = strdup(msgx.c_str());
		send(new_socket, message, strlen(message), 0);
	}

	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
		return 1;
	}

	closesocket(s);
	WSACleanup();

	return 0;
}