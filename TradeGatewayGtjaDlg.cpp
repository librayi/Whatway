
// TradeGatewayGtjaDlg.cpp : 实现文件
//

#include "stdafx.h"

#include <atomic>

#include <boost/format.hpp>

// ICU
#include <unicode/putil.h>
#include <unicode/ucnv.h>


#include "TradeGatewayGtja.h"
#include "TradeGatewayGtjaDlg.h"

#include "./business/szkingdom_win/Encrypt.h"



#include "./connectpool/ConnectManager.h"
#include "./business/apexsoft/dingdian.h"

#include "./cache/CacheData.h"
#include "./encrypt/MyBotan.h"

#include "log/mq/LogConnect.h"

#include "./output/FileLog.h"
#include "log/FileLogManager.h"
#include "./log/DistributedLogManager.h"
#include "log/mq/LogConnectPool.h"

#include "business\SunGuard\SywgConnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTradeGatewayGtjaDlg 对话框




CTradeGatewayGtjaDlg::CTradeGatewayGtjaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTradeGatewayGtjaDlg::IDD, pParent)
	, m_sSslAddr(_T(""))
	, m_sSslStatus(_T(""))
	, m_sSslWorker(_T(""))
	, m_sSslSend(_T(""))
	, m_sConnPoolMin(_T(""))
	, m_sLogFilePath(_T(""))
	, m_sLogDbConnPoolMin(_T(""))
	, m_sLogGuiShowcount(_T(""))
	, m_sLogLevel(_T(""))
	, m_nLogFileEnable(0)
	, m_sLogFileThreadPool(_T(""))
	, m_sLogDbThreadPool(_T(""))
	, m_sLogMqThreadPool(_T(""))
	, m_sLogShowThreadPool(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTradeGatewayGtjaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SSL_ADDR, m_sSslAddr);
	DDX_Text(pDX, IDC_SSL_STATUS, m_sSslStatus);
	DDX_Text(pDX, IDC_SSL_WORKER, m_sSslWorker);
	DDX_Text(pDX, IDC_SSL_SEND, m_sSslSend);

	DDX_Text(pDX, IDC_HTTP_ADDR, m_sHttpAddr);
	DDX_Text(pDX, IDC_HTTP_STATUS, m_sHttpStatus);
	DDX_Text(pDX, IDC_HTTP_WORKER, m_sHttpWorker);
	DDX_Text(pDX, IDC_HTTP_SEND, m_sHttpSend);

	DDX_Text(pDX, IDC_TCP_ADDR, m_sTcpAddr);
	DDX_Text(pDX, IDC_TCP_STATUS, m_sTcpStatus);
	DDX_Text(pDX, IDC_TCP_WORKER, m_sTcpWorker);
	DDX_Text(pDX, IDC_TCP_SEND, m_sTcpSend);

	DDX_Text(pDX, IDC_CONNPOOL_MIN, m_sConnPoolMin);
	DDX_Text(pDX, IDC_CONNPOOL_INCREASE, m_sConnPoolIncrease);
	//DDX_Text(pDX, IDC_CONNPOOL_MAX, m_sConnPoolMax);
	DDX_Text(pDX, IDC_CONN_TIMEOUT, m_sConnTimeout);
	DDX_Text(pDX, IDC_CONN_RETRY, m_sConnRetry);
	DDX_Text(pDX, IDC_CONN_IDLE_TIMEOUT, m_sConnIdleTimeout);
	DDX_Text(pDX, IDC_BUSI_RETRY, m_sBusiRetry);

	DDX_Text(pDX, IDC_LOG_FILE_PATH, m_sLogFilePath);

	DDX_Text(pDX, IDC_LOG_DB, m_sLogDb);
	DDX_Text(pDX, IDC_DB_CONNPOOL_MIN, m_sLogDbConnPoolMin);
	DDX_Text(pDX, IDC_DB_CONNPOOL_INCREASE, m_sLogDbConnPoolIncrease);
	DDX_Text(pDX, IDC_DB_CONNPOOL_MAX, m_sLogDbConnPoolMax);

	DDX_Text(pDX, IDC_LOG_GUI_SHOWCOUNT, m_sLogGuiShowcount);
	DDX_Text(pDX, IDC_LOG_LEVEL, m_sLogLevel);

	DDX_Check(pDX, IDC_LOG_FILE_ENABLE, m_nLogFileEnable);
	DDX_Check(pDX, IDC_LOG_DB_ENABLE, m_nLogDBEnable);
	DDX_Check(pDX, IDC_LOG_MQ_ENABLE, m_nLogMQEnable);
	DDX_Check(pDX, IDC_LOG_GUI_ENABLE, m_nLogGUIEnable);
	DDX_Text(pDX, IDC_LOG_FILE_WORKER, m_sLogFileThreadPool);
	DDX_Text(pDX, IDC_LOG_DB_WORKER, m_sLogDbThreadPool);
	DDX_Text(pDX, IDC_LOG_MQ_WORKER, m_sLogMqThreadPool);
	DDX_Text(pDX, IDC_LOG_SHOW_WORKER, m_sLogShowThreadPool);
}

