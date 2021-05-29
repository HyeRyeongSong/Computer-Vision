
// RGBDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "RGB.h"
#include "RGBDlg.h"
#include "afxdialogex.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FORWARD 1
#define BACKWARD 0

#define MAX_SIZE 100000

void changeColor(Mat img, Mat &copy, int i);

std::vector<int> xTrace_gl;
std::vector<int> yTrace_gl;

// train data�� ������� �� ���л�
std::vector<std::pair<double, double>> circle_pattern;
std::vector<std::pair<double, double>> star_pattern;
std::vector<std::pair<double, double>> triangle_pattern;
std::vector<std::pair<double, double>> square_pattern;


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CRGBDlg ��ȭ ����



CRGBDlg::CRGBDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RGB_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRGBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Img, m_pic);
}

BEGIN_MESSAGE_MAP(CRGBDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Img_Search, &CRGBDlg::OnBnClickedImgSearch)
	ON_BN_CLICKED(IDC_Img_Save, &CRGBDlg::OnBnClickedImgSave)
	ON_BN_CLICKED(IDC_Gray_Btn, &CRGBDlg::OnBnClickedGrayBtn)
	ON_BN_CLICKED(IDC_Otsu_Btn, &CRGBDlg::OnBnClickedOtsuBtn)
	ON_BN_CLICKED(IDC_MorphOpen_Btn, &CRGBDlg::OnBnClickedMorphOpenBtn) // morphology
	ON_BN_CLICKED(IDC_MorphClose_Btn, &CRGBDlg::OnBnClickedMorphCloseBtn) // morphology
	ON_BN_CLICKED(IDC_Label_Btn, &CRGBDlg::OnBnClickedLabelBtn) // labeling
	ON_BN_CLICKED(IDC_Train_Btn, &CRGBDlg::OnBnClickedTrainBtn) // training
	ON_BN_CLICKED(IDC_Match_Btn, &CRGBDlg::OnBnClickedMatchBtn) // matching
END_MESSAGE_MAP()


// CRGBDlg �޽��� ó����

BOOL CRGBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	// �н��� dataset ����
	for (int i = 0; i < 10; ++i) {

		std::string circle_path = "train\\circle\\" + std::to_string(i) + ".png";
		circle[i] = cv::imread(circle_path);

		std::string star_path = "train\\star\\" + std::to_string(i) + ".png";
		star[i] = cv::imread(star_path);

		std::string triangle_path = "train\\triangle\\" + std::to_string(i) + ".png";
		triangle[i] = cv::imread(triangle_path);

		std::string square_path = "train\\square\\" + std::to_string(i) + ".png";
		square[i] = cv::imread(square_path);

	}
	
	//DisplayImage(circle[0], 3);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CRGBDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CRGBDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CRGBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRGBDlg::OnBnClickedImgSearch()
{
	static TCHAR BASED_CODE szFilter[] = _T("�̹��� ����(*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.jpg;*.gif;*.png |�������(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.jpg"), _T("image"), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		pathName = dlg.GetPathName();
		CT2CA pszConvertedAnsiString_up(pathName);
		std::string up_pathName_str(pszConvertedAnsiString_up);
		img = cv::imread(up_pathName_str);
		DisplayImage(img, 3);
	}
}

void CRGBDlg::DisplayImage(Mat targetMat, int channel)
{
	CDC *pDC = nullptr;
	CImage* mfcImg = nullptr;

	pDC = m_pic.GetDC();
	CStatic *staticSize = (CStatic *)GetDlgItem(IDC_Img);
	staticSize->GetClientRect(rect);

	cv::Mat tempImage;
	cv::resize(targetMat, tempImage, Size(rect.Width(), rect.Height()));

	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biBitCount = 24;
	bitmapInfo.bmiHeader.biWidth = tempImage.cols;
	bitmapInfo.bmiHeader.biHeight = tempImage.rows;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biClrImportant = 0;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biSizeImage = 0;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;

	if (targetMat.channels() == 3)
	{
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 24);
	}
	else if (targetMat.channels() == 1)
	{
		//cvtColor(tempImage, tempImage, CV_GRAY2RGB);
		//cvtColor(tempImage, tempImage, COLOR_BGR2GRAY);
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 24);
	}
	else if (targetMat.channels() == 4)
	{
		bitmapInfo.bmiHeader.biBitCount = 32;
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 32);
	}
	cv::flip(tempImage, tempImage, 0);
	::StretchDIBits(mfcImg->GetDC(), 0, 0, tempImage.cols, tempImage.rows,
		0, 0, tempImage.cols, tempImage.rows, tempImage.data, &bitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

	mfcImg->BitBlt(::GetDC(m_pic.m_hWnd), 0, 0);

	if (mfcImg)
	{
		mfcImg->ReleaseDC();
		delete mfcImg;// mfcImg = nullptr;
	}
	tempImage.release();
	ReleaseDC(pDC);
}

