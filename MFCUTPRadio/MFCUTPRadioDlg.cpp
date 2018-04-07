
// MFCUTPRadioDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCUTPRadio.h"
#include "MFCUTPRadioDlg.h"
#include "afxdialogex.h"
//excel ��Ҫ�õ���ͷ�ļ�
#include "CApplication.h"
#include "CFont0.h"
#include"CRange.h"
#include"CWorkbook.h"
#include"CWorkbooks.h"
#include"CWorksheet.h"
#include"CWorksheets.h"

#include<vector>
using namespace std;

#define RESULT_ERROR -1
#define  RESULT_OK 0
#define UDP_PACKET_BUF_LEN 256

#pragma comment(lib,"ws2_32.lib")
const int MAX_BUF_LEN = 255;

#define CLIENT_PORT 65003
#define SERVER_PORT 65001
#define TCPCLIENT_PORT 64001

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//excel����ӿ������ 
//CApplication app;
//CWorkbook book;
//CWorkbooks books;
//CWorksheet sheet;
//CWorksheets sheets;
//CRange range;
//CFont0 font;
//CRange cols;
//LPDISPATCH lpDisp;



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCUTPRadioDlg �Ի���



CMFCUTPRadioDlg::CMFCUTPRadioDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCUTPRADIO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCUTPRadioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
}

BEGIN_MESSAGE_MAP(CMFCUTPRadioDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_UDP, &CMFCUTPRadioDlg::OnBnClickedButtonUdp)
	ON_BN_CLICKED(IDC_BUTTON_TCP, &CMFCUTPRadioDlg::OnBnClickedButtonTcp)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CMFCUTPRadioDlg::OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_OUTDATA, &CMFCUTPRadioDlg::OnBnClickedButtonOutdata)
	ON_BN_CLICKED(IDC_BUTTON_SNNUMBER, &CMFCUTPRadioDlg::OnBnClickedButtonSnnumber)
	ON_BN_CLICKED(IDC_BUTTON_GETDATA, &CMFCUTPRadioDlg::OnBnClickedButtonGetdata)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMFCUTPRadioDlg::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON_SNNUMBER2, &CMFCUTPRadioDlg::OnBnClickedButtonSnnumber2)
END_MESSAGE_MAP()


// CMFCUTPRadioDlg ��Ϣ�������

BOOL CMFCUTPRadioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������




	CEdit* m_Edit = (CEdit*)GetDlgItem(IDC_EDIT_SN);       // ����edit������ĳ���
	m_Edit->SetLimitText(24);
	CEdit* m_EditPort = (CEdit*)GetDlgItem(IDC_EDIT_PORT);
	m_EditPort->SetLimitText(5);

	OnClentInit();   
	HANDLE hThread = CreateThread(NULL, 0, MyThreadUDP, this, 0, NULL);
	SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
	if (hThread)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}

	HANDLE hThread2 = CreateThread(NULL, 0, MyThreadRecv, this, 0, NULL);
	SetThreadPriority(hThread2, THREAD_PRIORITY_BELOW_NORMAL);
	if (hThread2)
	{
		CloseHandle(hThread2);
		hThread2 = NULL;
	}


	
	m_list1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);      // ����ѡ��������  
	m_list1.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	m_list1.InsertColumn(1, _T("״̬"), LVCFMT_LEFT, 50);        // �����2�е�����  
	m_list1.InsertColumn(2, _T("�豸��"), LVCFMT_LEFT, 80);        // �����3�е�����  
	m_list1.InsertColumn(3, _T("SN��"), LVCFMT_LEFT, 190);        // �����4�е����� 
	m_list1.InsertColumn(4, _T("�����IP"), LVCFMT_LEFT, 120);
	m_list1.InsertColumn(5, _T("�˿�"), LVCFMT_LEFT, 60);
	m_list1.InsertColumn(6, _T("����IP"), LVCFMT_LEFT, 120);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCUTPRadioDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCUTPRadioDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCUTPRadioDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



DWORD  CMFCUTPRadioDlg::MyThreadUDP(LPVOID pParam)
{
	CMFCUTPRadioDlg *p = (CMFCUTPRadioDlg*)pParam;
	p->UDPStart();
	//p->ValidateRece();
	return 0;
}

