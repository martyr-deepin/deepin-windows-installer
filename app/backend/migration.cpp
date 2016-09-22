#include <winsock2.h>

#include "migration.h"

#include "utils.h"
#include <xsys.h>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QSettings>

#include <windows.h>
#include <shlobj.h>

using namespace DeepinInstaller;

Migration::Migration(const QString &root, QObject *parent) :
    QObject(parent)
{
    rootDir = root;
    migrationDataConfigFile = rootDir + "\\data\\deepin_data_migration.conf";
    volumeLetterConfigFile = rootDir + "\\data\\volume_letter.conf";
}

QString removeSpace(QString raw) {
    return raw.left(raw.length()-1).right(raw.length()-2);
}


bool Migration::run()
{
    createMigrationDataConfig();
    createVolumeLetterConfig();
    return true;
}

void Migration::createMigrationDataConfig() {
    WCHAR sysPath[MAX_PATH];
    GetSystemDirectory(sysPath, MAX_PATH);
    QString sysLetter = QString::fromWCharArray(sysPath).left(1);

//    QString migraFileName = sysLetter + ":\\.deepin_data_migration.ini";

    QString migraFileName = migrationDataConfigFile;
    QFile outFile(migraFileName);
    outFile.open(/*QIODevice::Text |*/ QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream ts(&outFile);
    ts.setCodec("utf-8");

    QList<DiskInfo> alldisklist = GetAllLocalDiskList();
    QList<DiskInfo>::iterator itor;
    QMap<QString, DiskInfo> diskmap;
    QMap<QString, DiskInfo>::iterator mapitor;

    QString uuids = " ";
    Config DeepinDataMigration;
    for (itor = alldisklist.begin(); itor != alldisklist.end(); ++itor){
        qDebug()<<"Add  Disk"<<itor->VolumeLetter;
        qDebug()<<"     Vol "<<itor->VolumeName;
        qDebug()<<"     Labe"<<itor->Label;
        qDebug()<<"     Seri"<<itor->UUID;
        qDebug()<<"     Type"<<itor->FileSystemName;
        if ("ntfs" == itor->FileSystemName || "vfat" == itor->FileSystemName) {
            Selection *Volume = new Selection();
            Volume->insert("Letter", itor->VolumeLetter);
            Volume->insert("Label", "\""+itor->Label+"\"");
            Volume->insert("Format", itor->FileSystemName);
            DeepinDataMigration.insert(itor->UUID , Volume);
            uuids += itor->UUID + ",";
            diskmap.insert(itor->VolumeLetter, *itor);
        }
    }

    Selection *Volume = new Selection();
    Volume->insert("Volumes", removeSpace(uuids));
    DeepinDataMigration.insert("Volume", Volume);

    Selection *System = new Selection();
    mapitor = diskmap.find(sysLetter);
    if (diskmap.end() != mapitor) {
        System->insert("Volume", mapitor->UUID);
    }
    DeepinDataMigration.insert("System", System);

//    WCHAR szPath[MAX_PATH];
//    SHGetSpecialFolderPath(NULL,szPath,CSIDL_DESKTOP,0);
//    QString relDesktop = QString::fromWCharArray(szPath);
//    QString relDesktopRoot = relDesktop.left(1);
//    relDesktop = QDir::fromNativeSeparators(relDesktop.right(relDesktop.length()-2));

//    for (itor = alldisklist.begin(); itor != alldisklist.end(); ++itor){
//        if (itor->VolumeLetter == relDesktopRoot) {
//            relDesktopRoot =  itor->UUID;
//        }
//    }

//    Selection *Desktop = new Selection();
//    Desktop->insert("Volume", relDesktopRoot);
//    Desktop->insert("Path", relDesktop);
//    DeepinDataMigration.insert("Desktop", Desktop);

//    QString username = Xapi::Username();
//    qDebug()<<username;
//    Selection *User = new Selection();
//    User->insert("Name", username);
//    DeepinDataMigration.insert("User", User);

//    collectNetWork(&DeepinDataMigration);

    Config::iterator selitor;
    for (selitor = DeepinDataMigration.begin(); selitor != DeepinDataMigration.end(); ++selitor ) {
        ts<<"["<<selitor.key() <<"]"<<endl;
        Selection::iterator kvitor;
        for (kvitor = selitor.value()->begin(); kvitor != selitor.value()->end(); ++kvitor ) {
            ts<<kvitor.key() <<"="<<kvitor.value()<<endl;
        }
        ts<<endl;
    }
    outFile.close();
}

void Migration::createVolumeLetterConfig()
{
    QSettings dataMigrationSettings(migrationDataConfigFile, QSettings::IniFormat);
    QSettings volumeLetterSettings(volumeLetterConfigFile, QSettings::IniFormat);

    dataMigrationSettings.beginGroup("Volume");
    auto uuidList = dataMigrationSettings.value("Volumes").toStringList();
    dataMigrationSettings.endGroup();

    qDebug()<<volumeLetterConfigFile<<uuidList;
    volumeLetterSettings.beginGroup("Volume");
    foreach (auto uuid, uuidList){
        dataMigrationSettings.beginGroup(uuid);
        auto letter = dataMigrationSettings.value("Letter").toString();
        dataMigrationSettings.endGroup();
        volumeLetterSettings.setValue(uuid.toLower(), letter);
    }
    volumeLetterSettings.endGroup();
    volumeLetterSettings.sync();
}

typedef struct _NetworkInterfaceEntry {
    QString ID;
    QString Name;
    QString Mac;
    QString IP;
    QString Mask;
    QString Gateway;
    QString PrimacyDNS;
    QString SecondDNS;
}NetworkInterfaceEntry;


#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Ws2_32.lib")



#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

QList<NetworkInterfaceEntry> listInterfaceEntryOld() {
    QList<NetworkInterfaceEntry> interfaceList;

    /* Declare and initialize variables */

// It is possible for an adapter to have multiple
// IPv4 addresses, gateways, and secondary WINS servers
// assigned to the adapter.
//
// Note that this sample code only prints out the
// first entry for the IP address/mask, and gateway, and
// the primary and secondary WINS server for each adapter.

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;

/* variables used to print DHCP time info */
    struct tm newtime;
    char buffer[32];
    errno_t error;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return interfaceList;
    }
// Make an initial call to GetAdaptersInfo to get
// the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return interfaceList;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            NetworkInterfaceEntry entry;
            entry.ID = QString::fromStdString(pAdapter->AdapterName);
            entry.Name = QString::fromStdString(pAdapter->Description);


            printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
            printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
            printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
            printf("\tAdapter Addr: \t");
            for (i = 0; i < pAdapter->AddressLength; i++) {
                if (i == (pAdapter->AddressLength - 1)){
                    entry.Mac += QString("%1").arg(pAdapter->Address[i], 2, 16, QLatin1Char('0'));
                    printf("%.2X\n", (int) pAdapter->Address[i]);
                }
                else{
                    entry.Mac += QString("%1:").arg(pAdapter->Address[i], 2, 16, QLatin1Char('0'));
                    printf("%.2X-", (int) pAdapter->Address[i]);
                }
            }
            printf("\tIndex: \t%d\n", pAdapter->Index);
            printf("\tType: \t");
            switch (pAdapter->Type) {
            case MIB_IF_TYPE_OTHER:
                printf("Other\n");
                break;
            case MIB_IF_TYPE_ETHERNET:
                printf("Ethernet\n");
                break;
            case MIB_IF_TYPE_TOKENRING:
                printf("Token Ring\n");
                break;
            case MIB_IF_TYPE_FDDI:
                printf("FDDI\n");
                break;
            case MIB_IF_TYPE_PPP:
                printf("PPP\n");
                break;
            case MIB_IF_TYPE_LOOPBACK:
                printf("Lookback\n");
                break;
            case MIB_IF_TYPE_SLIP:
                printf("Slip\n");
                break;
            default:
                printf("Unknown type %ld\n", pAdapter->Type);
                break;
            }

            entry.IP = QString::fromStdString(pAdapter->IpAddressList.IpAddress.String);
            entry.Mask = QString::fromStdString(pAdapter->IpAddressList.IpMask.String);
            entry.Gateway = QString::fromStdString(pAdapter->GatewayList.IpAddress.String);

            printf("\tIP Address: \t%s\n",
                   pAdapter->IpAddressList.IpAddress.String);
            printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

            printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
            printf("\t***\n");
            interfaceList.push_back(entry);
            if (pAdapter->DhcpEnabled) {
                printf("\tDHCP Enabled: Yes\n");
                printf("\t  DHCP Server: \t%s\n",
                       pAdapter->DhcpServer.IpAddress.String);

                printf("\t  Lease Obtained: ");
                /* Display local time */
                error = _localtime32_s(&newtime, (__time32_t*) &pAdapter->LeaseObtained);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    // Convert to an ASCII representation
                    error = asctime_s(buffer, 32, &newtime);
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else
                        /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }

                printf("\t  Lease Expires:  ");
                error = _localtime32_s(&newtime, (__time32_t*) &pAdapter->LeaseExpires);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    // Convert to an ASCII representation
                    error = asctime_s(buffer, 32, &newtime);
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else
                        /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }
            } else
                printf("\tDHCP Enabled: No\n");

            if (pAdapter->HaveWins) {
                printf("\tHave Wins: Yes\n");
                printf("\t  Primary Wins Server:    %s\n",
                       pAdapter->PrimaryWinsServer.IpAddress.String);
                printf("\t  Secondary Wins Server:  %s\n",
                       pAdapter->SecondaryWinsServer.IpAddress.String);
            } else
                printf("\tHave Wins: No\n");
            pAdapter = pAdapter->Next;
            printf("\n");
        }
    } else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

    }
    if (pAdapterInfo)
        FREE(pAdapterInfo);

    return interfaceList;
}