/* �Ʒ� 4���� ���꿡 ���Ͽ� ��� 1x3 Structuring Element�� ����*/

Mat dilation(Mat img)
{
	Mat img_copy = img.clone();

	for (int y = 1; y < img_copy.rows - 1; y++)
	{
		// ptr1: Input Image ����Ŵ
		unsigned char* ptr1 = img.data + 3 * (img.cols*y);
		// ptr2: Output Image ����Ŵ
		unsigned char* ptr2 = img_copy.data + 3 * (img_copy.cols*y);

		for (int x = 1; x < img_copy.cols - 1; x++)
		{
			// 1x3 SE ���� �� �ϳ��� 255�̸� SE ������ ���ϴ� �κ��� ���� 255�� �ٲ�
			// Object�� �ش��ϴ� ���� �ø�
			if ((ptr1[3 * (x - 1)] == 255) || (ptr1[3 * x] == 255) || (ptr1[3 * (x + 1)] == 255))
			{
				for (int i = x - 1; i <= x + 1; ++i)
				{
					ptr2[3 * i + 0] = 255;
					ptr2[3 * i + 1] = 255;
					ptr2[3 * i + 2] = 255;
				}
			}
		}
	}

	return img_copy;
}

Mat erosion(Mat img)
{
	Mat img_copy = img.clone();

	for (int y = 1; y < img_copy.rows - 1; y++)
	{
		// ptr1: Input Image ����Ŵ
		unsigned char* ptr1 = img.data + 3 * (img.cols*y);
		// ptr2: Output Image ����Ŵ
		unsigned char* ptr2 = img_copy.data + 3 * (img_copy.cols*y);

		for (int x = 1; x < img_copy.cols - 1; x++)
		{
			// 1x3 SE ���� �� ���� ���� ���� ã�Ƽ� �߽��ȼ��� �� ���� �־���
			// Object�� �ش��ϴ� ���� ����
			int minVal = min({ ptr1[3 * (x - 1)], ptr1[3 * x], ptr1[3 * (x + 1)] });
			ptr2[3 * x + 0] = minVal;
			ptr2[3 * x + 1] = minVal;
			ptr2[3 * x + 2] = minVal;
		}
	}

	return img_copy;
}

Mat opening(Mat img)
{
	Mat img_copy = img.clone();
	// ���� erosion
	img_copy = erosion(img_copy);
	// �� �� dilation
	img_copy = dilation(img_copy);
	return img_copy;
}

Mat closing(Mat img)
{
	Mat img_copy = img.clone();
	// ���� dilation
	img_copy = dilation(img_copy);
	// �� �� erosion
	img_copy = erosion(img_copy);
	return img_copy;
}

/* Contour-tracing Start */

/* �����̴� ���⸶�� �����ڵ带 �ο� */
void read_neighbor(Mat &labImage, int y, int x, int neighbor8[8]) {

	unsigned char* ptr;

	if (y <= 1) {
		neighbor8[5] = 0;
		neighbor8[6] = 0;
		neighbor8[7] = 0;
	}
	else {
		ptr = labImage.data + 3 * (labImage.cols*(y - 1));
		if (x <= 1) {
			neighbor8[5] = 0;
		}
		else {
			neighbor8[5] = ptr[3 * (x - 1)];
		}

		neighbor8[6] = ptr[3 * x];

		if (x >= labImage.cols - 2) {
			neighbor8[7] = 0;
		}
		else {
			neighbor8[7] = ptr[3 * (x + 1)];
		}
	}

	ptr = labImage.data + 3 * (labImage.cols*y);
	if (x >= labImage.cols - 2) {
		neighbor8[0] = 0;
	}
	else {
		neighbor8[0] = ptr[3 * (x + 1)];
	}
	if (x <= 1) {
		neighbor8[4] = 0;
	}
	else {
		neighbor8[4] = ptr[3 * (x - 1)];
	}

	if (y >= labImage.rows-2) {
		neighbor8[1] = 0;
		neighbor8[2] = 0;
		neighbor8[3] = 0;
	}
	else {
		ptr = labImage.data + 3 * (labImage.cols*(y + 1));
		if (x >= labImage.cols - 2) {
			neighbor8[1] = 0;
		}
		else {
			neighbor8[1] = ptr[3 * (x + 1)];
		}

		neighbor8[2] = ptr[3 * x];

		if (x <= 1) {
			neighbor8[3] = 0;
		}
		else {
			neighbor8[3] = ptr[3 * (x - 1)];
		}
	}
} /* end of "read_neighbor" */

