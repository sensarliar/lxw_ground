// testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"
//#include <math.h>
#include "dbw_rd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////


//#include <inttypes.h>
#include <string.h>
//#include <math.h>
//#include <stdlib.h>


#define FLAG_FDJ_BIT 0
#define FLAG_WQ_BIT 1
#define FLAG_QLJ_BIT 2
#define FLAG_JY_BIT 5


struct DbwInfo dbw_info;
char dbw_info_flag = 0x39;
		//now addr_   uint8_t INFO_ADDR[4]={0x04,0xE6,0x10};//291822  addr 3B
char INFO_ADDR[4] = {0x04,0x73,0xEE,0x0};
char RCV_CMD[255];
int cmd_rcv_flag;
int cmd_ch_num;

int rx_counts;
int timer_cnt_60ms = 0;




void parse_dbw_info_GM002(void);


void dbw_info_impl_init( void ) {
  dbw_info.msg_available = FALSE;
//  dbw_info.pos_available = FALSE;
//  dbw_info.gps_nb_ovrn = 0;
  dbw_info.msg_len = 0;
	dbw_info.set_len = DBW_MAXLEN-1;
	dbw_info.set_temp_len = DBW_MAXLEN-1;
}



/**
 * parse GPGGA-nmea-messages stored in
 * dbw_info.msg_buf .
 */
void parse_dbw_info_GM002(void) {
  int i = 7;     // current position in the message, start after: GPGGA,
	int j = 0;
	int temp_len = 255;


		//now addr_   uint8_t INFO_ADDR[4];//291822  addr 3B
	if(dbw_info.msg_buf[i] != INFO_ADDR[0])
	{
	return;
	}
	i++;
		if(dbw_info.msg_buf[i] != INFO_ADDR[1])
	{
	return;
	}
		i++;
		if(dbw_info.msg_buf[i] != INFO_ADDR[2])
	{
	return;
	}
	
///crc error	
	
	if(dbw_info.msg_buf[dbw_info.msg_len-2] != 0)
	{

    return;
	}
//	i+=6;
	i=16;
	temp_len = ((dbw_info.msg_buf[i] << 8) + dbw_info.msg_buf[i+1])/8;
	if(temp_len > dbw_info.msg_len-1-20 +1)
	{
		return;
	}
	//form 18 0xA4;
	for(j=0;j<(dbw_info.msg_len-2-19);j++)
	{
		RCV_CMD[j] = dbw_info.msg_buf[19+j];
	}
	RCV_CMD[j]='\0';
	cmd_ch_num=j;
	cmd_rcv_flag = 1;


	

//END parse GGA
}


/**
 * parse_nmea_char() has a complete line.
 * Find out what type of message it is and
 * hand it to the parser for that type.
 */
void dbw_info_parse_msg( void ) {

//  if(dbw_info.msg_len > 5 && !strncmp(dbw_info.msg_buf , "GMING", 5)) {
	if(dbw_info.msg_len > 5 && !strncmp(dbw_info.msg_buf , "$TXXX", 5)) {
    dbw_info.msg_buf[dbw_info.msg_len] = 0;

		parse_dbw_info_GM002();
  //  parse_nmea_GPRMC();
  }
  else {
    
        dbw_info.msg_buf[dbw_info.msg_len] = 0;

      }

  // reset message-buffer
  dbw_info.msg_len = 0;
	dbw_info.set_len = DBW_MAXLEN-1;
	dbw_info.set_temp_len = DBW_MAXLEN-1;
}
/**
 * This is the actual parser.
 * It reads one character at a time
 * setting dbw_info.msg_available to TRUE
 * after a full line.
 */
void dbw_info_parse_char( char c ) {
  //reject empty lines


  // fill the buffer, unless it's full
  if (dbw_info.msg_len < DBW_MAXLEN - 1) {

    // messages end with a linefeed
    //AD: TRUNK:       if (c == '\r' || c == '\n')
    if (dbw_info.msg_len < dbw_info.set_len-1) {
			dbw_info.msg_buf[dbw_info.msg_len] = c;
      dbw_info.msg_len ++;
    } else {
			dbw_info.msg_buf[dbw_info.msg_len] = c;
      dbw_info.msg_len ++;
			dbw_info.msg_available = TRUE;
    }
		if(dbw_info.msg_len==6)
		{
			dbw_info.set_temp_len = c;
		}
		if(dbw_info.msg_len==7)
		{
			dbw_info.set_temp_len = (dbw_info.set_temp_len << 8) + c;
			if((dbw_info.set_temp_len>=11)&&(dbw_info.set_temp_len<=230))
			{
				dbw_info.set_len = dbw_info.set_temp_len;
			}
		}
		if ( c == '$')
		{
      dbw_info.msg_buf[0] = c;
      dbw_info.msg_len =1;   
			dbw_info.set_len = DBW_MAXLEN-1;
			dbw_info.set_temp_len = DBW_MAXLEN-1;
		}

  }

  if (dbw_info.msg_len >= DBW_MAXLEN - 1)
  {
	   dbw_info.msg_available = TRUE;
  }
}