DWORD  CMFCUTPRadioDlg::MyThreadRecv(LPVOID pParam)
{
	CMFCUTPRadioDlg *p = (CMFCUTPRadioDlg*)pParam;
	p->ValidateRece();
}


void CMFCUTPRadioDlg::OnClentInit() {
	int nPort = SERVER_PORT;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	// ����socket api   
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return;
	}
	// ����socket   
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connect_socket)
	{
		err = WSAGetLastError();
		CString s;
		s.Format("\"socket\" error! error code is %d", err);
		MessageBox(s);
		return;
	}
	// �������׽���   
	SOCKADDR_IN sin; 
	sin.sin_family = AF_INET;
	sin.sin_port = htons(CLIENT_PORT);
	sin.sin_addr.s_addr = 0;
	// �����������ϵĹ㲥��ַ��������   
	sin_from.sin_family = AF_INET;
	sin_from.sin_port = htons(nPort);
	sin_from.sin_addr.s_addr = INADDR_BROADCAST;
	//���ø��׽���Ϊ�㲥���ͣ�   
	bool bOpt = true;
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));
	// ���׽���   
	err = bind(connect_socket, (SOCKADDR*)&sin, sizeof(SOCKADDR));
	if (SOCKET_ERROR == err)
	{
		err = WSAGetLastError();
		CString s;
		s.Format("\"bind\" error! error code is %d", err);
		return;
	}
	
	
}


int ChangeNum(CString str, int length)
{
	char  revstr[16] = { 0 }; //����ʮ�������ַ����ĳ��ȣ�����ע�����鲻ҪԽ�� 
	int   num[16] = { 0 };
	int   count = 1;
	int   result = 0;
	strcpy(revstr, str);
	for (int i = length - 1; i >= 0; i--)
	{
		if ((revstr[i] >= '0') && (revstr[i] <= '9'))
			num[i] = revstr[i] - 48;//�ַ�0��ASCIIֵΪ48 
		else if ((revstr[i] >= 'a') && (revstr[i] <= 'f'))
			num[i] = revstr[i] - 'a' + 10;
		else if ((revstr[i] >= 'A') && (revstr[i] <= 'F'))
			num[i] = revstr[i] - 'A' + 10;
		else
			num[i] = 0;
		result = result + num[i] * count;
		count = count * 16;//ʮ������(����ǰ˽��ƾ����������8)    
	}
	return result;
}


void CMFCUTPRadioDlg::ValidateRece()
{
	BYTE byteRece[50];
	while (true)
	{
		m_sock.Receive(byteRece, sizeof(byteRece));
	}
}