/* �����ڵ忡 �°� ��ǥ�� �������� */
void calCoord(int i, int *y, int *x) {
	switch (i) {
	case 0:
		*x = *x + 1;
		break;
	case 1:
		*y = *y + 1;
		*x = *x + 1;
		break;
	case 2:
		*y = *y + 1;
		break;
	case 3:
		*y = *y + 1;
		*x = *x - 1;
		break;
	case 4:
		*x = *x - 1;
		break;
	case 5:
		*y = *y - 1;
		*x = *x - 1;
		break;
	case 6:
		*y = *y - 1;
		break;
	case 7:
		*y = *y - 1;
		*x = *x + 1;
		break;
	} /* end of "switch" */
}

/* ��踦 ������ */
void BTracing(Mat &labImage, long* num_region, int y, int x, int label, bool tag, std::vector<int> &xTrace, std::vector<int> &yTrace) {

	/* y,x�� index �� ���� */
	yTrace.push_back(y);
	xTrace.push_back(x);

	/* �� connectivity�� ���� LookUp Table */
	int LUT_BLabeling8[8][8];

	/* 1���� �迭�� �� ������ ���� */
	int neighbor8[8];

	int cur_orient, pre_orient;
	int pre_y, pre_x, end_y, end_x;

	/* LookUp Table �ʱ�ȭ */

	memset(LUT_BLabeling8, 0, sizeof(LUT_BLabeling8));
	LUT_BLabeling8[4][0] = LUT_BLabeling8[5][0] = LUT_BLabeling8[6][0] = LUT_BLabeling8[7][0] = 1;
	LUT_BLabeling8[5][1] = LUT_BLabeling8[6][1] = LUT_BLabeling8[7][1] = 1;
	LUT_BLabeling8[6][2] = LUT_BLabeling8[7][2] = 1;
	LUT_BLabeling8[7][3] = 1;
	LUT_BLabeling8[1][5] = LUT_BLabeling8[2][5] = LUT_BLabeling8[3][5] = LUT_BLabeling8[4][5] = LUT_BLabeling8[5][5] = LUT_BLabeling8[6][5] = LUT_BLabeling8[7][5] = 1;
	LUT_BLabeling8[2][6] = LUT_BLabeling8[3][6] = LUT_BLabeling8[4][6] = LUT_BLabeling8[5][6] = LUT_BLabeling8[6][6] = LUT_BLabeling8[7][6] = 1;
	LUT_BLabeling8[3][7] = LUT_BLabeling8[4][7] = LUT_BLabeling8[5][7] = LUT_BLabeling8[6][7] = LUT_BLabeling8[7][7] = 1;

	/* �ٱ��� ����� ��쿡�� ���������� ���� */
	if (tag == FORWARD) {
		cur_orient = pre_orient = 0;
	}
	else { /* ���� ����� ��쿡�� ���������� ������ */
		cur_orient = pre_orient = 6;
	}

	end_y = pre_y = y;
	end_x = pre_x = x;

	do {
		/* 1���� �迭�� �� ��ġ(����)�� ���� �о�� */
		read_neighbor(labImage, y, x, neighbor8);
		/* ���� ���⿡�� -2�� ��ġ���� ���� -> 6������ ���� */
		int start_o = (8 + cur_orient - 2) % 8;
		int add_o, i;
		for (i = 0; i < 8; i++) {
			add_o = (start_o + i) % 8;
			/* �̿� �� 1(object)�� �߰��ϸ� ���� */
			if (neighbor8[add_o] != 0)
				break;
		}
		/* 1�� �̿��� ��ġ�� ������ */
		if (i < 8) {
			calCoord(add_o, &y, &x);
			cur_orient = add_o;
		}
		else {
			break;
		}
		/* LookUp Table�� ���� �ö󰡴� ��ġ���� �Ǻ� */
		if (LUT_BLabeling8[pre_orient][cur_orient]) {
			/* �ö󰡴� ��ġ�� �ű⿡ label�� �Ҵ����� */
			num_region[label]++;
			unsigned char* ptr = labImage.data + 3 * (labImage.cols*pre_y);
			ptr[3 * pre_x + 0] = ptr[3 * pre_x + 1] = ptr[3 * pre_x + 2] = label;

			/* y,x�� index �� ���� */
			yTrace.push_back(pre_y);
			xTrace.push_back(pre_x);
		}

		pre_y = y;
		pre_x = x;
		pre_orient = cur_orient;

	} while ((y != end_y) || (x != end_x)); // x, y�� ó�� ������ ��ġ�� ������ ����
}


