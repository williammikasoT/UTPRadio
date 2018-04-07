
// MFCUTPRadioDlg.h : ͷ�ļ�
//

#include<stdlib.h>
#include<iostream>
#include<stdio.h>
#include<conio.h>
#include<winsock2.h>
#include<windows.h>

#include "sockLx.h"

#pragma once


// CMFCUTPRadioDlg �Ի���
class CMFCUTPRadioDlg : public CDialogEx
{
// ����
public:
	CMFCUTPRadioDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCUTPRADIO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	static  DWORD  WINAPI MyThreadUDP(LPVOID pParam);	// UDP�㲥�����������
	static  DWORD  WINAPI MyThreadRecv(LPVOID pParam);  // TCP��ȡ�����������������ݷ�ֹ����
	//static  DWORD WINAPI WINAPIThreadConnectDevice(LPVOID wParam);
	void UDPStart();		// UDP �㲥��ȡip
	void OnClentInit();		// UDP ��ʼ��
	afx_msg void OnTimer(UINT_PTR nIDEvent);  //TCP��������
	void ValidateRece();
	

	CSockLx m_sock;        // tcp
	SOCKET connect_socket; // udp
	SOCKADDR_IN sin_from;  // udp
	CListCtrl m_list1;
	CString m_TcpIP;       // ��ǰ�б�ѡ��TCP��IP
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