typedef struct _IPInterface {
    QString Name;
    QString IP;
}IPInterface;

#include <QNetworkInterface>

QList<IPInterface> listActiveInterface() {
    QList<IPInterface> interfaceList;
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    if ( !ifaces.isEmpty() )
    {
      for(int i=0; i < ifaces.size(); i++)
      {

        unsigned int flags = ifaces[i].flags();
        bool isLoopback = (bool)(flags & QNetworkInterface::IsLoopBack);
        bool isP2P = (bool)(flags & QNetworkInterface::IsPointToPoint);
        bool isRunning = (bool)(flags & QNetworkInterface::IsRunning);

        // If this interface isn't running, we don't care about it
        if ( !isRunning ) continue;
        // We only want valid interfaces that aren't loopback/virtual and not point to point
        if ( !ifaces[i].isValid() || isLoopback || isP2P ) continue;
        QList<QNetworkAddressEntry> addresses = ifaces[i].addressEntries();
        for(int a=0; a < addresses.size(); a++)
        {
          QHostAddress ipaddr = addresses[a].ip();
          qDebug()<<ipaddr;
          // Ignore local host
          if ( ipaddr == QHostAddress::LocalHost ) continue;

          // Ignore non-ipv4 addresses
          if ( !ipaddr.toIPv4Address() ) continue;

          QString ip = ipaddr.toString();
          if ( ip.isEmpty() ) continue;
          IPInterface ii;
          ii.Name = ifaces[i].humanReadableName();
          ii.IP = ip;
          interfaceList.push_back( ii );
          qDebug() << "possible address: " << ifaces[i].humanReadableName() << "->" << ip;
        }
      }
    }
    return interfaceList;
}