Mat contourTracing(Mat img, std::vector<int> &xTrace, std::vector<int> &yTrace)
{
	Mat labImage = img.clone(); // �Է� ������ binary image, 0: background, 255: object

	long num_region[MAX_SIZE]; // �� ������ ȭ�Ҽ��� ������

	/* �ʱ�ȭ */
	for (int i = 0; i < MAX_SIZE; i++)
		num_region[i] = 0;

	unsigned int labelnumber = 1; // label number�� 2���� ������ų ����

	/* labImage labeling ���� 255�� 1�� �ʱ�ȭ */
	for (int y = 0; y < labImage.rows; y++)
	{
		unsigned char* ptr = labImage.data + 3 * (labImage.cols * y);
		for (int x = 0; x < labImage.cols; x++)
		{
			if (ptr[3 * x] == 255) {
				ptr[3 * x + 0] = 1;
				ptr[3 * x + 1] = 1;
				ptr[3 * x + 2] = 1;
			}
		}
	}

	for (int y = 1; y < labImage.rows - 1; y++)
	{
		unsigned char* ptr1 = labImage.data + 3 * (labImage.cols*y); // ��� ��
		unsigned char* ptr2 = labImage.data + 3 * (labImage.cols*(y - 1)); // ���� ��


		for (int x = 1; x < labImage.cols - 1; x++)
		{
			unsigned char cur_p = ptr1[3 * x];

			if (cur_p == 1) { // object
				int ref_p1 = ptr1[3 * (x - 1)]; // (���� ��)�� ��
				int ref_p2 = ptr2[3 * (x - 1)]; // (���� ��)�� ��
				int ref_p3 = ptr2[3 * (x)]; // (�� ��)�� ��
				if (ref_p1 > 1) { // propagation

					/* ���������̹Ƿ� ���޹��� label�� ȭ�Ҽ��� �ϳ� ���� ��
					���޹��� label ��ȣ�� �Ҵ� */
					num_region[ref_p1]++;
					//ptr1[3 * x + 0] = ptr1[3 * x + 1] = ptr1[3 * x + 2] = ref_p1;
				}
				else if ((ref_p1 == 0) && (ref_p2 >= 2)) { // hole

					/* Ȧ�����̹Ƿ� ���޹��� label�� ȭ�Ҽ��� �ϳ� ���� ��
					���޹��� label ��ȣ�� �Ҵ� ��
					���������� bound tracing�� */
					num_region[ref_p2]++;
					ptr1[3 * x + 0] = ptr1[3 * x + 1] = ptr1[3 * x + 2] = ref_p2;
					BTracing(labImage, num_region, y, x, ref_p2, BACKWARD, xTrace, yTrace);
				}
				else if ((ref_p1 == 0) && (ref_p2 == 0)) { // region start

					/* ���������̹Ƿ� label ��ȣ�� �ϳ� ���� ��
					�ش� label�� ȭ�Ҽ��� �ϳ� ������Ű��
					���� ������ label ��ȣ�� �Ҵ� ��
					���������� bound tracing�� */
					labelnumber++;
					num_region[labelnumber]++;
					ptr1[3 * x + 0] = ptr1[3 * x + 1] = ptr1[3 * x + 2] = labelnumber;
					BTracing(labImage, num_region, y, x, labelnumber, FORWARD, xTrace, yTrace);
				}
			}
			else { // background
				ptr1[3 * x + 0] = 0;
				ptr1[3 * x + 1] = 0;
				ptr1[3 * x + 2] = 0;
			}
		}
	}

	/* label�� ��谪�� ���� */
	for (int y = 1; y < labImage.rows-1; y++)
	{
		unsigned char* b_ptr = img.data + 3 * (img.cols*y);
		unsigned char* ptr = labImage.data + 3 * (labImage.cols*y);
		for (int x = 1; x < labImage.cols-1; x++)
		{
			if (ptr[3 * x] > 1) {
				ptr[3 * x + 0] = 255;
				ptr[3 * x + 1] = 255;
				ptr[3 * x + 2] = 255;
			}
			else {
				ptr[3 * x + 0] = 0;
				ptr[3 * x + 1] = 0;
				ptr[3 * x + 2] = 0;
			}
		}
	}

	return labImage;

}

/* Contour-tracing End */

/* LCS */

