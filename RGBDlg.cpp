
// RGBDlg.cpp : 구현 파일
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

// train data의 평균패턴 및 공분산
std::vector<std::pair<double, double>> circle_pattern;
std::vector<std::pair<double, double>> star_pattern;
std::vector<std::pair<double, double>> triangle_pattern;
std::vector<std::pair<double, double>> square_pattern;


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CRGBDlg 대화 상자



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


// CRGBDlg 메시지 처리기

BOOL CRGBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// 학습용 dataset 저장
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

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRGBDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRGBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRGBDlg::OnBnClickedImgSearch()
{
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.jpg;*.gif;*.png |모든파일(*.*)|*.*||");
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

/* 아래 4가지 연산에 대하여 모두 1x3 Structuring Element를 적용*/

Mat dilation(Mat img)
{
	Mat img_copy = img.clone();

	for (int y = 1; y < img_copy.rows - 1; y++)
	{
		// ptr1: Input Image 가리킴
		unsigned char* ptr1 = img.data + 3 * (img.cols*y);
		// ptr2: Output Image 가리킴
		unsigned char* ptr2 = img_copy.data + 3 * (img_copy.cols*y);

		for (int x = 1; x < img_copy.cols - 1; x++)
		{
			// 1x3 SE 영역 중 하나라도 255이면 SE 영역에 속하는 부분을 전부 255로 바꿈
			// Object에 해당하는 영역 늘림
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
		// ptr1: Input Image 가리킴
		unsigned char* ptr1 = img.data + 3 * (img.cols*y);
		// ptr2: Output Image 가리킴
		unsigned char* ptr2 = img_copy.data + 3 * (img_copy.cols*y);

		for (int x = 1; x < img_copy.cols - 1; x++)
		{
			// 1x3 SE 영역 중 가장 작은 값을 찾아서 중심픽셀에 이 값을 넣어줌
			// Object에 해당하는 영역 줄임
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
	// 먼저 erosion
	img_copy = erosion(img_copy);
	// 그 후 dilation
	img_copy = dilation(img_copy);
	return img_copy;
}

Mat closing(Mat img)
{
	Mat img_copy = img.clone();
	// 먼저 dilation
	img_copy = dilation(img_copy);
	// 그 후 erosion
	img_copy = erosion(img_copy);
	return img_copy;
}

/* Contour-tracing Start */

/* 움직이는 방향마다 방향코드를 부여 */
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

/* 방향코드에 맞게 좌표를 움직여줌 */
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

/* 경계를 추적함 */
void BTracing(Mat &labImage, long* num_region, int y, int x, int label, bool tag, std::vector<int> &xTrace, std::vector<int> &yTrace) {

	/* y,x의 index 값 저장 */
	yTrace.push_back(y);
	xTrace.push_back(x);

	/* 각 connectivity에 따른 LookUp Table */
	int LUT_BLabeling8[8][8];

	/* 1차원 배열에 각 방향을 저장 */
	int neighbor8[8];

	int cur_orient, pre_orient;
	int pre_y, pre_x, end_y, end_x;

	/* LookUp Table 초기화 */

	memset(LUT_BLabeling8, 0, sizeof(LUT_BLabeling8));
	LUT_BLabeling8[4][0] = LUT_BLabeling8[5][0] = LUT_BLabeling8[6][0] = LUT_BLabeling8[7][0] = 1;
	LUT_BLabeling8[5][1] = LUT_BLabeling8[6][1] = LUT_BLabeling8[7][1] = 1;
	LUT_BLabeling8[6][2] = LUT_BLabeling8[7][2] = 1;
	LUT_BLabeling8[7][3] = 1;
	LUT_BLabeling8[1][5] = LUT_BLabeling8[2][5] = LUT_BLabeling8[3][5] = LUT_BLabeling8[4][5] = LUT_BLabeling8[5][5] = LUT_BLabeling8[6][5] = LUT_BLabeling8[7][5] = 1;
	LUT_BLabeling8[2][6] = LUT_BLabeling8[3][6] = LUT_BLabeling8[4][6] = LUT_BLabeling8[5][6] = LUT_BLabeling8[6][6] = LUT_BLabeling8[7][6] = 1;
	LUT_BLabeling8[3][7] = LUT_BLabeling8[4][7] = LUT_BLabeling8[5][7] = LUT_BLabeling8[6][7] = LUT_BLabeling8[7][7] = 1;

	/* 바깥쪽 경계의 경우에는 정방향으로 돌고 */
	if (tag == FORWARD) {
		cur_orient = pre_orient = 0;
	}
	else { /* 안쪽 경계의 경우에는 역방향으로 돌게함 */
		cur_orient = pre_orient = 6;
	}

	end_y = pre_y = y;
	end_x = pre_x = x;

	do {
		/* 1차원 배열에 각 위치(방향)을 먼저 읽어옴 */
		read_neighbor(labImage, y, x, neighbor8);
		/* 현재 방향에서 -2한 위치부터 시작 -> 6번부터 시작 */
		int start_o = (8 + cur_orient - 2) % 8;
		int add_o, i;
		for (i = 0; i < 8; i++) {
			add_o = (start_o + i) % 8;
			/* 이웃 중 1(object)을 발견하면 종료 */
			if (neighbor8[add_o] != 0)
				break;
		}
		/* 1인 이웃의 위치를 가져옴 */
		if (i < 8) {
			calCoord(add_o, &y, &x);
			cur_orient = add_o;
		}
		else {
			break;
		}
		/* LookUp Table을 통해 올라가는 위치인지 판별 */
		if (LUT_BLabeling8[pre_orient][cur_orient]) {
			/* 올라가는 위치면 거기에 label을 할당해줌 */
			num_region[label]++;
			unsigned char* ptr = labImage.data + 3 * (labImage.cols*pre_y);
			ptr[3 * pre_x + 0] = ptr[3 * pre_x + 1] = ptr[3 * pre_x + 2] = label;

			/* y,x의 index 값 저장 */
			yTrace.push_back(pre_y);
			xTrace.push_back(pre_x);
		}

		pre_y = y;
		pre_x = x;
		pre_orient = cur_orient;

	} while ((y != end_y) || (x != end_x)); // x, y가 처음 시작한 위치를 만나면 종료
}


Mat contourTracing(Mat img, std::vector<int> &xTrace, std::vector<int> &yTrace)
{
	Mat labImage = img.clone(); // 입력 영상은 binary image, 0: background, 255: object

	long num_region[MAX_SIZE]; // 각 영역의 화소수를 세어줌

	/* 초기화 */
	for (int i = 0; i < MAX_SIZE; i++)
		num_region[i] = 0;

	unsigned int labelnumber = 1; // label number는 2부터 증가시킬 것임

	/* labImage labeling 위해 255를 1로 초기화 */
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
		unsigned char* ptr1 = labImage.data + 3 * (labImage.cols*y); // 가운데 값
		unsigned char* ptr2 = labImage.data + 3 * (labImage.cols*(y - 1)); // 위의 값


		for (int x = 1; x < labImage.cols - 1; x++)
		{
			unsigned char cur_p = ptr1[3 * x];

			if (cur_p == 1) { // object
				int ref_p1 = ptr1[3 * (x - 1)]; // (왼쪽 값)을 봄
				int ref_p2 = ptr2[3 * (x - 1)]; // (왼위 값)을 봄
				int ref_p3 = ptr2[3 * (x)]; // (위 값)을 봄
				if (ref_p1 > 1) { // propagation

					/* 전파조건이므로 전달받은 label의 화소수를 하나 증가 후
					전달받은 label 번호를 할당 */
					num_region[ref_p1]++;
					//ptr1[3 * x + 0] = ptr1[3 * x + 1] = ptr1[3 * x + 2] = ref_p1;
				}
				else if ((ref_p1 == 0) && (ref_p2 >= 2)) { // hole

					/* 홀조건이므로 전달받은 label의 화소수를 하나 증가 후
					전달받은 label 번호를 할당 후
					역방향으로 bound tracing함 */
					num_region[ref_p2]++;
					ptr1[3 * x + 0] = ptr1[3 * x + 1] = ptr1[3 * x + 2] = ref_p2;
					BTracing(labImage, num_region, y, x, ref_p2, BACKWARD, xTrace, yTrace);
				}
				else if ((ref_p1 == 0) && (ref_p2 == 0)) { // region start

					/* 시작조건이므로 label 번호를 하나 증가 후
					해당 label의 화소수를 하나 증가시키고
					새로 지정한 label 번호를 할당 후
					정방향으로 bound tracing함 */
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

	/* label된 경계값만 추출 */
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

/* 특징추출 → train */

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
	D[0][0] = abs(A[0] - B[0]); // 두 값의 차의 절댓값
	G[0][0] = 0; // 필요 없는 값이므로 0으로 초기화

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
	int k = 1; // 경로는 뒤에서부터 거꾸로 저장됨 -> 출력시 뒤집어서 할 것

	while ((i != 0) && (j != 0)) { // i와 j가 모두 0이 아닐 때까지 반복
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

	double S = D[N-1][M-1] / k; // 매칭된 길이를 k로 나눠줌 (dissimilarity)

}

// 1개의 class에 대한 SDTW
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

	bool isFirst = true; // 처음에는 임의패턴을 평균패턴으로 선택
	std::vector<double> currMean = h[0]; // 현재 평균 패턴
	std::vector<double> nextMean; // 위치별 평균값 (다음 평균 패턴)
	std::vector<double> cov; // 위치별 공분산

	std::vector<double> sumMatch; // 각 위치에서 매칭되는 모든 값 다 더해놓음
	std::vector<double> squaredSum; // 각 위치에서 매칭되는 모든 값의 제곱 다 더해놓음
	std::vector<int> cntMatch; // 각 위치에서 매칭되는 값의 갯수 세줌

	/* 평균패턴이 변하는 동안 while문 */

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

		/* 1. DTW로 평균패턴과 그와 패턴 각각의 매칭 위치 */

		for (int i = 0; i < 10; ++i) {
			if (isFirst && i == 0) {
				isFirst = false;
				continue;
			}
			DTW(currMean, h[i], sumMatch, squaredSum, cntMatch);
		}

		/* 2. 매칭위치의 값들로 새로운 평균패턴 생성 */

		for (int i = 0; i < nextMean.size(); ++i) {
			nextMean[i] = sumMatch[i] / (double)cntMatch[i];
		}

		/* 3. 평균패턴이 같은지 변하는지 체크 */
		bool isSame = true;
		for (int i = 0; i < nextMean.size(); ++i) {
			if (abs(currMean[i] - nextMean[i]) >= 1) {
				isSame = false;
				break;
			}
		}

		/* 평균패턴이 같으면 위치별 공분산을 구해주고 while문 끝냄 */
		if (isSame) {

			// 위치별 공분산을 구함
			for (int i = 0; i < squaredSum.size(); ++i) {
				cov[i] = (squaredSum[i] / (double)cntMatch[i]) - pow(nextMean[i], 2);
			}

			break;
		}
		else { /* 평균패턴이 다르면 새로운 평균패턴으로 다시 while문 돔 */
			currMean = nextMean;
		}
	}

	std::vector<std::pair<double, double>> ans;

	for (int i = 0; i < nextMean.size(); ++i) {
		ans.push_back(std::make_pair(nextMean[i], cov[i]));
	}

	return ans;

}

/* 평균패턴과 공분산에 기반한 통계적 거리를 기반으로 DTW를 실행하여 score 계산 */
double DTW_Match(std::vector<std::pair<double, double>> final_pattern, std::vector<double> &B)
{
	/* final_pattern: 각 위치의 (평균패턴, 공분산), B: test data */

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
	D[0][0] = -0.5 * pow((final_pattern[0].first - B[0]), 2) / final_pattern[0].second; // 두 값의 통계적 거리
	G[0][0] = 0; // 필요 없는 값이므로 0으로 초기화

	for (int j = 1; j < M; j++) {
		D[0][j] = D[0][j - 1] + -0.5 * pow((final_pattern[0].first - B[j]), 2) / final_pattern[0].second;  // 두 값의 통계적 거리
		G[0][j] = 2;
	}
	for (int i = 1; i < N; i++)
		D[i][0] = DBL_MAX;

	printf("done");

	//  Forward
	for (int i = 1; i < N; i++) {
		for (int j = 1; j < M; j++) {
			double minVal = min({ D[i - 1][j], D[i][j - 1], D[i - 1][j - 1] });
			D[i][j] = (-0.5 * pow((final_pattern[i].first - B[j]), 2) / final_pattern[i].second) + minVal; // 두 값의 통계적 거리 + minVal;
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
	int k = 1; // 경로는 뒤에서부터 거꾸로 저장됨 -> 출력시 뒤집어서 할 것

	while ((i != 0) && (j != 0)) { // i와 j가 모두 0이 아닐 때까지 반복
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

	double S = D[N - 1][M - 1] / k; // 매칭된 길이를 k로 나눠줌 (dissimilarity)

	return S;

}


void CRGBDlg::OnBnClickedImgSave()
{
	imwrite("copy.jpg", img);

	MessageBox(_T("이미지 저장 완료!"), _T(""));
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
	//OnBnClickedRedBtn 참고하여 작성하시오.
	Mat img_copy = img.clone();
	changeColor(img, img_copy, 5);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}


void CRGBDlg::OnBnClickedMorphOpenBtn()
{
	// 열림 영상 저장
	Mat img_copy = img.clone();
	img_copy = opening(img_copy);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}

void CRGBDlg::OnBnClickedMorphCloseBtn()
{
	// 닫힘 영상 저장
	Mat img_copy = img.clone();
	img_copy = closing(img_copy);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}

void CRGBDlg::OnBnClickedLabelBtn()
{
	// Label 영상 저장
	Mat img_copy = img.clone();

	xTrace_gl.resize(0);
	yTrace_gl.resize(0);
	img_copy = contourTracing(img_copy, xTrace_gl, yTrace_gl);

	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}

void CRGBDlg::OnBnClickedTrainBtn()
{
	MessageBox(_T("train start! (시간이 많이 소요되니 주의하세요)"), _T(""));

	circle_pattern = SDTW(circle);

	MessageBox(_T("circle 학습완료"), _T(""));

	star_pattern = SDTW(star);

	MessageBox(_T("star 학습완료"), _T(""));

	triangle_pattern = SDTW(triangle);

	MessageBox(_T("triangle 학습완료"), _T(""));

	square_pattern = SDTW(square);

	MessageBox(_T("square 학습완료"), _T(""));

	MessageBox(_T("train finish!"), _T(""));

}


void CRGBDlg::OnBnClickedMatchBtn()
{	
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.jpg;*.gif;*.png |모든파일(*.*)|*.*||");
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

		// 통계적 거리에 기반하여 각 class의 score 산출
		double circleScore = DTW_Match(circle_pattern, hTest);
		double starScore = DTW_Match(star_pattern, hTest);
		double triangleScore = DTW_Match(triangle_pattern, hTest);
		double squareScore = DTW_Match(square_pattern, hTest);

		// test image는 가장 낮은 score를 가진 class에 속하므로 속한 class를 출력

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
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
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
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
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
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
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
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자

				//RGB 값을 더해서 3으로 나눈 값을 각각의 원소에 넣어준다
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
		long int H[256]; // Histogram을 만듦
		memset(H, 0, sizeof(H)); // Histogram을 0으로 초기화

		// 정규화된 Historgram
		double NH[256];
		memset(NH, 0, sizeof(NH)); // Normalized Histogram을 0으로 초기화

		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x < copy.cols; x++)
			{
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
				H[ptr1[3 * x + 0]]++;
			}
		}

		// 화소 수에 상관 없이 항상 똑같이 히스토그램을 계산하기 위해 Histogram의 값을 전체 화소 수로 나눔
		for (int i = 0; i < 256; ++i)
		{
			NH[i] = (double)H[i] / (copy.rows * copy.cols);
		}

		// 전체 평균 계산
		double M_t = 0;
		for (int i = 0; i < 256; ++i)
			M_t = i * NH[i];

		// 각각의 가중치 값 계산
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

		// 각각의 평균값 계산
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

		// 평균 사이의 거리의 가중평균 계산
		double var[256] = { 0 };

		for (int i = 1; i < 255; ++i)
		{
			var[i] = (w0[i] * (m0[i] - M_t) * (m0[i] - M_t)) + (w1[i] * (m1[i] - M_t) * (m1[i] - M_t));
		}

		// T 값을 계산 -> argmax of var
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

		// Otsu's segmentation (이진화) 진행
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x < copy.cols; x++)
			{
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
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