void CMFCUTPRadioDlg::UDPStart()
{
	CString strAddr;
	SOCKADDR_IN clientAddr;
	int nAddrLen = sizeof(SOCKADDR);
	BYTE buffer[MAX_BUF_LEN] = "";
	
	while (true)
	{
		// ��������   
		int nSendSize = recvfrom(connect_socket, (char *)buffer, MAX_BUF_LEN, 0, (struct sockaddr FAR *)&clientAddr, (int FAR *)&nAddrLen);
		if (SOCKET_ERROR != nSendSize)
		{
			char    *pIPAddr = inet_ntoa(clientAddr.sin_addr);
			if (NULL != pIPAddr)
			{
				WCHAR    wzIPBuffer[32] = { 0 };
				printf("clientAddr: %s\n", pIPAddr);  //��ӡ��ַ
				strAddr = pIPAddr;
				//MessageBox(s);
			}
		}
		if (SOCKET_ERROR == nSendSize)
		{
			int err = WSAGetLastError();
			printf("\"recvfrom\" error! error code is %d\n", err);
			return ;
		}
		CString s = "received data:";
		
		char temp[10];
		/*char buff[MAX_BUF_LEN] = "";
		for (int i = 0; i < nSendSize; i++)
		{
			sprintf(temp,"%x", buffer[i]);
			strcat(buff, temp);
		}
		s += buff;
		MessageBox(s);*/ 

		// ���յ�һ�����ݾ��������һ������
		char name[MAX_BUF_LEN] = "";
		int namelength = buffer[7] + buffer[8];  //���ֳ���
		CString strName;
		for (int i = 9; i < namelength + 9; i++)
		{
			CString str;
			str.Format("%c", buffer[i]);
			strName += str;
		}
		m_list1.InsertItem(0, "");                          // ������  

		m_list1.SetItemText(0,1,"δ����");					// 1����״̬

		m_list1.SetItemText(0, 2, strName);                 // ���õ�2��(����) 

		//int snNum = namelength + 9 + 6;				// ��ǰ��ʹ��6���ֽڵ�
		int snNum = namelength + 9 + 12;			// �ָĳ�12���ֽ�
		char snBuff[MAX_BUF_LEN] = "";
		for (int i = namelength + 9; i < snNum; i++)
		{
			sprintf(temp, "%02X", buffer[i]);
			strcat(snBuff, temp);
		}
		m_list1.SetItemText(0,3,snBuff);                 // ���õ�����(SN��) 

		CString strSIp;
		int sIp = snNum + 4;
		for (int i = snNum; i<sIp; i++)
		{
			CString str;
			str.Format("%d",buffer[i]);
			if (i < sIp-1) {
				str += ".";
			}
			strSIp += str;
		}
		m_list1.SetItemText(0,4,strSIp);              // 4���÷�����IP


		int iPort = sIp + 2;
		char portBuff[MAX_BUF_LEN] = "0x";
		for (int i = sIp; i < iPort; i++)
		{
			sprintf(temp, "%0x", buffer[i]);
			strcat(portBuff, temp);
		}
		CString strPort;
		strPort = portBuff;
		int n = 0;
		n = ChangeNum(strPort, strPort.GetLength());
		strPort.Format("%d", n);
		m_list1.SetItemText(0,5,strPort);            // 5���ö˿ں�

		m_list1.SetItemText(0, 6, strAddr);		   	// 6���ú���IP
	}

}


void CMFCUTPRadioDlg::OnBnClickedButtonUdp()
{
	m_list1.DeleteAllItems();
	char buff[MAX_BUF_LEN] = "";
	BYTE szMsg[] = { 0x23,0x23,0x00,0x0c,0x00,0x00,0x99,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2A,0x2A };
	int nLen = sizeof(sin_from);
	if (SOCKET_ERROR == sendto(connect_socket, (const char*)szMsg, 16/* strlen(szMsg)*/, 0, (sockaddr*)&sin_from, nLen))
	{
		AfxMessageBox("Send UDP Failed");
		return;
	}

	GetDlgItem(IDC_EDIT_NAME)->SetWindowText("");
	GetDlgItem(IDC_EDIT_IP)->SetWindowText("");
	GetDlgItem(IDC_EDIT_SN)->SetWindowText("");
	GetDlgItem(IDC_EDIT_PORT)->SetWindowText("");

	//UDPStart();
}


void CMFCUTPRadioDlg::OnBnClickedButtonTcp()
{
	/*if (INVALID_SOCKET == m_sock) {      //�ж��Ƿ��Ѿ�����socket
		if (!m_sock.Create())			 //��ʼ��TCP
		{
			CString s;
			s = "��������ʱʧ�ܣ�" + GetLastError();
			MessageBox(s);
		}
	}*/
	
	POSITION ps;
	int nIndex;
	ps = m_list1.GetFirstSelectedItemPosition();
	nIndex = m_list1.GetNextSelectedItem(ps);
	
	CString strBtn;
	//if (!m_sock.Connect(str, TCPCLIENT_PORT)) 
	GetDlgItem(IDC_BUTTON_TCP)->GetWindowTextA(strBtn);
	if(strBtn == "�Ͽ�")
	{

		GetDlgItem(IDC_BUTTON_TCP)->SetWindowText("����");
		GetDlgItem(IDC_BUTTON_UDP)->EnableWindow(TRUE);

		m_sock.Close();
		m_list1.SetItemText(nIndex, 1, "δ����");				// ����1״̬
		KillTimer(1);			// ֹͣ��������

		return;
	}
	else
	{

		CString strBoxName, strIP, strPort, strName, strSN;
		m_TcpIP = strBoxName = m_list1.GetItemText(nIndex, 6);			// ��ȡ6����IP
		strName = m_list1.GetItemText(nIndex, 2);       // ��ȡ2����
		strIP = m_list1.GetItemText(nIndex, 4);         // ��ȡ4������IP
		strSN = m_list1.GetItemText(nIndex, 3);			// ��ȡ3SN��
		strPort = m_list1.GetItemText(nIndex, 5);		// ��ȡ5�˿ں�

		if (!m_sock.Create())			 //��ʼ��TCP
		{
			CString s;
			s = "��������ʱʧ�ܣ�" + GetLastError();
			MessageBox(s);
		}
		if (!m_sock.Connect(strBoxName, TCPCLIENT_PORT))  
		{
			return;
		}

		GetDlgItem(IDC_BUTTON_TCP)->SetWindowText("�Ͽ�");
		m_list1.SetItemText(nIndex, 1, "������");          // ����1״̬
		GetDlgItem(IDC_BUTTON_UDP)->EnableWindow(FALSE);


		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(strName);
		GetDlgItem(IDC_EDIT_IP)->SetWindowText(strIP);
		GetDlgItem(IDC_EDIT_SN)->SetWindowText(strSN);
		GetDlgItem(IDC_EDIT_PORT)->SetWindowText(strPort);

		SetTimer(1, 3000, NULL);    // ��������
	}
}