void getLCS(std::vector<int> &xTrace, std::vector<int> &yTrace, std::vector<double> &h) 
{
	int w = 50; // window size
	int N = xTrace.size();
	h.resize(N);

	for (int i = 0; i < N; ++i) {
		int left = i - (w - 1) / 2;
		int right = i + (w - 1) / 2;

		left = (left + N) % N;
		right = (right + N) % N;

		double u = (double)((xTrace[i] * (yTrace[left] - yTrace[right] ))
			+ (yTrace[i] * (xTrace[right]  - xTrace[left] ))
			+ ((yTrace[right] )*(xTrace[left] ))
			- ((yTrace[left])*(xTrace[right] )));
		double v = (double)(pow((yTrace[left] ) - yTrace[right], 2)
			+ pow((xTrace[left] - xTrace[right] ), 2));
		v = sqrt(v);
		h[i] = abs(u / v);
	}
}

/* Ư¡���� �� train */

void DTW(std::vector<double> &A, std::vector<double> &B, std::vector<double> &sumMatch, std::vector<double> &squaredSum, std::vector<int> &cntMatch)
{
	int N = A.size();
	int M = B.size();

	std::vector<std::vector<double>> D;
	std::vector<std::vector<int>> G;
	std::vector<int> p;

	D.resize(N+1);
	G.resize(N+1);

	for (int i = 0; i < N; ++i) {
		D[i].resize(M+1);
		G[i].resize(M+1);
	}

	// Initialization
	D[0][0] = abs(A[0] - B[0]); // �� ���� ���� ����
	G[0][0] = 0; // �ʿ� ���� ���̹Ƿ� 0���� �ʱ�ȭ

	for (int j = 1; j < M; j++) {
		D[0][j] = D[0][j - 1] + abs(A[0] - B[j]);
		G[0][j] = 2;
	}
	for (int i = 1; i < N; i++)
		D[i][0] = DBL_MAX;

	printf("done");

	//  Forward
	for (int i = 1; i < N; i++) {
		for (int j = 1; j < M; j++) {
			double minVal = min({ D[i - 1][j], D[i][j - 1], D[i - 1][j - 1] });
			D[i][j] = abs(A[i] - B[j]) + minVal;
			printf("%d %d\n", i, j);
			if (minVal == D[i - 1][j - 1])
				G[i][j] = 3;
			else if (minVal == D[i][j - 1])
				G[i][j] = 2;
			else
				G[i][j] = 1;
		}
	}

	// Backtracking
	int i = N-1;
	int j = M-1;
	int k = 1; // ��δ� �ڿ������� �Ųٷ� ����� -> ��½� ����� �� ��

	while ((i != 0) && (j != 0)) { // i�� j�� ��� 0�� �ƴ� ������ �ݺ�
		p.push_back(G[i][j]);
		sumMatch[i] += B[j];
		squaredSum[i] += pow(B[j], 2);
		cntMatch[i]++;
		switch (G[i][j]) {
		case 1:
			i--;
			k++;
			break;
		case 2:
			j--;
			k++;
			break;
		case 3:
			i--;
			j--;
			k++;
			break;
		}

	} // Termination

	p.push_back(G[0][0]);
	sumMatch[0] += B[0];
	squaredSum[0] += pow(B[0], 2);
	cntMatch[0]++;

	double S = D[N-1][M-1] / k; // ��Ī�� ���̸� k�� ������ (dissimilarity)

}