/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//m_brush.CreateSolidBrush(RGB(255,255,255));
	m_brush.CreateSolidBrush(TRANSPARENT);
	
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
	DDX_Control(pDX, IDC_BUTTON1, m_com1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, OnSend)
	ON_BN_CLICKED(IDC_BUTTON1, OnOpen)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ON_COM_RECEIVE, OnRecvComData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM0");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM1");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM2");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM3");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM4");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM5");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM6");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM7");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM8");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM9");
	
	

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	Bytes = 0;
	rx_counts=0;
	// TODO: Add extra initialization here
	//InintCom(2);
	//m_com.write("TXXXX");
	//m_com.write("m_shuju=$GPZDA,032232.00,14,05,2009,00,00*67,$GPZDA");
	//OnRecvComData();
//	rx_counts=0;
	
	
	dbw_info_impl_init();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		/*CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect,RGB(30,144,255));*/
		
		CDialog::OnPaint();
	}
		//OnRecvComData();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool CTestDlg::InintCom(int port)
{
	
	if(!m_com.open(port))//打开串口2
	{
		MessageBox("串口打开失败", "COM15", MB_OK);
		return false;
	}
	
	else
	{
		m_com.set_hwnd(m_hWnd);//设置窗口句柄
		((CComboBox*)GetDlgItem(IDC_COMBO5))->EnableWindow(FALSE);
	}

	return true;
}
char receive_buf[1024] = {0};
char buf_temp[512];