CString GetPartStr(CString &str, CString del) {
	int num = str.Find(del);  //"."
	CString strOne = str.Left(num);
	str = str.Mid(num + 1, str.GetLength());
	return strOne;
}



void CMFCUTPRadioDlg::OnBnClickedButtonModify()
{
	if (INVALID_SOCKET == m_sock)
	{
		MessageBox("�������豸");
		return;
	}

	// �޸����е�ѡ��
	// 23 23 00 0e 00 00 76 00 00 c0 a8 02 21 30 39 00 2a 2a  //c0 a8 02 21 �޸ķ�����IP�� 30 39 �޸Ķ˿ڣ�
	CString s,strIP,strPort,strName,strSN;
	BYTE szIPAndPort[18] = { 0x23, 0x23, 0x00, 0x0e, 0x00, 0x00, 0x76, 0x00, 0x00 };    //�Ȼ��IP�Ͷ˿�ʮ������
	GetDlgItem(IDC_EDIT_IP)->GetWindowText(strIP);	
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(strPort);
	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(strName);
	GetDlgItem(IDC_EDIT_SN)->GetWindowText(strSN);
	if (strIP.IsEmpty() || strPort.IsEmpty() || strName.IsEmpty() || strSN.IsEmpty()){
		MessageBox("������IP���˿ڡ����֡�SN��");
		return;
	}

	//if (strSN.GetLength() != 12){
	//	MessageBox("SN����12λ����");
	//	return;
	//}

	/*if (strPort.GetLength() != 5){
		MessageBox("����˿���5λ����");
		return;
	}*/

//	int iListCount = m_list1.GetItemCount();
	//char temp[10] = { "" };
	CString strNum;
	strNum = GetPartStr(strIP,".");
	int i = atoi(strNum);
	szIPAndPort[9] = (BYTE)i;
	strNum = GetPartStr(strIP, ".");
	i = atoi(strNum);
	szIPAndPort[10] = (BYTE)i;
	strNum = GetPartStr(strIP, ".");
	i = atoi(strNum);
	szIPAndPort[11] = (BYTE)i;
	GetPartStr(strIP, ".");
	i = atoi(strIP);
	szIPAndPort[12] = (BYTE)i;		//��ȡ�޸�IP

	// ��ȡ18001 1.ת��0x3039 2.ת��0x30,0x39
	i = atoi(strPort);
	CString  buff;
	strPort.Format("%04x", i);
	buff = strPort.Left(2);
	buff = "0x" + buff;
	i = ChangeNum(buff, buff.GetLength());
	szIPAndPort[13] =(BYTE)i;

	buff = strPort.Right(2);
	buff = "0x" + buff;
	i = ChangeNum(buff, buff.GetLength());  //����޸Ķ˿�
	szIPAndPort[14] = (BYTE)i;
	szIPAndPort[15] = 0x00;
	szIPAndPort[16] = 0x2a;
	szIPAndPort[17] = 0x2a;

	//m_sock.Send(szIPAndPort, 18);				 // �޸�IP �� �˿�

	//23 23 00 11 00 00 68 09 48 75 61 5a 68 6f 6e 67 31 00 00 2a 2a  //11֡���� 09 ���ֳ���
	//23 23 00 11 00 00 68                               00 00 2a 2a // 9+1+11
	//1.��ȡ���� ��ȡ���� ����ת16���� ���֡����
	BYTE byteName[50] = { 0x23,0x23,0x00 };
	int dataLength=0;
	int len = strName.GetLength();
	dataLength = len + 1 + 11 - 4;
	byteName[3] = dataLength;   //֡����
	byteName[4] = 0x00;
	byteName[5] = 0x00;
	byteName[6] = 0x68;
	byteName[7] = len;		   //���ֳ���
	char * carr = NULL;
	carr = (char*)strName.GetBuffer(strName.GetLength());
	int iLength = 8;
	int icarr = 0;
	for ( ; iLength<len+8; iLength++)
	{
		byteName[iLength] = carr[icarr++];
	}
	carr = NULL;
	byteName[iLength++] = 0x00;
	byteName[iLength++] = 0x00;
	byteName[iLength++] = 0x2a;
	byteName[iLength++] = 0x2a;


	//23 23 00 0e 00 00 70 00 00 01 17 07 31 00 05 00 2a 2a  //0e���� ��01 17 07 31 00 05 SN�� �޸�SN��
	//strSN
	//1.��ȡsn ת16���� 
	BYTE byteSN[50] = { 0x23,0x23,0x00,0x0e,0x00,0x00,0x70,0x00,0x00 };
	char* cSnArr = (char*)strSN.GetBuffer(strSN.GetLength());
	char cBuff[50] = { "" };
	char cTemp[50] = { "" };
	CString strSNBuff;
	int iNumSN = 0;
	int byteSNNum = 9;
	for (int i = 0; i < strSN.GetLength(); i++)
	{
		cTemp[0] = cSnArr[i];
		cTemp[1] = cSnArr[++i];
		strSNBuff = cTemp;
		strSNBuff = "0x" + strSNBuff;
		iNumSN = ChangeNum(strSNBuff, strSNBuff.GetLength());
		byteSN[byteSNNum++] = iNumSN;
		cBuff[0] = '\0';
	}
	byteSN[byteSNNum++] = 0x00;
	byteSN[byteSNNum++] = 0x2a;
	byteSN[byteSNNum++] = 0x2a;

	
	GetDlgItem(IDC_EDIT_IP)->GetWindowText(strIP);
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(strPort);
	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(strName);
	GetDlgItem(IDC_EDIT_SN)->GetWindowText(strSN);
	POSITION ps;
	int nIndex;
	ps = m_list1.GetFirstSelectedItemPosition();
	nIndex = m_list1.GetNextSelectedItem(ps);


	// 3����ͬ�޸�֮������Ӳ����� ����Ϊû�н��շ����������������ݷ�����������
	BYTE byteRece[50] = {""};
	if (strIP != m_list1.GetItemText(nIndex, 4) || strPort != m_list1.GetItemText(nIndex,5)) 
	{
		m_sock.Send(szIPAndPort, 18);				 // �޸�IP �� �˿�
		Sleep(500);
		//m_sock.Receive(byteRece, sizeof(byteRece));
	}

	if (strName != m_list1.GetItemText(nIndex, 2))
	{
		m_sock.Send(byteName, dataLength + 4);		 // �޸� ����
		Sleep(500);
		//m_sock.Receive(byteRece, sizeof(byteRece));
	}

	if (strSN != m_list1.GetItemText(nIndex, 3))
	{
		m_sock.Send(byteSN, 18);					 // �޸� SN��
		//Sleep(200);
		//m_sock.Receive(byteRece, sizeof(byteRece));
	}
	
	m_list1.SetItemText(nIndex, 2, strName);                 
	m_list1.SetItemText(nIndex, 3, strSN);                 
	m_list1.SetItemText(nIndex, 4, strIP);
	m_list1.SetItemText(nIndex, 5, strPort);


}

void CMFCUTPRadioDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case 1:
		//23 23 00 0c 00 00 66 00 00 00 00 00 00 00 2a 2a   //����
		BYTE byteHeart[] = { 0x23, 0x23, 0x00, 0x0c, 0x00, 0x00 ,0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x2a };
		int len = sizeof(byteHeart);
		m_sock.Send(byteHeart,len);
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

// 1.��ģ�����д������ 
// �򿪶�ȡģ�棬��ȡ���һ�У�д������

void CMFCUTPRadioDlg::OnBnClickedButtonOutdata()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CApplication app;
	CWorkbook book;
	CWorkbooks books;
	CWorksheet sheet;
	CWorksheets sheets;
	CRange range;
	CFont0 font;
	CRange cols;
	LPDISPATCH lpDisp;

	CString strName, strIP, strSN, strPort, strI, strEName, strNum;
	vector<CString> vStr,vStrN,vStrLetter;

	//exce��ʼ��
	if (!AfxOleInit())
	{
		MessageBox("��ʼ��������");
		return ;//FALSE
	}
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		this->MessageBox(_T("�޷�����ExcelӦ�ã�"));
		return ;//TRUE
	}
	books = app.get_Workbooks();

	// ��ȡ��ǰ���е�Ŀ¼
	TCHAR szPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szPath);
	CString strPath(szPath);
	strPath += "\\ģ��.xlsx";

	lpDisp = books.Open(strPath, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);
	book.AttachDispatch(lpDisp);
	sheets = book.get_Worksheets();
	sheet = sheets.get_Item(COleVariant((short)1));
	
	//��ʾExcel��

	app.put_Visible(TRUE);
	app.put_UserControl(TRUE);


}