// 1���� class�� ���� SDTW
std::vector<std::pair<double, double>> SDTW(Mat (&trainData)[10])
{
	std::vector<std::vector<int>> xTrace;
	std::vector<std::vector<int>> yTrace;
	std::vector<std::vector<double>> h;

	xTrace.resize(10);
	yTrace.resize(10);
	h.resize(10);

	for (int i = 0; i < 10; ++i) {
		changeColor(trainData[i], trainData[i], 4); // to GrayScale image
		changeColor(trainData[i], trainData[i], 5); // to Otsu image
		trainData[i] = opening(trainData[i]); // MorphologicalImage Processing
		trainData[i] = contourTracing(trainData[i], xTrace[i], yTrace[i]); // Labeling
		getLCS(xTrace[i], yTrace[i], h[i]); // Calculate LCS
	}

	bool isFirst = true; // ó������ ���������� ����������� ����
	std::vector<double> currMean = h[0]; // ���� ��� ����
	std::vector<double> nextMean; // ��ġ�� ��հ� (���� ��� ����)
	std::vector<double> cov; // ��ġ�� ���л�

	std::vector<double> sumMatch; // �� ��ġ���� ��Ī�Ǵ� ��� �� �� ���س���
	std::vector<double> squaredSum; // �� ��ġ���� ��Ī�Ǵ� ��� ���� ���� �� ���س���
	std::vector<int> cntMatch; // �� ��ġ���� ��Ī�Ǵ� ���� ���� ����

	/* ��������� ���ϴ� ���� while�� */

	while (true) {

		/* Initialize */
		nextMean.resize(currMean.size());
		fill(nextMean.begin(), nextMean.end(), 0);

		sumMatch = currMean; 

		squaredSum.resize(currMean.size());
		for (int i = 0; i < currMean.size(); ++i) {
			squaredSum[i] = pow(currMean[i], 2);
		}

		cntMatch.resize(currMean.size());
		fill(cntMatch.begin(), cntMatch.end(), 1);

		cov.resize(currMean.size());

		/* 1. DTW�� ������ϰ� �׿� ���� ������ ��Ī ��ġ */

		for (int i = 0; i < 10; ++i) {
			if (isFirst && i == 0) {
				isFirst = false;
				continue;
			}
			DTW(currMean, h[i], sumMatch, squaredSum, cntMatch);
		}

		/* 2. ��Ī��ġ�� ����� ���ο� ������� ���� */

		for (int i = 0; i < nextMean.size(); ++i) {
			nextMean[i] = sumMatch[i] / (double)cntMatch[i];
		}

		/* 3. ��������� ������ ���ϴ��� üũ */
		bool isSame = true;
		for (int i = 0; i < nextMean.size(); ++i) {
			if (abs(currMean[i] - nextMean[i]) >= 1) {
				isSame = false;
				break;
			}
		}

		/* ��������� ������ ��ġ�� ���л��� �����ְ� while�� ���� */
		if (isSame) {

			// ��ġ�� ���л��� ����
			for (int i = 0; i < squaredSum.size(); ++i) {
				cov[i] = (squaredSum[i] / (double)cntMatch[i]) - pow(nextMean[i], 2);
			}

			break;
		}
		else { /* ��������� �ٸ��� ���ο� ����������� �ٽ� while�� �� */
			currMean = nextMean;
		}
	}

	std::vector<std::pair<double, double>> ans;

	for (int i = 0; i < nextMean.size(); ++i) {
		ans.push_back(std::make_pair(nextMean[i], cov[i]));
	}

	return ans;

}

/* ������ϰ� ���л꿡 ����� ����� �Ÿ��� ������� DTW�� �����Ͽ� score ��� */
double DTW_Match(std::vector<std::pair<double, double>> final_pattern, std::vector<double> &B)
{
	/* final_pattern: �� ��ġ�� (�������, ���л�), B: test data */

	int N = final_pattern.size();
	int M = B.size();

	std::vector<std::vector<double>> D;
	std::vector<std::vector<int>> G;
	std::vector<int> p;

	D.resize(N + 1);
	G.resize(N + 1);

	for (int i = 0; i < N; ++i) {
		D[i].resize(M + 1);
		G[i].resize(M + 1);
	}

	// Initialization
	D[0][0] = -0.5 * pow((final_pattern[0].first - B[0]), 2) / final_pattern[0].second; // �� ���� ����� �Ÿ�
	G[0][0] = 0; // �ʿ� ���� ���̹Ƿ� 0���� �ʱ�ȭ

	for (int j = 1; j < M; j++) {
		D[0][j] = D[0][j - 1] + -0.5 * pow((final_pattern[0].first - B[j]), 2) / final_pattern[0].second;  // �� ���� ����� �Ÿ�
		G[0][j] = 2;
	}
	for (int i = 1; i < N; i++)
		D[i][0] = DBL_MAX;

	printf("done");

	//  Forward
	for (int i = 1; i < N; i++) {
		for (int j = 1; j < M; j++) {
			double minVal = min({ D[i - 1][j], D[i][j - 1], D[i - 1][j - 1] });
			D[i][j] = (-0.5 * pow((final_pattern[i].first - B[j]), 2) / final_pattern[i].second) + minVal; // �� ���� ����� �Ÿ� + minVal;
			printf("%d %d\n", i, j);
			if (minVal == D[i - 1][j - 1])
				G[i][j] = 3;
			else if (minVal == D[i][j - 1])
				G[i][j] = 2;
			else
				G[i][j] = 1;
		}
	}

	// Backtracking
	int i = N - 1;
	int j = M - 1;
	int k = 1; // ��δ� �ڿ������� �Ųٷ� ����� -> ��½� ����� �� ��

	while ((i != 0) && (j != 0)) { // i�� j�� ��� 0�� �ƴ� ������ �ݺ�
		p.push_back(G[i][j]);

		switch (G[i][j]) {
		case 1:
			i--;
			k++;
			break;
		case 2:
			j--;
			k++;
			break;
		case 3:
			i--;
			j--;
			k++;
			break;
		}

	} // Termination

	p.push_back(G[0][0]);

	double S = D[N - 1][M - 1] / k; // ��Ī�� ���̸� k�� ������ (dissimilarity)

	return S;

}