QString getAddrStirng( SOCKET_ADDRESS addr) {
    WCHAR cAddr[160];
    DWORD cLen=160;
    int iErr;
    ZeroMemory(cAddr,160);
    if (SOCKET_ERROR == WSAAddressToString((SOCKADDR *)(addr.lpSockaddr),sizeof(SOCKADDR),NULL,cAddr,&cLen))
    {
        iErr=WSAGetLastError();
        qDebug()<<"ERRROR"<<iErr;
    }
    return QString::fromStdWString(cAddr);
}

QList <NetworkInterfaceEntry> listInterfaceEntry () {
    QList<NetworkInterfaceEntry> interfaceList;

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return interfaceList;
    }

    // Declare and initialize variables
//    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    int i = 0;
    ULONG flags, outBufLen = 0, family;
    LPVOID lpMsgBuf;
    PIP_ADAPTER_ADDRESSES pAddresses;
    PIP_ADAPTER_ADDRESSES pCurrAddresses;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    IP_ADAPTER_PREFIX *pPrefix = NULL;

    // Set the flags to pass to GetAdaptersAddresses
    flags = GAA_FLAG_INCLUDE_PREFIX;
    // default to unspecified address family (both)
    family = AF_INET;
    lpMsgBuf = NULL;
    pAddresses = NULL;

    pCurrAddresses = NULL;
    pUnicast = NULL;
    pAnycast = NULL;
    pMulticast = NULL;

    outBufLen = sizeof (IP_ADAPTER_ADDRESSES);
    pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
    if (pAddresses == NULL) {
        printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct!\n");
        exit(1);
    } else {
        printf("Memory allocation for IP_ADAPTER_ADDRESSES struct is OK!\n");
    }

    // Make an initial call to GetAdaptersAddresses to get the
    // size needed into the outBufLen variable

    if (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW) {
        printf("Not enough buffer! Re-allocating...\n");
        FREE(pAddresses);
        pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
    } else
        printf("Buffer allocation is OK!\n");

    if (pAddresses == NULL) {
        printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct!\n");
        exit(1);
    }else
        printf("Memory allocation for IP_ADAPTER_ADDRESSES struct is OK!\n");

    // Make a second call to GetAdapters Addresses to get the actual data we want
    printf("Memory allocated for GetAdapterAddresses = %d bytes\n", outBufLen);

    dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;

        while (pCurrAddresses) {
            printf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n", pCurrAddresses->Length);
            printf("\tIfIndex (IPv4 interface): %u\n", pCurrAddresses->IfIndex);
            printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

            NetworkInterfaceEntry entry;

            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                entry.IP = getAddrStirng(pUnicast->Address);
                qDebug()<<entry.IP;
            }

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
                entry.PrimacyDNS = getAddrStirng(pDnServer->Address);
                if (pDnServer->Next) {
                    entry.SecondDNS = getAddrStirng(pDnServer->Next->Address);
                }
                qDebug()<<entry.PrimacyDNS;
                qDebug()<<entry.SecondDNS;
            }

            entry.ID = QString::fromStdWString(pCurrAddresses->Description);
            entry.Name = QString::fromStdWString(pCurrAddresses->FriendlyName);

            if (pCurrAddresses->PhysicalAddressLength != 0) {
                printf("\tPhysical address: ");
                for (i = 0; i < (int) pCurrAddresses->PhysicalAddressLength; i++)
                {
                    if (i == (pCurrAddresses->PhysicalAddressLength - 1)) {
                        entry.Mac += QString("%1").arg(pCurrAddresses->PhysicalAddress[i], 2, 16, QLatin1Char('0'));
                        printf("%.2X\n", (int) pCurrAddresses->PhysicalAddress[i]);
                    }else{
                        entry.Mac += QString("%1:").arg(pCurrAddresses->PhysicalAddress[i], 2, 16, QLatin1Char('0'));
                        printf("%.2X-", (int) pCurrAddresses->PhysicalAddress[i]);
                    }
                }
            }

            printf("\tFlags: %ld\n", pCurrAddresses->Flags);
            printf("\tMtu: %lu\n", pCurrAddresses->Mtu);
            printf("\tIfType: %ld\n", pCurrAddresses->IfType);
            printf("\tOperStatus: %ld\n", pCurrAddresses->OperStatus);
            printf("\tIpv6IfIndex (IPv6 interface): %u\n", pCurrAddresses->Ipv6IfIndex);
            printf("\tZoneIndices (hex): ");

            pPrefix = pCurrAddresses->FirstPrefix;
            if (pPrefix){
                qDebug()<<"Pre"<<getAddrStirng(pPrefix->Address);

                printf("\tNumber of IP Adapter Prefix entries: %d\n", i);
            } else {
                printf("\tNumber of IP Adapter Prefix entries: 0\n");
            }
            printf("\n");
            pCurrAddresses = pCurrAddresses->Next;
            interfaceList.push_back(entry);
        }
    }
    FREE(pAddresses);
    return interfaceList;
}