BEGIN_MESSAGE_MAP(CTradeGatewayGtjaDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	

	ON_BN_CLICKED(IDC_OPEN_LOG, &CTradeGatewayGtjaDlg::OnBnClickedOpenLog)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_EXIT, &CTradeGatewayGtjaDlg::OnBnClickedExit)
//	ON_BN_CLICKED(IDC_TEST_CRASH, &CTradeGatewayGtjaDlg::OnBnClickedTestCrash)
	//ON_BN_CLICKED(IDC_BUTTON3, &CTradeGatewayGtjaDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_CLEAR_CACHE, &CTradeGatewayGtjaDlg::OnBnClickedClearCache)
	ON_BN_CLICKED(IDC_BUTTON1, &CTradeGatewayGtjaDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHANGELOG, &CTradeGatewayGtjaDlg::OnBnClickedChangelog)
END_MESSAGE_MAP()


// CTradeGatewayGtjaDlg 消息处理程序

BOOL CTradeGatewayGtjaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	

	// 初始始化界面
	boost::format fmt("%1%：%2%:%3%");
	boost::format fmt2("%1%：%2%");
	//boost::str();

	fmt % "桌面和移动平台交易网关" % gConfigManager::instance().m_sIp % gConfigManager::instance().m_nSslPort;
	m_sSslAddr = fmt.str().c_str();
	m_sSslStatus = "运行状态：停止";
	fmt2 % "请求队列工作线程数" % gConfigManager::instance().m_nSslWorkerThreadPool;
	m_sSslWorker = fmt2.str().c_str();
	fmt2 % "响应队列工作线程数" % gConfigManager::instance().m_nSslSendThreadPool;
	m_sSslSend = fmt2.str().c_str();

	fmt % "flash交易网关" % gConfigManager::instance().m_sIp % gConfigManager::instance().m_nHttpPort;
	m_sHttpAddr = fmt.str().c_str();
	m_sHttpStatus = "运行状态：停止";
	fmt2 % "请求队列工作线程数" % gConfigManager::instance().m_nHttpWorkerThreadPool;
	m_sHttpWorker = fmt2.str().c_str();
	fmt2 % "响应队列工作线程数" % gConfigManager::instance().m_nHttpSendThreadPool;
	m_sHttpSend = fmt2.str().c_str();

	fmt % "页面交易网关" % gConfigManager::instance().m_sIp % gConfigManager::instance().m_nTcpPort;
	m_sTcpAddr = fmt.str().c_str();
	m_sTcpStatus = "运行状态：停止";
	fmt2 % "请求队列工作线程数" % gConfigManager::instance().m_nTcpWorkerThreadPool;
	m_sTcpWorker = fmt2.str().c_str();
	fmt2 % "响应队列工作线程数" % gConfigManager::instance().m_nTcpSendThreadPool;
	m_sTcpSend = fmt2.str().c_str();

	fmt2 % "柜台连接池最小值" % gConfigManager::instance().m_nConnectPoolMin;
	m_sConnPoolMin = fmt2.str().c_str();
	fmt2 % "柜台连接池增长值" % gConfigManager::instance().m_nConnectPoolIncrease;
	m_sConnPoolIncrease = fmt2.str().c_str();
	
	//fmt2 % "连接超时时间" % gConfigManager::instance().m_nConnectTimeout;
	//m_sConnTimeout = fmt2.str().c_str();
	
	fmt2 % "连接失败重试次数" % gConfigManager::instance().m_nConnectRetry;
	m_sConnRetry = fmt2.str().c_str();
	//fmt2 % "连接空闲超时时间" % gConfigManager::instance().m_nIdleTimeout;
	//m_sConnIdleTimeout = fmt2.str().c_str();
	fmt2 % "业务失败重试次数" % gConfigManager::instance().m_nBusinessRetry;
	m_sBusiRetry = fmt2.str().c_str();

	fmt2 % "日志文件目录" % gConfigManager::instance().m_sLogFilePath;
	m_sLogFilePath = fmt2.str().c_str();
	
	boost::format fmt3("数据库信息：%1%@%2%");
	fmt3 % gConfigManager::instance().m_sLogDbUserName % gConfigManager::instance().m_sLogDbServiceName;
	m_sLogDb = fmt3.str().c_str();
	
	fmt2 % "数据库连接池最小值" % gConfigManager::instance().m_nLogDbMin;
	m_sLogDbConnPoolMin = fmt2.str().c_str();
	fmt2 % "数据库连接池增长值" % gConfigManager::instance().m_nLogDbIncrease;
	m_sLogDbConnPoolIncrease = fmt2.str().c_str();
	fmt2 % "数据库连接池最大值" % gConfigManager::instance().m_nLogDbMax;
	m_sLogDbConnPoolMax = fmt2.str().c_str();

	fmt2 % "显示记录数" % gConfigManager::instance().m_nLogGuiShowCount;
	m_sLogGuiShowcount = fmt2.str().c_str();


	switch (gConfigManager::instance().m_nLogLevel)
	{
	case 0:
		m_sLogLevel = "日志输出级别：调试";
		break;
	case 1:
		m_sLogLevel = "日志输出级别：信息";
		break;
	case 2:
		m_sLogLevel = "日志输出级别：警告";
		break;
	case 3:
		m_sLogLevel = "日志输出级别：错误";
		break;
	}

	m_nLogFileEnable = gConfigManager::instance().m_nLogFileEnable;
	m_nLogDBEnable = gConfigManager::instance().m_nLogDbEnable;
	m_nLogMQEnable = gConfigManager::instance().m_nLogMqEnable;
	m_nLogGUIEnable = gConfigManager::instance().m_nLogGuiEnable;

	fmt2 % "文件日志系统线程池大小" % gConfigManager::instance().m_nLogFileThreadPool;
	m_sLogFileThreadPool = fmt2.str().c_str();
	fmt2 % "数据库日志系统线程池大小" % gConfigManager::instance().m_nLogDbThreadPool;
	m_sLogDbThreadPool = fmt2.str().c_str();
	fmt2 % "分布式日志系统线程池大小" % gConfigManager::instance().m_nLogMqThreadPool;
	m_sLogMqThreadPool = fmt2.str().c_str();
	fmt2 % "界面日志系统线程池大小" % gConfigManager::instance().m_nLogShowThreadPool;
	m_sLogShowThreadPool = fmt2.str().c_str();

	UpdateData(false);

	

	
	//运行即启动
	start();

	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTradeGatewayGtjaDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTradeGatewayGtjaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 打开日志目录