void CRGBDlg::OnBnClickedImgSave()
{
	imwrite("copy.jpg", img);

	MessageBox(_T("�̹��� ���� �Ϸ�!"), _T(""));
}


void CRGBDlg::OnBnClickedGrayBtn()
{
	Mat img_copy = img.clone();
	changeColor(img, img_copy, 4);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}


void CRGBDlg::OnBnClickedOtsuBtn()
{
	//OnBnClickedRedBtn �����Ͽ� �ۼ��Ͻÿ�.
	Mat img_copy = img.clone();
	changeColor(img, img_copy, 5);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}


void CRGBDlg::OnBnClickedMorphOpenBtn()
{
	// ���� ���� ����
	Mat img_copy = img.clone();
	img_copy = opening(img_copy);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}

void CRGBDlg::OnBnClickedMorphCloseBtn()
{
	// ���� ���� ����
	Mat img_copy = img.clone();
	img_copy = closing(img_copy);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}

void CRGBDlg::OnBnClickedLabelBtn()
{
	// Label ���� ����
	Mat img_copy = img.clone();

	xTrace_gl.resize(0);
	yTrace_gl.resize(0);
	img_copy = contourTracing(img_copy, xTrace_gl, yTrace_gl);

	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}

void CRGBDlg::OnBnClickedTrainBtn()
{
	MessageBox(_T("train start! (�ð��� ���� �ҿ�Ǵ� �����ϼ���)"), _T(""));

	circle_pattern = SDTW(circle);

	MessageBox(_T("circle �н��Ϸ�"), _T(""));

	star_pattern = SDTW(star);

	MessageBox(_T("star �н��Ϸ�"), _T(""));

	triangle_pattern = SDTW(triangle);

	MessageBox(_T("triangle �н��Ϸ�"), _T(""));

	square_pattern = SDTW(square);

	MessageBox(_T("square �н��Ϸ�"), _T(""));

	MessageBox(_T("train finish!"), _T(""));

}


void CRGBDlg::OnBnClickedMatchBtn()
{	
	static TCHAR BASED_CODE szFilter[] = _T("�̹��� ����(*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.jpg;*.gif;*.png |�������(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.jpg"), _T("image"), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		pathName = dlg.GetPathName();
		CT2CA pszConvertedAnsiString_up(pathName);
		std::string up_pathName_str(pszConvertedAnsiString_up);
		img = cv::imread(up_pathName_str);

		DisplayImage(img, 3);

		std::vector<double> hTest;

		changeColor(img, img, 4); // to GrayScale image
		changeColor(img, img, 5); // to Otsu image
		img = opening(img); // MorphologicalImage Processing
		img = contourTracing(img, xTrace_gl, yTrace_gl); // Labeling
		getLCS(xTrace_gl, yTrace_gl, hTest); // Calculate LCS

		// ����� �Ÿ��� ����Ͽ� �� class�� score ����
		double circleScore = DTW_Match(circle_pattern, hTest);
		double starScore = DTW_Match(star_pattern, hTest);
		double triangleScore = DTW_Match(triangle_pattern, hTest);
		double squareScore = DTW_Match(square_pattern, hTest);

		// test image�� ���� ���� score�� ���� class�� ���ϹǷ� ���� class�� ���

		double minScore = min({ circleScore, starScore, triangleScore, squareScore });

		if (minScore == circleScore) {
			MessageBox(_T("class 0: circle"), _T(""));
		}
		else if (minScore == starScore) {
			MessageBox(_T("class 1: star"), _T(""));
		}
		else if (minScore == triangleScore) {
			MessageBox(_T("class 2: triangle"), _T(""));
		}
		else if (minScore == squareScore) {
			MessageBox(_T("class 1: square"), _T(""));
		}

	}

	DisplayImage(circle[4], 3);

}


