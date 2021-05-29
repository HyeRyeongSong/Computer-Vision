#include <opencv2/opencv.hpp>
#include "afxwin.h"
using namespace cv;
// RGBDlg.h : ��� ����
//

#pragma once


// CRGBDlg ��ȭ ����
class CRGBDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CRGBDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	Mat img;
	CString pathName;
	CRect rect;
	Mat circle[10]; // class 1
	Mat star[10]; // class 2
	Mat triangle[10]; // class 3
	Mat square[10]; // class 4

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RGB_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedImgSearch();
	CStatic m_pic;
	//�̹��� load��
	void CRGBDlg::DisplayImage(Mat targetMat, int channel);
	afx_msg void OnBnClickedImgSave();
	afx_msg void OnBnClickedGrayBtn();
	afx_msg void OnBnClickedOtsuBtn();
	afx_msg void OnBnClickedMorphOpenBtn();
	afx_msg void OnBnClickedMorphCloseBtn();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedLabelBtn();
	afx_msg void OnBnClickedTrainBtn();
	afx_msg void OnBnClickedMatchBtn();
};