CString Rx_win_text;
BOOL CTestDlg::OnRecvComData()
{
	int i;
	int bytes_temp = 1;
	bytes_temp = m_com.read((char *)&receive_buf, 1024);
//	Bytes += bytes_temp;
	if(bytes_temp <= 0)
	{
		return false;
	}
	//SetDlgItemText(IDC_DEBUG,receive_buf);
	
	i = 0;
	while(i<bytes_temp )
	{
		if(!receive_buf[i])
		{
			buf_temp[i]=' ';
		}else
		{
			buf_temp[i]=receive_buf[i];
		}
		i++;
	}
	buf_temp[i] = '\0';

	Rx_win_text += buf_temp;

	if(Rx_win_text.GetLength()>2048)
	{
		Rx_win_text="  ";
	}
	GetDlgItem(IDC_DEBUG)->SetWindowText(Rx_win_text);

	i=0;
	bytes_temp++;
	while((bytes_temp--)&&!dbw_info.msg_available)
	{

		if(bytes_temp > 0)
		{
			dbw_info_parse_char(receive_buf[i++]);
		}
		
	}

                                                
    if (dbw_info.msg_available) {                     
      dbw_info_parse_msg();                                 
      dbw_info.msg_available = FALSE;                   
    }   




	//显示
	if(cmd_rcv_flag)
	{
	

		strBuffer=RCV_CMD;

		char  m_hour[100];
		CString m_min;
		CString m_sec;
		CString m_day;
		CString m_month;
		CString m_year;
		CString m_jihao;
		CString m_fadongji;
		char fadongji;
		char fadongji1;
		char fadongji2;

		CString fadongji1_str;
		CString fadongji2_str;

		CString m_weidu;
		CString m_weiduf;
		CString m_wei;
		CString m_jingdu;
		CString m_jingduf;
		CString m_jing;
		CString m_haiba;
		CString m_roll;
		CString m_pitch;
		CString m_yaw;


		//char ch0;

		//机号信息
		m_jihao=strBuffer.Left(3);
		SetDlgItemText(IDC_JIHAO,m_jihao);
		
		fadongji=strBuffer.GetAt(3);
		fadongji1=((fadongji>>1)&0x1)+'0';
		fadongji1_str=CString(fadongji1);
		fadongji2=((fadongji>>5)&0x1)+'0';
		fadongji2_str=CString(fadongji2);
		SetDlgItemText(IDC_FDJ1,fadongji1_str);
		SetDlgItemText(IDC_FDJ2,fadongji2_str);
		


		//时间信息
		hour=strBuffer.Mid(4,2);
		itoa((atoi(hour)+8)%24,m_hour,10);
		SetDlgItemText(IDC_Hour,m_hour);

		m_min=strBuffer.Mid(6,2);
		SetDlgItemText(IDC_Min,m_min);

		m_sec=strBuffer.Mid(8,5);
		SetDlgItemText(IDC_Sec,m_sec);

		
		//位置信息

		m_weidu=strBuffer.Mid(13,2);
		m_weiduf=strBuffer.Mid(15,9);
		SetDlgItemText(IDC_Weidu,m_weidu);
		SetDlgItemText(IDC_Weiduf,m_weiduf);
	//#if 0	
		m_wei=strBuffer.Mid(24,1);

		if(m_wei.GetAt(0)=='N')
			m_wei="北纬";
		else m_wei="南纬";//定位信息提取
		SetDlgItemText(IDC_Wei,m_wei);
					
		
		m_jingdu=strBuffer.Mid(25,3);//经度信息提取
		m_jingduf=strBuffer.Mid(28,9);
		SetDlgItemText(IDC_Jingdu,m_jingdu);
		SetDlgItemText(IDC_Jingduf,m_jingduf);
					
		
		m_jing=strBuffer.Mid(37,1);
		if(m_jing.GetAt(0)=='W')
			m_jing="西经";
		else m_jing="东经";//定位信息提取
		SetDlgItemText(IDC_Jing,m_jing);


		m_haiba=strBuffer.Mid(38,5);//海拔信息提取
		SetDlgItemText(IDC_Haiba,m_haiba);


		m_day=strBuffer.Mid(43,2);
		SetDlgItemText(IDC_Day,m_day);
						
				
						
		m_month=strBuffer.Mid(45,2);
		SetDlgItemText(IDC_Month,m_month);
						
						
		m_year=strBuffer.Mid(47,2);
		SetDlgItemText(IDC_Year,m_year);
		
		int index_comma;
		int index_comma_pre;
		index_comma = strBuffer.Find(',',49);
		m_roll=strBuffer.Mid(49,index_comma-49);
		SetDlgItemText(IDC_ROLL,m_roll);
		index_comma_pre=index_comma+1;

		index_comma = strBuffer.Find(',',index_comma_pre);
		m_pitch=strBuffer.Mid(index_comma_pre,index_comma-index_comma_pre);
		SetDlgItemText(IDC_PITCH,m_pitch);
		index_comma_pre=index_comma+1;

		index_comma = strBuffer.Find(',',index_comma_pre);
		m_yaw=strBuffer.Mid(index_comma_pre,index_comma-index_comma_pre);
		SetDlgItemText(IDC_YAW,m_yaw);
						
	//#endif				
			strBuffer.Empty();
			char m_rx_counts[10];
			cmd_rcv_flag = 0;
			rx_counts++;
			itoa(rx_counts,m_rx_counts,10);
			SetDlgItemText(IDC_COUNT,m_rx_counts);

	}


	return true;
}

char m_count_djs[10];

void CTestDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 1)
	{
		OnRecvComData();
		
		if(timer_cnt_60ms>0)
		{
			timer_cnt_60ms--;
			if(timer_cnt_60ms==0)
			{
				GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
			}
		}
		
		itoa(int(timer_cnt_60ms*60/1000),m_count_djs,10);
		SetDlgItemText(IDC_DJS,m_count_djs);


	}

	CDialog::OnTimer(nIDEvent);
}


void CTestDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
	
	
	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO5))->GetCurSel();
	
	InintCom(nIndex);
	SetTimer(1,60,0);
//	Delay(10);

	char ICJC_MSG[20]={0x24,0x49,0x43,0x4a,0x43,0x00,0x0c,0x00,0x00,0x00,0x00,0x2b,0x0d,0x0a};
	m_com.write(ICJC_MSG,14);


}


// Len of HoTT serial buffer
int hott_msg_len = 0;

//uint8_t MSG_TO_SEND[43]="WWW.UCORTEX.COM---gaoming,gaoming,nihao ma";
char INFO_HEAD[6]="$TXSQ";   //5B
char INFO_LEN[2]={0x33,0x34};  //2B
//char INFO_ADDR[4]={0x04,0xE6,0x10};//321040  addr 3B
//uint8_t MSG_TX_FLAG=0B01000110;     //1B
char MSG_TX_FLAG=0x46;     //1B
//uint8_t MSG_TX_ADDR[4]={0x04,0xE6,0x10};//321040  TX addr   //3B
char MSG_TX_ADDR[4]={0x04,0xE6,0x10};//  TX addr   //3B
char MSG_TX_LEN[2]={0x00,0x00};      //2B
char MSG_TX_ACK=0x00;               //1B
char MSG_TX[211]="gaoming";    //1680bit maxium   NON-MIL 628BIT  78B   17B+MSG +1CRC
int msg_num=0;  //neirong



