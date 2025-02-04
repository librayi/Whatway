
// TradeGatewayGtja.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"

//内存泄漏检查
//#include <vld.h>

#include <CrashRpt.h>




#include "TradeGatewayGtja.h"
#include "TradeGatewayGtjaDlg.h"

#include "./config/configmanager.h"

#include "./business/szkingdom_win/Encrypt.h"
#include "./business/apexsoft/DingDian.h"



#include "business/hundsun_com/CComm.h"


#include "./output/FileLog.h"




#include "./business/SunGuard/AGC.h"
#include "errcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTradeGatewayGtjaApp

BEGIN_MESSAGE_MAP(CTradeGatewayGtjaApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTradeGatewayGtjaApp 构造

CTradeGatewayGtjaApp::CTradeGatewayGtjaApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CTradeGatewayGtjaApp 对象

CTradeGatewayGtjaApp theApp;


// CTradeGatewayGtjaApp 初始化

BOOL WINAPI CrashCallback(LPVOID lpvState)
{
	char szPath[256];  
	GetModuleFileName(NULL,szPath,256);//得到程序模块名称，全路径    
	ShellExecuteA(NULL,"open",szPath,NULL,NULL,SW_SHOWNORMAL); 
	
	return TRUE;

}


BOOL CTradeGatewayGtjaApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	
	// 读配置文件
	gConfigManager::instance().LoadConfig();
	gConfigManager::instance().LoadCrossDomain();

	


	// 崩溃处理
	crashDump.DontShowUI();
	//crashDump.BeginDump();
	
	std::string crash = gConfigManager::instance().m_sPath + "\\crashdump";
	CR_INSTALL_INFO info;  
	memset(&info, 0, sizeof(CR_INSTALL_INFO));  

	info.cb = sizeof(CR_INSTALL_INFO);    
	info.pszAppName = _T("Cssweb TradeGateway");  
	info.pszAppVersion = _T("2.0.01");  
	
	//info.pszEmailSubject = _T("MyApp 1.0.0 Error Report");  
	//info.pszEmailTo = _T("myapp_support@hotmail.com");    

	//info.pszUrl = _T("http://myapp.com/tools/crashrpt.php");  
	//info.pfnCrashCallback = CrashCallback;   
	//info.uPriorities[CR_HTTP] = 3;  // First try send report over HTTP 
	//info.uPriorities[CR_SMTP] = 2;  // Second try send report over SMTP  
	//info.uPriorities[CR_SMAPI] = 1; // Third try send report over Simple MAPI    
	// Install all available exception handlers, use HTTP binary transfer encoding (recommended).
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	info.dwFlags |= CR_INST_NO_GUI; 
	info.dwFlags |= CR_INST_DONT_SEND_REPORT; 
	//info.dwFlags |= CR_INST_APP_RESTART; 

	// Define the Privacy Policy URL 
	//info.pszPrivacyPolicyURL = _T("http://myapp.com/privacypolicy.html"); 

	info.pszErrorReportSaveDir = crash.c_str();

	//info.pszRestartCmdLine = _T("/restart");
  
	// Install exception handlers
	int nResult = crInstall(&info);    
	if (nResult != 0)
	{
		char szErrorMsg[512] = "";
		crGetLastErrorMsg(szErrorMsg, 512);   
		AfxMessageBox(szErrorMsg);
		//return FALSE;
	}
	


	// 金证版本加载dogskin.dll
	if (!g_Encrypt.LoadDLL())
		return FALSE;

	/*
	if (gConfigManager::instance().m_nLogDbEnable)
	{
		if (!DBConnPool::instance().CreateConnPool(gConfigManager::instance().m_sLogDbUserName, 
			gConfigManager::instance().m_sLogDbPassword,
			gConfigManager::instance().m_sLogDbServiceName,
			gConfigManager::instance().m_nLogDbMin,
			gConfigManager::instance().m_nLogDbMax,
			gConfigManager::instance().m_nLogDbIncrease))
		{
			AfxMessageBox("建立数据库连接失败");
			return FALSE;
		}
	}
	*/

	// 顶点柜台初始化
	g_DingDian.ReadMapFile();
	g_DingDian.ReadOutDomainFile();

//	sLogManager::instance().LoadFieldFilter();
//	sLogManager::instance().LoadFuncFilter();


	
	// 加载agc xml定义
	gAGC.LoadXML();

	// 错误码定义
	gError::instance().init();

	if (gConfigManager::instance().m_nLogMqEnable)
	{
//		if (!gMQManager::instance().CreateConnectPool())
//		{
//			AfxMessageBox("建立分布式日志系统服务器失败!");
//			return FALSE;
//		}
	}

	


	CTradeGatewayGtjaDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CTradeGatewayGtjaApp::ExitInstance()
{
	google::protobuf::ShutdownProtobufLibrary();

//	gMQManager::instance().CloseConnectPool();

	// 释放dogskin.dll
	g_Encrypt.UnLoadDLL();
	
	g_DingDian.UnInit();



	// 崩溃处理
	crUninstall();

	return CWinApp::ExitInstance();
}

int CTradeGatewayGtjaApp::Run()
{
	// Call your crInstall code here ...

	return CWinApp::Run();
}
/*
CCriticalSection g_csPackID;
 long	g_lPackID;
 */