void CTradeGatewayGtjaDlg::OnBnClickedOpenLog()
{
	ShellExecute(NULL, "open", gConfigManager::instance().m_sLogFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

// 得到cpu内核数
int CTradeGatewayGtjaDlg::GetCPUCount()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	return systemInfo.dwNumberOfProcessors * 2;

}

void CTradeGatewayGtjaDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ÔÚ´Ë´¦Ìí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂë
}

void CTradeGatewayGtjaDlg::OnClose()
{
	// 释放资源，防止内存泄漏
	stop();

	CDialog::OnClose();
}


void CTradeGatewayGtjaDlg::start()
{
	// 分布式日志
	if (gConfigManager::instance().m_nLogMqEnable)
	{
		if(!gLogConnectPool::instance().CreateConnectPool())
		{
			AfxMessageBox("创建分布式日志连接池失败");
			return;
		}
	}

	//运行检测验证码过期的线程
	//captcha::instance().StartClearExpireCaptchaThread();

	// 启动缓存线程，用于清除缓存数据
	g_CacheData.Start();

	//文件日志
	gFileLogManager::instance().start();
	gFileLogManager::instance().LoadFieldFilter();
	gFileLogManager::instance().LoadFuncFilter();

	// 分布式日志
	if (gConfigManager::instance().m_nLogMqEnable)
	{
		gDistributedLogManager::instance().start();
		gDistributedLogManager::instance().LoadFieldFilter();
		gDistributedLogManager::instance().LoadFuncFilter();
	}

	/*
	pTradeServerHttp = new trade_server_http();
	piospool_http = new io_service_pool(GetCPUCount());
	pHttpServer = new http_server(*piospool_http, gConfigManager::instance().m_nHttpPort, pTradeServerHttp->recv_queue());
	pTradeServerHttp->start();
	pHttpServer->start();
	m_sHttpStatus = "运行状态：正在运行";
	UpdateData(FALSE);
	*/

	// tcp 4字节
	if (gConfigManager::instance().m_nTcpEnable)
	{
		pTradeServerTcpOld = new TradeServer(MSG_TYPE_TCP_OLD);

		iospool_tcp_old = new io_service_pool(GetCPUCount());
		pTcpOldServer = new TcpServer(*iospool_tcp_old, gConfigManager::instance().m_nTcpPort, pTradeServerTcpOld->recv_queue(), MSG_TYPE_TCP_OLD);
	

		pTradeServerTcpOld->start();
		pTcpOldServer->start();
		m_sTcpStatus = "运行状态：正在运行";
		UpdateData(FALSE);
	}

	// ssl pb 
	if (gConfigManager::instance().m_nSslEnable)
	{
		pTradeServerSslOld = new TradeServer(MSG_TYPE_SSL_PB);

		iospool_ssl_old = new io_service_pool(GetCPUCount());
		pSslOldServer = new SSLServer(*iospool_ssl_old, gConfigManager::instance().m_nSslPort, pTradeServerSslOld->recv_queue(), MSG_TYPE_SSL_PB);
	

		pTradeServerSslOld->start();
		pSslOldServer->start();
		m_sSslStatus = "运行状态：正在运行";
		UpdateData(FALSE);
	}
	

	// tcp new
	if (gConfigManager::instance().m_nTcpNewEnable)
	{
		pTradeServerTcpNew = new TradeServer(MSG_TYPE_TCP_NEW);

		iospool_tcp_new = new io_service_pool(GetCPUCount());
		pTcpNewServer = new TcpServer(*iospool_tcp_new, gConfigManager::instance().m_nTcpNewPort, pTradeServerTcpNew->recv_queue(), MSG_TYPE_TCP_NEW);
	

		pTradeServerTcpNew->start();
		pTcpNewServer->start();
		//m_sTcpStatus = "运行状态：正在运行";
		UpdateData(FALSE);
	}
	


	// ssl new 
	if (gConfigManager::instance().m_nSslNewEnable)
	{
		pTradeServerSslNew = new TradeServer(MSG_TYPE_SSL_NEW);

		iospool_ssl_new = new io_service_pool(GetCPUCount());
		pSslNewServer = new SSLServer(*iospool_ssl_new, gConfigManager::instance().m_nSslNewPort, pTradeServerSslNew->recv_queue(), MSG_TYPE_TCP_NEW);
	

		pTradeServerSslNew->start();
		pSslNewServer->start();
		//m_sTcpStatus = "运行状态：正在运行";
		UpdateData(FALSE);
	}
	
	

	gFileLog::instance().Log("交易网关启动");

}