char ALL_HEAD[18];

void init_msg_head(void){
			 int i,j;
			hott_msg_len=17+1+msg_num+1;
			INFO_LEN[1]=(char)(hott_msg_len);
//  	INFO_LEN[1]=(char)*(hott_msg_len&0xFF00);
			INFO_LEN[0]= hott_msg_len>>8;
			MSG_TX_LEN[1]=(msg_num+1)*8;
			MSG_TX_LEN[0]=(msg_num+1)*8 >> 8;
	
			for(i=0,j=0; i<5; i++,j++){
			ALL_HEAD[i]=INFO_HEAD[j];
			}
			for(j=0; j<2; i++,j++){
			ALL_HEAD[i]=INFO_LEN[j];
			}
			for(j=0; j<3; i++,j++){
			ALL_HEAD[i]=INFO_ADDR[j];
			}

			ALL_HEAD[i]=MSG_TX_FLAG;
			i++;
//			ALL_HEAD[i]=0x46;

			for(j=0; j<3; i++,j++){
			ALL_HEAD[i]=MSG_TX_ADDR[j];
			}			
			for(j=0; j<2; i++,j++){
			ALL_HEAD[i]=MSG_TX_LEN[j];
			}	
		
			ALL_HEAD[i]=MSG_TX_ACK;
			i++;	
			ALL_HEAD[i]=0xa4;
			i++;

			
}



//static void send_one_frame_data(void) {
void send_one_frame_data(void) {
    char msg_crc = 0;
//	int init_len;
	char *hott_msg_ptr = 0;
//	char temp;

//	char all_bytes[1024];

	/*
  if (hott_msg_len == 0) {
    hott_msg_ptr = 0;
//    hott_telemetry_is_sending = FALSE;
//    hott_telemetry_sendig_msgs_id = 0;
    msg_crc = 0;
  }
  else {
    --hott_msg_len;
    if (hott_msg_len != 0) {
      msg_crc += *hott_msg_ptr;
      uart_transmit(&uart1, *hott_msg_ptr++);
    } else
      uart_transmit(&uart1, (int8_t)msg_crc);
  }
	*/
	

/*
	init_len=hott_msg_len;
	while(hott_msg_len){
	    --hott_msg_len;
    if (hott_msg_len != 0) {
			if(hott_msg_len == init_len-19)
			{hott_msg_ptr = MSG_TX;}
			
//      msg_crc = msg_crc  *hott_msg_ptr;
			msg_crc ^= *hott_msg_ptr;

//      uart_transmit(&uart2, *hott_msg_ptr++);
				
		} 
	}
*/	
	msg_crc = 0;
	hott_msg_ptr = ALL_HEAD;

	int len=18;
	while(len)
	{
		msg_crc ^= *hott_msg_ptr;
		len--;
		hott_msg_ptr++;
	}

	len=msg_num;
	hott_msg_ptr = MSG_TX;
	while(len)
	{
		msg_crc ^= *hott_msg_ptr;
		len--;
		hott_msg_ptr++;
	}

	MSG_TX[msg_num]=msg_crc;
	MSG_TX[msg_num+1]=0x0d;
	MSG_TX[msg_num+2]=0x0a;


//	uart_transmit(&uart2, temp);	

}


void CTestDlg::OnSend() 
{
	// TODO: Add your control notification handler code here
	CString text;
	GetDlgItem(IDC_SEND)->GetWindowText(text);

   // int bytes_temp=1;
	//bytes_temp = m_com.write((char *)&text, 1024);
	CString phone_num_str;
	int phone_num;
	GetDlgItem(IDC_EDIT_PHONE_NUM)->GetWindowText(phone_num_str);
/*
	if(phone_num_str)
	{
	phone_num=atoi(phone_num_str);
	MSG_TX_ADDR[2]=phone_num;
	MSG_TX_ADDR[1]=phone_num>>8;
	MSG_TX_ADDR[0]=phone_num>>8;


	}
	*/

	char * text_tx;
	text_tx= text.GetBuffer(0);
	int i=0;
	while(*text_tx)
	{
		MSG_TX[i++]=*text_tx;
		text_tx++;
	}
	msg_num = i;
	
	init_msg_head();
	send_one_frame_data();
	m_com.write(ALL_HEAD,18);
	m_com.write(MSG_TX,msg_num+3);

//	timer_cnt_60ms=1000;
	timer_cnt_60ms=1016;
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);


//	m_com.write(&text_tx_p);
//	int bytes_temp=1;
//	bytes_temp  = m_com.write((char *)&text_tx);
}