void CMFCUTPRadioDlg::OnBnClickedButtonSnnumber()
{
	// ��ȡ������Ϊ����� 
	//GetDlgItem(IDC_EDIT_SN)->SetWindowTextA(GetTime());  ����SN��

	//1.��������   2.����
	SaveExcel();

	CopyData(IDC_EDIT_SN);

	//���渴�ƣ��������������excel�����ݴ�ӡ����




}


CString CMFCUTPRadioDlg::GetTime()
{
	CString strTime,str;
	CTime time = CTime::GetCurrentTime();   ///����CTime����
	int m_nYear = time.GetYear();      ///��
	int m_nMonth = time.GetMonth();      ///��
	int m_nDay = time.GetDay();      ///��
	int m_nHour = time.GetHour();      ///Сʱ
	int m_nMinute = time.GetMinute();   ///����
	int m_nSecond = time.GetSecond();   ///��
	strTime.Format("%d", m_nYear);
	str = strTime.Right(2);
	strTime.Format("%02d", m_nMonth);		 //%02 ��0��λ
	str += strTime;
	strTime.Format("%02d", m_nDay);
	str += strTime;
	strTime.Format("%02d", m_nHour);
	str += strTime;
	strTime.Format("%02d", m_nMinute);
	str += strTime;
	strTime.Format("%02d", m_nSecond);
	str += strTime;

	return str;
}


