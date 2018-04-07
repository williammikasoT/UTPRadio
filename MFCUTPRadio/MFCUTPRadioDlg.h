
// MFCUTPRadioDlg.h : 头文件
//

#include<stdlib.h>
#include<iostream>
#include<stdio.h>
#include<conio.h>
#include<winsock2.h>
#include<windows.h>

#include "sockLx.h"

#pragma once


// CMFCUTPRadioDlg 对话框
class CMFCUTPRadioDlg : public CDialogEx
{
// 构造
public:
	CMFCUTPRadioDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCUTPRADIO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	static  DWORD  WINAPI MyThreadUDP(LPVOID pParam);	// UDP广播获得在线仪器
	static  DWORD  WINAPI MyThreadRecv(LPVOID pParam);  // TCP获取服务器返回来的数据防止奔溃
	//static  DWORD WINAPI WINAPIThreadConnectDevice(LPVOID wParam);
	void UDPStart();		// UDP 广播获取ip
	void OnClentInit();		// UDP 初始化
	afx_msg void OnTimer(UINT_PTR nIDEvent);  //TCP发送心跳
	void ValidateRece();
	

	CSockLx m_sock;        // tcp
	SOCKET connect_socket; // udp
	SOCKADDR_IN sin_from;  // udp
	CListCtrl m_list1;
	CString m_TcpIP;       // 当前列表选择TCP的IP
	//HANDLE m_hThread;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonUdp();
	afx_msg void OnBnClickedButtonTcp();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonOutdata();
	afx_msg void OnBnClickedButtonSnnumber();
	CString GetTime();
	bool SaveExcel();
	afx_msg void OnBnClickedButtonGetdata();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	void CopyData(int number);
	afx_msg void OnBnClickedButtonSnnumber2();
};