void CTradeGatewayGtjaDlg::stop()
{
	// 文件日志
	gFileLogManager::instance().stop();

	// 分布式日志
	if (gConfigManager::instance().m_nLogMqEnable)
	{
		gLogConnectPool::instance().CloseConnectPool();
		gDistributedLogManager::instance().stop();
	}

	if (gConfigManager::instance().m_nTcpEnable)
	{
		if (pTcpOldServer != NULL)
		{
			pTcpOldServer->stop();
			delete pTcpOldServer;
		}
		if (pTradeServerTcpOld != NULL)
		{
			pTradeServerTcpOld->stop();
			delete pTradeServerTcpOld;
		}
		if (iospool_tcp_old != NULL)
		{
			iospool_tcp_old->stop();
			delete iospool_tcp_old;
		}
	}

	if (gConfigManager::instance().m_nSslEnable)
	{
		if (pSslOldServer != NULL)
		{
			pSslOldServer->stop();
			delete pSslOldServer;
		}
		if (pTradeServerSslOld != NULL)
		{
			pTradeServerSslOld->stop();
			delete pTradeServerSslOld;
		}
		if (iospool_ssl_old != NULL)
		{
			iospool_ssl_old->stop();
			delete iospool_ssl_old;
		}
	}
	
	if (gConfigManager::instance().m_nTcpNewEnable)
	{
		if (pTcpNewServer != NULL)
		{
			pTcpNewServer->stop();
			delete pTcpNewServer;
		}
		if (pTradeServerTcpNew != NULL)
		{
			pTradeServerTcpNew->stop();
			delete pTradeServerTcpNew;
		}
		if (iospool_tcp_new != NULL)
		{
			iospool_tcp_new->stop();
			delete iospool_tcp_new;
		}
	}

	if (gConfigManager::instance().m_nSslNewEnable)
	{
		if (pSslNewServer != NULL)
		{
			pSslNewServer->stop();
			delete pSslNewServer;
		}
		if (pTradeServerSslNew != NULL)
		{
			pTradeServerSslNew->stop();
			delete pTradeServerSslNew;
		}
		if (iospool_ssl_new != NULL)
		{
			iospool_ssl_new->stop();
			delete iospool_ssl_new;
		}
	}
	
}