bool CMFCUTPRadioDlg::SaveExcel()
{


	// ��������
	CApplication app;
	CWorkbook book;
	CWorkbooks books;
	CWorksheet sheet;
	CWorksheets sheets;
	CRange range;
	CFont0 font;
	CRange cols;
	LPDISPATCH lpDisp;

	CString strName, strIP, strSN, strPort, strI, strEName, strNum;
	vector<CString> vStr, vStrN, vStrLetter, vStrData;

	GetDlgItem(IDC_EDIT_IP)->GetWindowText(strIP);
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(strPort);
	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(strName);
	GetDlgItem(IDC_EDIT_SN)->GetWindowText(strSN);


	//exce��ʼ��
	if (!AfxOleInit())
	{
		MessageBox("��ʼ��������");
		return FALSE;
	}
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		this->MessageBox(_T("�޷�����ExcelӦ�ã�"));
		return FALSE;//TRUE
	}
	books = app.get_Workbooks();

	// ��ȡ��ǰ���е�Ŀ¼
	TCHAR szPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szPath);
	CString strPath(szPath);
	strPath += "\\ģ��.xlsx";

	lpDisp = books.Open(strPath, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional);
	book.AttachDispatch(lpDisp);
	sheets = book.get_Worksheets();
	sheet = sheets.get_Item(COleVariant((short)1));
	//�������Ϊ��A��1���ĵ�Ԫ�� 
	//range = sheet.get_Range(COleVariant(_T("B2")), COleVariant(_T("B2")));
	//��õ�Ԫ������� 
	range = sheet.get_UsedRange();
	range = range.get_Rows();
	long UsedRows = range.get_Count();
	strNum.Format("%d", UsedRows + 1);

	vStrLetter.push_back("A");
	vStrLetter.push_back("B");
	vStrLetter.push_back("C");
	vStrLetter.push_back("D");
	vStrLetter.push_back("E");
	strSN = "'" + strSN;
	vStrData.push_back(strSN);
	vStrData.push_back(strName);
	vStrData.push_back(strIP);
	vStrData.push_back(strPort);  // �ж��ٸ�vSterLetter���ж��ٸ�vStrData

	CString strTime, str;
	CTime time = CTime::GetCurrentTime();   ///����CTime����
	int m_nYear = time.GetYear();      ///��
	int m_nMonth = time.GetMonth();      ///��
	int m_nDay = time.GetDay();      ///��
	int m_nHour = time.GetHour();      ///Сʱ
	int m_nMinute = time.GetMinute();   ///����
	int m_nSecond = time.GetSecond();   ///��
	strTime.Format("%d/", m_nYear);
	str = strTime;
	strTime.Format("%02d/", m_nMonth);
	str += strTime;
	strTime.Format("%02d/", m_nDay);
	str += strTime;
	strTime.Format("%02d:", m_nHour);
	str += strTime;
	strTime.Format("%02d:", m_nMinute);
	str += strTime;
	strTime.Format("%02d", m_nSecond);
	str += strTime;

	vStrData.push_back(str);


	//A3 A3,B3 B3 C3 C3,D3 D3
	for (int i = 0; i < vStrLetter.size(); i++)
	{
		strI = vStrLetter[i] + strNum;
		range = sheet.get_Range(COleVariant(strI), COleVariant(strI));
		CString sss = vStrData[i];
		range.put_Value2(COleVariant(sss)); //vStrData[i]
	}
	// excel�����ļ�����ǰĿ¼�¸���,����ʾ����
	book.Save();
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();
	books.ReleaseDispatch();
	app.Quit();
	app.ReleaseDispatch();

	return TRUE;
}


void CMFCUTPRadioDlg::OnBnClickedButtonGetdata()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION ps;
	int nIndex;
	ps = m_list1.GetFirstSelectedItemPosition();
	nIndex = m_list1.GetNextSelectedItem(ps);

	CString strBoxName, strIP, strPort, strName, strSN;
	strName = m_list1.GetItemText(nIndex, 2);       // ��ȡ2����
	strIP = m_list1.GetItemText(nIndex, 4);         // ��ȡ4������IP
	strSN = m_list1.GetItemText(nIndex, 3);			// ��ȡ3SN��
	strPort = m_list1.GetItemText(nIndex, 5);		// ��ȡ5�˿ں�

	GetDlgItem(IDC_EDIT_NAME)->SetWindowText(strName);
	GetDlgItem(IDC_EDIT_IP)->SetWindowText(strIP);
	GetDlgItem(IDC_EDIT_SN)->SetWindowText(strSN);
	GetDlgItem(IDC_EDIT_PORT)->SetWindowText(strPort);

	CString str = "https://huazhouxinxikeji.com/sn/?sn=";
	str = str + strSN;
	str = str + "&type=0";

	GetDlgItem(IDC_EDIT_DATA)->SetWindowText(str);

}


void CMFCUTPRadioDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (pNMItemActivate->iItem != -1)
	{
		OnBnClickedButtonGetdata();
	}
	*pResult = 0;
}


void CMFCUTPRadioDlg::CopyData(int number)
{
	CString str;
	GetDlgItem(number)->GetWindowTextA(str);
	if (OpenClipboard())
	{
		EmptyClipboard();

		HGLOBAL hClipboardData = GlobalAlloc(GHND | GMEM_SHARE, (str.GetLength() + 1) * sizeof(TCHAR));
		if (hClipboardData)
		{
			TCHAR* pszData = (TCHAR *)GlobalLock(hClipboardData);
			_tcscpy(pszData, str);
			GlobalUnlock(hClipboardData);
			SetClipboardData(CF_TEXT, hClipboardData);
		}

		CloseClipboard();
	}
}


void CMFCUTPRadioDlg::OnBnClickedButtonSnnumber2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CopyData(IDC_EDIT_DATA);
}