#include <QTcpSocket>

bool collectNetWork(Config *cfg) {
    QList<NetworkInterfaceEntry> newentrylist = listInterfaceEntry();
    QList<NetworkInterfaceEntry> oldentrylist = listInterfaceEntryOld();

    QList<NetworkInterfaceEntry> entrylist;
    for (int i =0; i<newentrylist.size(); ++i) {
        for (int j =0; j<oldentrylist.size(); ++j) {
            if (oldentrylist.at(j).IP == newentrylist.at(i).IP) {
                NetworkInterfaceEntry entry =oldentrylist.at(j);
                entry.PrimacyDNS = newentrylist.at(i).PrimacyDNS;
                entry.SecondDNS = newentrylist.at(i).SecondDNS;
                entrylist.push_back(entry);
            }
        }

    }

    //Get active list
    QList<IPInterface> activeInterface = listActiveInterface();

    QList<NetworkInterfaceEntry> activeEntrys;



    for (int i =0; i<entrylist.size(); ++i) {
        qDebug()<<entrylist.at(i).IP;

        for (int j=0; j<activeInterface.size(); ++j) {
            qDebug()<<activeInterface.at(j).IP;
            if (entrylist.at(i).IP == activeInterface.at(j).IP) {
                NetworkInterfaceEntry entry = entrylist.at(i);
                entry.Name = activeInterface.at(j).Name;
                activeEntrys.push_back(entry);
            }

        }
    }

    if (activeEntrys.isEmpty()) {
        return false;
    }

    QTcpSocket dnsTestSocket;
    QString localIP = activeEntrys.at(0).IP;    //fall back
    for (int i =0; i<activeEntrys.size(); ++i) {
        QString gatewayAddress  = activeEntrys.at(i).Gateway;  //try google DNS or sth. else reliable first
        dnsTestSocket.connectToHost(gatewayAddress, 53);
        if (dnsTestSocket.waitForConnected(3000))
        {
            localIP = dnsTestSocket.localAddress().toString();
        }
    }

    Selection *CurrentNetwork = new Selection();
    for (int i =0; i<activeEntrys.size(); ++i) {
        if (localIP == activeEntrys.at(i).IP) {
            CurrentNetwork->insert("ID", activeEntrys.at(i).ID);
            CurrentNetwork->insert("Name", "\"" + activeEntrys.at(i).Name + "\"");
            CurrentNetwork->insert("Mac", activeEntrys.at(i).Mac.toUpper());
            CurrentNetwork->insert("IP", activeEntrys.at(i).IP);
            CurrentNetwork->insert("Mask", activeEntrys.at(i).Mask);
            CurrentNetwork->insert("Gateway", activeEntrys.at(i).Gateway);
            CurrentNetwork->insert("PrimacyDNS", activeEntrys.at(i).PrimacyDNS);
            CurrentNetwork->insert("SecondDNS", activeEntrys.at(i).SecondDNS);
        }
    }
    cfg->insert("CurrentNetwork", CurrentNetwork);
    return true;
}