// 退出按钮事件
void CTradeGatewayGtjaDlg::OnBnClickedExit()
{
	
	
//	g_ConnectManager.CloseConnPool();

	stop();

	EndDialog(0);
}

// 手工清除缓存数据
void CTradeGatewayGtjaDlg::OnBnClickedClearCache()
{
	// TODO: 在此添加控件通知处理程序代码
	g_CacheData.m_mapCacheData.clear();
}


void CTradeGatewayGtjaDlg::OnBnClickedTestCrash()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	g_MyBotan.init();

	std::string plain;
	std::string cipher = "e60ebaace7ef3fb88b07e0096204e0ba";
	bool bRet = g_MyBotan.Base16Decoder_AESDecrypt("AES-256/ECB/PKCS7", "29dlo*%AO+3i16BaweTw.lc!)61K{9^5", cipher, plain);
	
//	gMQConnect::instance().Connect("127.0.0.1", 8080);

//	std::string log = "hello the world";
//	gMQConnect::instance().Send(log);

//	gMQConnect::instance().Close();
	
	//int *p = NULL;
	//*p = 0;
	//OracleConnPool::Instance()->CreateConnPool("stockpool", "stockpool", "stockpooldb", 1, 1, 1);
	//
//	bool bRet = g_OracleConnPool.CreateConnPool("trade", "trade", "xe", 1, 1, 1);
	//if (bRet)
	//	TRACE("success\n");
}