void changeColor(Mat img, Mat &copy, int i)
{
	if (i == 1)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x<copy.cols; x++)
			{
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����
				ptr1[3 * x + 0] = 0;
				ptr1[3 * x + 1] = 0;
				ptr1[3 * x + 2] = ptr1[3 * x + 2];
			}
		}
	}
	else if (i == 2)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x<copy.cols; x++)
			{
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����
				ptr1[3 * x + 0] = 0;
				ptr1[3 * x + 1] = ptr1[3 * x + 1];
				ptr1[3 * x + 2] = 0;
			}
		}
	}
	else if(i == 3)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x<copy.cols; x++)
			{
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����
				ptr1[3 * x + 0] = ptr1[3 * x + 0];
				ptr1[3 * x + 1] = 0;
				ptr1[3 * x + 2] = 0;
			}
		}
	}
	else if (i == 4)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x < copy.cols; x++)
			{
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����

				//RGB ���� ���ؼ� 3���� ���� ���� ������ ���ҿ� �־��ش�
				ptr1[3 * x + 0] = (ptr1[3 * x + 0] + ptr1[3 * x + 1] + ptr1[3 * x + 2]) / 3;
				ptr1[3 * x + 1] = (ptr1[3 * x + 0] + ptr1[3 * x + 1] + ptr1[3 * x + 2]) / 3;
				ptr1[3 * x + 2] = (ptr1[3 * x + 0] + ptr1[3 * x + 1] + ptr1[3 * x + 2]) / 3;
				/*
				ptr1[3 * x + 0] = (0.0722f*ptr1[3 * x + 2]) + (0.7152f*ptr1[3 * x + 1]) + (0.2126f*ptr1[3 * x + 0]);
				ptr1[3 * x + 1] = (0.0722f*ptr1[3 * x + 2]) + (0.7152f*ptr1[3 * x + 1]) + (0.2126f*ptr1[3 * x + 0]);
				ptr1[3 * x + 2] = (0.0722f*ptr1[3 * x + 2]) + (0.7152f*ptr1[3 * x + 1]) + (0.2126f*ptr1[3 * x + 0]);
				*/
			}
		}
	}
	else if (i == 5)
	{
		long int H[256]; // Histogram�� ����
		memset(H, 0, sizeof(H)); // Histogram�� 0���� �ʱ�ȭ

		// ����ȭ�� Historgram
		double NH[256];
		memset(NH, 0, sizeof(NH)); // Normalized Histogram�� 0���� �ʱ�ȭ

		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x < copy.cols; x++)
			{
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����
				H[ptr1[3 * x + 0]]++;
			}
		}

		// ȭ�� ���� ��� ���� �׻� �Ȱ��� ������׷��� ����ϱ� ���� Histogram�� ���� ��ü ȭ�� ���� ����
		for (int i = 0; i < 256; ++i)
		{
			NH[i] = (double)H[i] / (copy.rows * copy.cols);
		}

		// ��ü ��� ���
		double M_t = 0;
		for (int i = 0; i < 256; ++i)
			M_t = i * NH[i];

		// ������ ����ġ �� ���
		double w0[256];
		double w1[256];

		for (int i = 0; i < 256; ++i)
		{
			w0[i] = 0;
			w1[i] = 0;
			for (int j = 0; j <= i; ++j)
			{
				w0[i] += NH[j];
			}
			for (int j = i + 1; j < 256; ++j)
			{
				w1[i] += NH[j];
			}
		}

		// ������ ��հ� ���
		double m0[256];
		double m1[256];

		for (int i = 0; i < 256; ++i)
		{
			m0[i] = 0;
			m1[i] = 0;
			for (int j = 0; j <= i; ++j)
			{
				m0[i] += (double)(j * NH[j]) / w0[i];
			}
			for (int j = i + 1; j < 256; ++j)
			{
				m1[i] += (double)(j * NH[j]) / w1[i];
			}
		}

		// ��� ������ �Ÿ��� ������� ���
		double var[256] = { 0 };

		for (int i = 1; i < 255; ++i)
		{
			var[i] = (w0[i] * (m0[i] - M_t) * (m0[i] - M_t)) + (w1[i] * (m1[i] - M_t) * (m1[i] - M_t));
		}

		// T ���� ��� -> argmax of var
		int T = 0;
		double maxVar = 0;
		for (int i = 1; i < 255; ++i)
		{
			if (var[i] > maxVar)
			{
				maxVar = var[i];
				T = i;
			}
		}

		// Otsu's segmentation (����ȭ) ����
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x < copy.cols; x++)
			{
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����
				if (ptr1[3 * x + 0] <= T)
				{
					ptr1[3 * x + 0] = 0;
					ptr1[3 * x + 1] = 0;
					ptr1[3 * x + 2] = 0;
				}
				else {
					ptr1[3 * x + 0] = 255;
					ptr1[3 * x + 1] = 255;
					ptr1[3 * x + 2] = 255;
				}
				
			}
		}
	}
}