void CTradeGatewayGtjaDlg::OnBnClickedButton3()
{
	/*
	OCI_Connection * cn = NULL;
	cn = DBConnPool::instance().GetConnect();

	OCI_Statement *st  = OCI_StatementCreate(cn);

	OCI_Statement* st2 = OCI_StatementCreate(cn);


create or replace package body pkg_test
as
 procedure read_rows (header varchar2, result out type_cursor)
 is
  sqlText varchar2(500);
 begin
  if header is null or length(header)=0 then
   sqlText := 'select * from test_pkg_test';
  else
   sqlText := 'select * from test_pkg_test where substr(name,1,' || to_char(length(header)) || ')=''' || header || '''';
  end if;
  --dbms_output.put_line(sqlText);
  open result for sqlText;
 end read_rows;
end pkg_test;


	std::string header = "";

	OCI_Prepare(st, "BEGIN pkg_test.read_rows(:header, :c); END;");

	OCI_BindString(st, ":header", (char*)header.c_str(), header.length());
	OCI_BindStatement(st, ":c", st2); // out类型



	if (OCI_Execute(st))
	{
		OCI_Resultset  *rs;
		rs = OCI_GetResultset(st2);
		while (OCI_FetchNext(rs))
		{
			int id = OCI_GetInt(rs, 1);
			std::string name = OCI_GetString(rs, 2);
			TRACE("id=%d    name=%s\n", id, name.c_str());
		}

	}

	OCI_StatementFree(st); 
	OCI_StatementFree(st2); 
	DBConnPool::instance().FreeConnect(cn);
	*/
}




void CTradeGatewayGtjaDlg::OnBnClickedButton1()
{
	char * p = NULL;
	strcpy(p, "000");
	/*
	CSywgConnect test;

	SWIMoney money;
	test.char_Int64(&money, "1000");

	
	std::string sysNo = "jlp";
	if (sysNo.find("njzq") == std::string::npos)
	{
		TRACE("not found");
	}
	else
	{
		TRACE("not found");
	}
	*/
	/*

	boost::posix_time::ptime ptSendTime = boost::posix_time::microsec_clock::local_time();
	int hour = ptSendTime.time_of_day().hours();
	int minute = ptSendTime.time_of_day().minutes();

	

	double val = 0/1000.0;

	std::ostringstream osbuf2;    
	osbuf2 << std::setiosflags(std::ios::fixed) << std::setprecision(2);
	osbuf2 << val;
	std::string tmp = osbuf2.str();
	*/

	/*
	std::string buf = "10.01";
	std::string result = "";
	std::string integer = "";
	std::string decimal = "";

	std::vector<std::string> kv;
	boost::split(kv, buf, boost::is_any_of("."));

	// 没有小数点
	if (kv.size() == 1)
	{
		integer = kv[0];
		decimal = "0000";
		result = integer + decimal;
	}

	if (kv.size() == 2)
	{
		integer = kv[0];

		decimal = kv[1];
		for (int i = decimal.length(); i < 4; i++)
		{
			decimal += "0";
		}

		result = integer + decimal;
	}

	int64 t;
	t = _atoi64(result.c_str());
	*/
	

/*
	if (kv.size() < 2)
		decimal = "0000";
		
		if (!kv[0].empty())
			key = kv[0];

		
		if (!kv[1].empty())
			value = kv[1];
			*/

	/*
	bool bRet = false;
	LogConnect * logConnect = new LogConnect();	
	bRet = logConnect->Connect("127.0.0.1", 6002);

	std::string request = "test";
	std::string response = "";

	bRet = logConnect->Send(request, response);

	UErrorCode errcode = U_ZERO_ERROR;
	char dest[8192];
	memset(dest, 0x00, sizeof(dest));
	int ret = ucnv_convert("gbk", "utf8", dest, sizeof(dest), response.c_str(), -1, &errcode);
	*/
}



void CTradeGatewayGtjaDlg::OnBnClickedChangelog()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string changeLog = "";
	changeLog = gConfigManager::instance().m_sPath + "\\ChangeLog.txt";
	ShellExecute(NULL, "open", changeLog.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
