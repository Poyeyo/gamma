/////////////////////////////////////////////////////////////////////////////
// Name:		gamma.cpp
// Purpose:	 Gamma Control
// Author:	  Poyeyo
// Modified by:
// Created:	 21/08/05
// Copyright:   (c)
// Licence:	 MIT licence
/////////////////////////////////////////////////////////////////////////////

#define GAMMA_CODE true

#ifdef GAMMA_CODE
	#include <windows.h>
	#include <wingdi.h>
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/taskbar.h>
#include <wx/checkbox.h>
#include <wx/image.h>
#include <wx/slider.h>
#include <wx/dcbuffer.h>
#include <wx/listbox.h>
#include <wx/config.h>
#include <wx/snglinst.h>

class process;
class MyDialog;

// Define a new application
class MyApp: public wxApp
{
public:
	bool OnInit(void);
	int  OnExit(void);
	MyDialog   *dialog;
	
	wxSingleInstanceChecker* singlechecker;
	process* daprocess;
	
protected:
};

IMPLEMENT_APP(MyApp)

class MyTaskBarIcon: public wxTaskBarIcon
{
public:
	MyTaskBarIcon()
	{}
	
	void OnLeftButtonDClick(wxTaskBarIconEvent&);
	void OnMenuRestore(wxCommandEvent&);
	void OnMenuExit(wxCommandEvent&);
	//void OnMenuSetNewIcon(wxCommandEvent&);
	//void OnMenuCheckmark(wxCommandEvent&);
	//void OnMenuUICheckmark(wxUpdateUIEvent&);
	void OnTaskMenuPreset(wxCommandEvent& event);
	virtual wxMenu *CreatePopupMenu();
	
	DECLARE_EVENT_TABLE()
};


class MyDialog: public wxDialog
{
public:
	MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
		const wxPoint& pos, const wxSize& size, const long windowStyle = wxDEFAULT_DIALOG_STYLE);
	~MyDialog();
	
	void OnOK(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnCloseWindow(wxCloseEvent& event);
	void OnPaint( wxPaintEvent &event );
	void UpdateGamma(WXTYPE);
	void Init(void);
	
	MyTaskBarIcon   *m_taskBarIcon;
	wxImage *m_Image;
	wxBitmap *m_Bitmap;
	wxBitmap *Pattern;
	bool capable;
	
#ifdef GAMMA_CODE
	WORD gammaramp[768];
	WORD gammaorig[768];
	HDC devcont;
#endif
	int gamm, red, green, blue;
	wxStaticText *sgamma, *sred, *sgreen, *sblue;
	wxSlider *slred, *slgreen, *slblue;
	wxCheckBox *linked;
	wxChoice* presets;
	wxTextCtrl* presetlab;
	
protected:
DECLARE_EVENT_TABLE()
};

class process : public wxEvtHandler
{
public:
	void Init(void);
	void OnGammaSlider(wxScrollEvent& event);
	void OnRedSlider(wxScrollEvent& event);
	void OnGreenSlider(wxScrollEvent& event);
	void OnBlueSlider(wxScrollEvent& event);
	void OnLinkCheck(wxCommandEvent& event);
	
	void OnDefault(wxCommandEvent& event);
	void OnApplyPreset(wxCommandEvent& event);
	void OnDeletePreset(wxCommandEvent& event);
	void OnNewPreset(wxCommandEvent& event);
	
private:
	wxSlider *sred, *sgreen, *sblue;
	
	DECLARE_EVENT_TABLE()
};

enum {
	ID_GAMMA = 10001,
	ID_RED,
	ID_GREEN,
	ID_BLUE,
	ID_DEFAULT,
	ID_LINK,
	ID_APPLY,
	ID_DELETE,
	ID_NEWP,
	ID_PRESM
};


BEGIN_EVENT_TABLE(process, wxEvtHandler)
	EVT_COMMAND_SCROLL(ID_GAMMA, process::OnGammaSlider)
	EVT_COMMAND_SCROLL(ID_RED, process::OnRedSlider)
	EVT_COMMAND_SCROLL(ID_GREEN, process::OnGreenSlider)
	EVT_COMMAND_SCROLL(ID_BLUE, process::OnBlueSlider)
	EVT_CHECKBOX(ID_LINK, process::OnLinkCheck)
	EVT_BUTTON(ID_DEFAULT, process::OnDefault)
	EVT_BUTTON(ID_APPLY, process::OnApplyPreset)
	EVT_BUTTON(ID_DELETE, process::OnDeletePreset)
	EVT_BUTTON(ID_NEWP, process::OnNewPreset)
END_EVENT_TABLE()


bool MyApp::OnInit(void)
{
	SetVendorName("MacondoBits");
	SetAppName("Gamma Control");
	
	wxSingleInstanceChecker* singlechecker = new wxSingleInstanceChecker("Gamma Control");
	if (singlechecker->IsAnotherRunning()) 
	{
		return FALSE;
	}
	
	daprocess = new process();
	// Create the main frame window
	dialog = new MyDialog(NULL, wxID_ANY, wxT("MacondoBits Gamma Control"), wxDefaultPosition, wxSize(600, 490));
	
	//dialog->Show(true);
	dialog->Init();
	
	return true;
}

int MyApp::OnExit()
{
	delete singlechecker;
	return 0;
}


void
process::Init() 
{
	/*sred   = (wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  );
	sgreen = (wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN);
	sblue  = (wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE );*/
}

void
process::OnGammaSlider(wxScrollEvent& event)
{
	int val = wxGetApp().dialog->gamm = event.GetInt();
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->SetValue(val);
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->SetValue(val);
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->SetValue(val);
	double fgamma = event.GetInt()*2.5/256+0.5;
	wxString gammalab;
	gammalab.Printf("%.2lf", fgamma);
	wxGetApp().dialog->sgamma->SetLabel(gammalab);
	/*wxGetApp().dialog->sred  ->SetLabel("");
	wxGetApp().dialog->sgreen->SetLabel("");
	wxGetApp().dialog->sblue ->SetLabel("");*/
	
	wxGetApp().dialog->red = event.GetInt();
	wxGetApp().dialog->green = event.GetInt();
	wxGetApp().dialog->blue = event.GetInt();
	wxGetApp().dialog->UpdateGamma(event.GetEventType());
}

void
process::OnRedSlider(wxScrollEvent& event)
{
	wxGetApp().dialog->red = event.GetInt();
	wxGetApp().dialog->UpdateGamma(event.GetEventType());
}

void 
process::OnGreenSlider(wxScrollEvent& event)
{
	wxGetApp().dialog->green = event.GetInt();
	wxGetApp().dialog->UpdateGamma(event.GetEventType());
}

void 
process::OnBlueSlider(wxScrollEvent& event)
{
	wxGetApp().dialog->blue = event.GetInt();
	wxGetApp().dialog->UpdateGamma(event.GetEventType());
}


void 
process::OnLinkCheck(wxCommandEvent& event)
{
	wxCheckBox* linked = (wxCheckBox*)event.GetEventObject();
	bool link = linked->IsChecked();
	wxGetApp().dialog->FindWindow(ID_GAMMA)->Enable(link);
	wxGetApp().dialog->FindWindow(ID_RED)->Enable(!link);
	wxGetApp().dialog->FindWindow(ID_GREEN)->Enable(!link);
	wxGetApp().dialog->FindWindow(ID_BLUE)->Enable(!link);
}

void
process::OnDefault(wxCommandEvent& event)
{
	wxString presetkey;
	long red=51, green=51, blue=51;
	
	presetkey.Printf("/default/");
	wxConfigBase *pConfig = wxConfigBase::Get();
	pConfig->Write(presetkey, "");
	
	wxGetApp().dialog->linked->SetValue(true);
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->Disable();
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->Disable();
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->Disable();/**/
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_GAMMA))->Enable();
	
	wxGetApp().dialog->red   = red;
	wxGetApp().dialog->green = green;
	wxGetApp().dialog->blue  = blue;
	wxGetApp().dialog->gamm  = 51;
	
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->SetValue(red  );
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->SetValue(green);
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->SetValue(blue );
	((wxSlider*)wxGetApp().dialog->FindWindow(ID_GAMMA))->SetValue(wxGetApp().dialog->gamm);
	double fgamma = wxGetApp().dialog->gamm*2.5/256+0.5;
	wxString gammalab;
	gammalab.Printf("%.2lf", fgamma);
	wxGetApp().dialog->sgamma->SetLabel(gammalab);
	/*wxGetApp().dialog->sred  ->SetLabel("");
	wxGetApp().dialog->sgreen->SetLabel("");
	wxGetApp().dialog->sblue ->SetLabel("");*/
	wxGetApp().dialog->UpdateGamma(wxEVT_SCROLL_CHANGED);
}

void
process::OnApplyPreset(wxCommandEvent& event)
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	wxString presetname = wxGetApp().dialog->presets->GetStringSelection();
	if (!presetname.IsEmpty())
	{
		wxString presetkey;
		long red=wxGetApp().dialog->red, green=wxGetApp().dialog->green, blue=wxGetApp().dialog->blue;
		presetkey.Printf("/presets/%s/Red", presetname.c_str());
		pConfig->Read(presetkey, &red);
		presetkey.Printf("/presets/%s/Green", presetname.c_str());
		pConfig->Read(presetkey, &green);
		presetkey.Printf("/presets/%s/Blue", presetname.c_str());
		pConfig->Read(presetkey, &blue);
		
		presetkey.Printf("/default/");
		pConfig->Write(presetkey, presetname);
		
		if ((red == green) && (red == blue))
		{
			wxGetApp().dialog->linked->SetValue(true);
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->Disable();
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->Disable();
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->Disable();/**/
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_GAMMA))->Enable();
		} else
		{
			wxGetApp().dialog->linked->SetValue(false);//
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->Enable();
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->Enable();
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->Enable();/**/
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_GAMMA))->Disable();
		}
		
		wxGetApp().dialog->red   = red;
		wxGetApp().dialog->green = green;
		wxGetApp().dialog->blue  = blue;
		wxGetApp().dialog->gamm  = (red + green + blue)/3;
		
		((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->SetValue(red  );
		((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->SetValue(green);
		((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->SetValue(blue );
		((wxSlider*)wxGetApp().dialog->FindWindow(ID_GAMMA))->SetValue(wxGetApp().dialog->gamm);
		double fgamma = wxGetApp().dialog->gamm*2.5/256+0.5;
		wxString gammalab;
		gammalab.Printf("%.2lf", fgamma);
		wxGetApp().dialog->sgamma->SetLabel(gammalab);
		/*wxGetApp().dialog->sred  ->SetLabel("");
		wxGetApp().dialog->sgreen->SetLabel("");
		wxGetApp().dialog->sblue ->SetLabel("");*/
		wxGetApp().dialog->UpdateGamma(wxEVT_SCROLL_CHANGED);
	}
}

void
process::OnDeletePreset(wxCommandEvent& event)
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	wxString presetname = wxGetApp().dialog->presets->GetStringSelection();
	if (!presetname.IsEmpty())
	{
		wxString presetkey;
		presetkey.Printf("/presets/%s", presetname.c_str());
		pConfig->DeleteGroup(presetkey);
		int n = wxGetApp().dialog->presets->FindString(presetname);
		if (n != wxNOT_FOUND)
			wxGetApp().dialog->presets->Delete(n);
	}
}

void
process::OnNewPreset(wxCommandEvent& event)
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	wxString presetname;
	presetname.Printf("/presets/%s/Red", wxGetApp().dialog->presetlab->GetValue().c_str());
	pConfig->Write(presetname, wxGetApp().dialog->red);
	presetname.Printf("/presets/%s/Green", wxGetApp().dialog->presetlab->GetValue().c_str());
	pConfig->Write(presetname, wxGetApp().dialog->green);
	presetname.Printf("/presets/%s/Blue", wxGetApp().dialog->presetlab->GetValue().c_str());
	pConfig->Write(presetname, wxGetApp().dialog->blue);
	pConfig->Write("/default/", presetname);
	int n = wxGetApp().dialog->presets->FindString(wxGetApp().dialog->presetlab->GetValue());
	if (n == wxNOT_FOUND)
		wxGetApp().dialog->presets->Append(wxGetApp().dialog->presetlab->GetValue());
	wxGetApp().dialog->presets->Select(wxGetApp().dialog->presets->FindString(wxGetApp().dialog->presetlab->GetValue()));
	pConfig->Flush();
}

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
	EVT_BUTTON(wxID_OK, MyDialog::OnOK)
	EVT_BUTTON(wxID_ABOUT, MyDialog::OnAbout)
	EVT_BUTTON(wxID_EXIT, MyDialog::OnExit)
	EVT_PAINT(MyDialog::OnPaint)
	EVT_CLOSE(MyDialog::OnCloseWindow)
END_EVENT_TABLE()

MyDialog::MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long windowStyle):
	wxDialog(parent, id, title, pos, size, windowStyle)
{
	Centre(wxBOTH);
	
	SetIcon(wxICON(gamma));
	//(void) new wxStaticText(this, wxID_ANY, _T("Press 'Hide me' to hide me, Exit to quit."), wxPoint(20, 430));
	
	(void) new wxButton(this, wxID_EXIT, _T("Exit"), wxPoint(490, 420), wxSize(80, 25));
	(void) new wxButton(this, wxID_ABOUT, _T("About"), wxPoint(400, 420), wxSize(80, 25));
	(new wxButton(this, wxID_OK, _T("Hide me"), wxPoint(310, 420), wxSize(80, 25)))->SetDefault();
	
	(void) new wxStaticText(this, wxID_ANY, _T("Gamma:"), wxPoint(20, 305));
	(void) new wxSlider(this, ID_GAMMA, 208, 0, 256, wxPoint(65, 305), wxSize(260, 25), wxSL_HORIZONTAL|wxSL_BOTTOM);
	(void) new wxStaticText(this, wxID_ANY, _T("Red:"), wxPoint(20, 335));
	(void) new wxSlider(this, ID_RED,   208, 0, 256, wxPoint(65, 335), wxSize(260, 25), wxSL_HORIZONTAL|wxSL_BOTTOM);
	(void) new wxStaticText(this, wxID_ANY, _T("Green:"), wxPoint(20, 365));
	(void) new wxSlider(this, ID_GREEN, 208, 0, 256, wxPoint(65, 365), wxSize(260, 25), wxSL_HORIZONTAL|wxSL_BOTTOM);
	(void) new wxStaticText(this, wxID_ANY, _T("Blue:"), wxPoint(20, 395));
	(void) new wxSlider(this, ID_BLUE,  208, 0, 256, wxPoint(65, 395), wxSize(260, 25), wxSL_HORIZONTAL|wxSL_BOTTOM);
	
	//(void) new wxStaticText(this, wxID_ANY, _T("Link RGB:"), wxPoint(360, 300));
	linked = new wxCheckBox(this, ID_LINK, _T("Link RGB:"), wxPoint(70, 430), wxDefaultSize, wxALIGN_RIGHT);
	(void) new wxButton(this, ID_DEFAULT, _T("Default"), wxPoint(160, 425), wxSize(65, 20));
	
	sgamma = new wxStaticText(this, wxID_ANY, "", wxPoint(325, 305));
	sred   = new wxStaticText(this, wxID_ANY, "", wxPoint(325, 335));
	sgreen = new wxStaticText(this, wxID_ANY, "", wxPoint(325, 365));
	sblue  = new wxStaticText(this, wxID_ANY, "", wxPoint(325, 395));
	
	(void) new wxStaticBox(this, wxID_ANY, wxT("Presets:"), wxPoint(360, 295), wxSize(208, 115));
	
	presets = new wxChoice(this, wxID_ANY, wxPoint(375, 315), wxSize(175, 20), 0, NULL, wxLB_SINGLE);
	(void) new wxButton(this, ID_APPLY, _T("Apply"), wxPoint(390, 340), wxSize(60, 20));
	(void) new wxButton(this, ID_DELETE, _T("Delete"), wxPoint(470, 340), wxSize(60, 20));
	
	presetlab = new wxTextCtrl(this, wxID_ANY, "", wxPoint(375, 380), wxSize(80, 20));
	(void) new wxButton(this, ID_NEWP, _T("New preset"), wxPoint(470, 380), wxSize(80, 20));
	
	PushEventHandler(wxGetApp().daprocess);
	
	m_taskBarIcon = new MyTaskBarIcon();
	if (!m_taskBarIcon->SetIcon(wxICON(gamma), wxT("MacondoBits Gamma Control")))
		wxMessageBox(wxT("Could not set taskbar icon."));
	
	m_Image = new wxImage(256, 256, true);
	m_Bitmap = new wxBitmap(256, 256, -1);
	Pattern = new wxBitmap(wxBITMAP(pattern));
}

#define BLACK_BG true

void MyDialog::Init(void)
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	pConfig->SetPath("/presets/");
	wxString preset;
	long index;
	bool success = pConfig->GetFirstGroup(preset, index);
	if (success)
	{
		presets->Append(preset);
		for (; pConfig->GetNextGroup(preset, index); )
			presets->Append(preset);
	}
	preset.Empty();
	pConfig->Read("/default/", &preset);
#ifdef GAMMA_CODE
	char lpszDriver[] = "DISPLAY";
	devcont = CreateDC(lpszDriver, NULL, NULL, NULL);
	if (devcont != NULL)
	{
		capable = GetDeviceGammaRamp(devcont, &gammaramp);
		//GetDeviceGammaRamp(devcont, &gammaorig);
		//for (int i = 0; i < 256*3; i++) gammaorig[i] = gammaramp[i];
		memcpy(&gammaorig, &gammaramp, sizeof(WORD)*768);
#else
	capable = true;
#endif
		unsigned char* image_data;
		//m_Image = new wxImage(256, 256, true);
		image_data = m_Image->GetData();
		
#ifndef BLACK_BG
		for (int i = 0; i < 256*256*3; i++) image_data[i] = (unsigned char)255;
#endif

		if (capable)
		{
			unsigned short gamma;
			int x, y, a;
			for (int i = 0; i < 256; i++)
			{
				x = i;
#ifdef GAMMA_CODE
				gamma = (unsigned int) gammaramp[i];
#else
				gamma = 256*i;
#endif
				y = 255 - gamma/256;
				//while (y>=256) y--;
				a = (x + 256*y)*3;
#ifdef BLACK_BG
				image_data[a + 0] = (unsigned char)255;
#else
				image_data[a + 1] = (unsigned char)0;
				image_data[a + 2] = (unsigned char)0;
#endif
#ifdef GAMMA_CODE
				gamma = (unsigned int) gammaramp[i+256];
#endif
				y = 255 - gamma/256;
				//while (y>=256) y--;
				a = (x + 256*y)*3;
#ifdef BLACK_BG
				image_data[a + 1] = (unsigned char)255;
#else
				image_data[a + 0] = (unsigned char)0;
				image_data[a + 2] = (unsigned char)0;
#endif
#ifdef GAMMA_CODE
				gamma = (unsigned int) gammaramp[i+512];
#endif
				y = 255 - gamma/256;
				//while (y>=256) y--;
				a = (x + 256*y)*3;
#ifndef BLACK_BG
				image_data[a + 0] = (unsigned char)0;
				image_data[a + 1] = (unsigned char)0;
#else
				image_data[a + 2] = (unsigned char)255;
#endif
			}
			double fred, fgreen, fblue, cut = 0.205;
			int cpoint = (int)round(256.0 * cut);
#ifdef GAMMA_CODE
			red   = gammaramp[cpoint + 0  ]/256;
			green = gammaramp[cpoint + 256]/256;
			blue  = gammaramp[cpoint + 512]/256;
#else
			red = green = blue = cpoint;
#endif
			if (!preset.IsEmpty())
			{
				presets->Select(presets->FindString(preset));
				wxCommandEvent *button = new wxCommandEvent();
				button->SetId(ID_APPLY);
				wxGetApp().daprocess->OnApplyPreset(*button);
			}
			if ((red == green) && (red == blue))
			{
				linked->SetValue(true);
				((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->Disable();
				((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->Disable();
				((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->Disable();/**/
				((wxSlider*)wxGetApp().dialog->FindWindow(ID_GAMMA))->Enable();
			} else
			{
				linked->SetValue(false);//
				((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->Enable();
				((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->Enable();
				((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->Enable();/**/
				((wxSlider*)wxGetApp().dialog->FindWindow(ID_GAMMA))->Disable();
			}
			
			fred   = ((double)red  ) *2.5/256+0.5;
			fgreen = ((double)green) *2.5/256+0.5;
			fblue  = ((double)blue ) *2.5/256+0.5;
				
			wxString gamlab;
			gamlab.Printf("%.2lf", (fred + fgreen + fblue)/3  );
			wxGetApp().dialog->sgamma->SetLabel(gamlab);
			gamlab.Printf("%.2lf", fred  );
			wxGetApp().dialog->sred  ->SetLabel(gamlab);
			gamlab.Printf("%.2lf", fgreen);
			wxGetApp().dialog->sgreen->SetLabel(gamlab);
			gamlab.Printf("%.2lf", fblue  );
			wxGetApp().dialog->sblue ->SetLabel(gamlab);/**/
			//red   = (int)round((256.0*(fred   - 0.5))/2.5);//
			//green = (int)round((256.0*(fgreen - 0.5))/2.5);//
			//blue  = (int)round((256.0*(fblue  - 0.5))/2.5);//
			gamm  = (red + green + blue) / 3;
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_GAMMA))->SetValue(gamm );
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_RED  ))->SetValue(red  );
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_GREEN))->SetValue(green);
			((wxSlider*)wxGetApp().dialog->FindWindow(ID_BLUE ))->SetValue(blue );/**/
		}
		else
			wxMessageBox("Your display adapter doesn't support changing the Gamma settings");
		wxBitmap* old_bitmap = m_Bitmap;//
		m_Bitmap = new wxBitmap(m_Image, -1);
		delete old_bitmap;//
#ifdef GAMMA_CODE
	}
	DeleteDC(devcont);
#endif
	if (preset.IsEmpty()) Show(true);
}

void
MyDialog::UpdateGamma(WXTYPE etype)
{
	wxString gammalab;
	
	double fred = red*2.5/256+0.5;
	gammalab.Printf("%.2f", fred);
	sred->SetLabel(gammalab);
	
	double fgreen = green*2.5/256+0.5;
	gammalab.Printf("%.2f", fgreen);
	sgreen->SetLabel(gammalab);
	
	double fblue = blue*2.5/256+0.5;
	gammalab.Printf("%.2f", fblue);
	sblue->SetLabel(gammalab);/**/
	
	unsigned char* image_data;
	image_data = m_Image->GetData();
	
#define SAFE_CLEAR true
	
#ifdef SAFE_CLEAR
	m_Image->Create(256, 256);
	image_data = m_Image->GetData();
#ifndef BLACK_BG
	for (int i = 0; i < 256*256*3; i++)
	{
		image_data[i] = (unsigned char)255;
	}
#endif
#endif
	
	int x;
#ifndef SAFE_CLEAR
	unsigned short gamma;
	int y, a;
	for (int i = 0; i < 256; i++)
	{
		x = i;
#ifdef GAMMA_CODE
		gamma = (unsigned int) gammaramp[i];
#else
		gamma = i*256;
#endif
		y = 255 - gamma/256;
		//while (y>=255) y--;
		a = (x + 256*y)*3;
#ifdef BLACK_BG
		image_data[a + 0] = (unsigned char)0;
#else
		image_data[a + 1] = (unsigned char)255;
		image_data[a + 2] = (unsigned char)255;
#endif
#ifdef GAMMA_CODE
		gamma = (unsigned int) gammaramp[i+256];
#else
		gamma = i*256;
#endif
		y = 255 - gamma/256;
		//while (y>=255) y--;
		a = (x + 256*y)*3;
#ifdef BLACK_BG
		image_data[a + 1] = (unsigned char)0;
#else
		image_data[a + 0] = (unsigned char)255;
		image_data[a + 2] = (unsigned char)255;
#endif
#ifdef GAMMA_CODE
		gamma = (unsigned int) gammaramp[i+512];
#else
		gamma = i*256;
#endif
		y = 255 - gamma/256;
		//while (y>=255) y--;
		a = (x + 256*y)*3;
#ifdef BLACK_BG
		image_data[a + 2] = (unsigned char)0;
#else
		image_data[a + 0] = (unsigned char)255;
		image_data[a + 1] = (unsigned char)255;
#endif
	}
#endif
	
	unsigned short rgamma, ggamma, bgamma;
	int ry, gy, by, ra, ga, ba;
	double base;
	for (int i = 0; i < 256; i++)
	{
		x = i;
		base = ((double)i)/256.0;
		rgamma = (short unsigned int)round(pow(base,(1.0/fred  )) * 256.0);
		ggamma = (short unsigned int)round(pow(base,(1.0/fgreen)) * 256.0);
		bgamma = (short unsigned int)round(pow(base,(1.0/fblue )) * 256.0);
		while (rgamma>255) rgamma--;
		ry = 255 - rgamma;
		ra = (x + 256*ry)*3;
		while (ggamma>255) ggamma--;
		gy = 255 - ggamma;
		ga = (x + 256*gy)*3;
		while (bgamma>255) bgamma--;
		by = 255 - bgamma;
		ba = (x + 256*by)*3;
		//printf("%d\n", by);
		
#ifndef BLACK_BG
		m_Image->SetRGB(x, ry, m_Image->GetRed(x, ry), (unsigned char)0, (unsigned char)0);
		m_Image->SetRGB(x, gy, (unsigned char)0, m_Image->GetGreen(x, gy), (unsigned char)0);
		m_Image->SetRGB(x, by, (unsigned char)0, (unsigned char)0, m_Image->GetBlue(x, by));
#else		
		m_Image->SetRGB(x, ry, (unsigned char)255, m_Image->GetGreen(x, ry), m_Image->GetBlue(x, ry));
		m_Image->SetRGB(x, gy, m_Image->GetRed(x, gy), (unsigned char)255, m_Image->GetBlue(x, gy));
		m_Image->SetRGB(x, by, m_Image->GetRed(x, by), m_Image->GetGreen(x, by), (unsigned char)255);
#endif
		
#ifdef GAMMA_CODE
		if (capable && (etype == wxEVT_SCROLL_CHANGED || etype == wxEVT_SCROLL_THUMBRELEASE))
		{
			gammaramp[i	] = rgamma*256;
			gammaramp[i+256] = ggamma*256;
			gammaramp[i+512] = bgamma*256;
			char lpszDriver[] = "DISPLAY";
			devcont = CreateDC(lpszDriver, NULL, NULL, NULL);
			SetDeviceGammaRamp(devcont, &gammaramp);
			DeleteDC(devcont);
		}/**/
#endif
	}
	
	if (m_Image->Ok())
	{
		wxBitmap* old_bitmap = m_Bitmap;//
		m_Bitmap = new wxBitmap(m_Image, -1);
		delete old_bitmap;//
	}
	RefreshRect(wxRect(20, 20, 256, 256));
	Update();
}


MyDialog::~MyDialog()
{
	delete m_taskBarIcon;
	//delete m_Image;
#ifdef GAMMA_CODE
	char lpszDriver[] = "DISPLAY";
	devcont = CreateDC(lpszDriver, NULL, NULL, NULL);
	SetDeviceGammaRamp(devcont, &gammaorig);
	DeleteDC(devcont);
#endif
}

void MyDialog::OnPaint( wxPaintEvent &event )
{
	//wxBufferedPaintDC dc( this );
	wxPaintDC dc( this );
	//PrepareDC( dc );
	//dc.DrawText( _T("PNG handler"), 30, 135 );
	if (m_Bitmap != NULL)
	{
		dc.DrawBitmap( *m_Bitmap, 20, 20 );
		dc.DrawBitmap( *Pattern, 300, 20 );
	}
}


void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
	Show(false);
}

void MyDialog::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageDialog mess(this, "MacondoBits Gamma Control\n\nhttp://www.macondobits.com\n\n(c) 2005 MacondoBits", "About", wxOK|wxICON_INFORMATION);
	mess.ShowModal();
}

void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
	Destroy();
	//Close(true);
}

void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	Show(false);
	//Destroy();
}


enum {
	PU_RESTORE = 10001,
	//PU_NEW_ICON,
	PU_EXIT//,
	//PU_CHECKMARK
};


BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
	EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
	EVT_MENU(PU_EXIT,	MyTaskBarIcon::OnMenuExit)
	//EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
	//EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
	EVT_MENU(wxID_ANY, MyTaskBarIcon::OnTaskMenuPreset)
	EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
END_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
	wxGetApp().dialog->Show(true);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
	wxGetApp().dialog->Destroy();
}

//static bool check = true;

//void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
//{
//	   check =!check;
//}
//
//void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
//{
//	   event.Check( check );
//}

// Overridables
wxMenu*
MyTaskBarIcon::CreatePopupMenu()
{
	wxMenu *menu = new wxMenu;
	wxMenuItem *mi;
	// this values should not be changed
	const int w1=3, w2=15;
	
	mi = new wxMenuItem(menu, PU_RESTORE, _T("Show Control window"), "", wxITEM_NORMAL);
	wxFont fnt = mi->GetFont();
	fnt.SetWeight(wxFONTWEIGHT_BOLD);
	mi->SetFont(fnt);
	mi->SetMarginWidth(w1);
	menu->Append(mi);
	mi = new wxMenuItem(menu, wxID_SEPARATOR, "", "", wxITEM_SEPARATOR);
	mi->SetMarginWidth(w2);
	menu->Append(mi);
	//menu->AppendSeparator();
	
	wxConfigBase *pConfig = wxConfigBase::Get();
	pConfig->SetPath("/presets/");
	wxString preset, def;
	pConfig->Read("/default/", &def);
	long index;
	bool success = pConfig->GetFirstGroup(preset, index);
	if (success)
	{
		int idnum = 5000;
		mi = new wxMenuItem(menu, idnum++, preset, "", wxITEM_CHECK);
		mi->SetMarginWidth(w2);
		menu->Append(mi);
		mi->Check(preset == def);
		for (; pConfig->GetNextGroup(preset, index); idnum++)
		{
			mi = new wxMenuItem(menu, idnum, preset, "", wxITEM_CHECK);
			mi->SetMarginWidth(w2);
			menu->Append(mi);
			mi->Check(preset == def);
		}
	}
	menu->AppendSeparator();
	mi = new wxMenuItem(menu, PU_EXIT, _T("E&xit"), "", wxITEM_NORMAL);
	mi->SetMarginWidth(w1);
	menu->Append(mi);
	
	return menu;
}

void
MyTaskBarIcon::OnTaskMenuPreset(wxCommandEvent& event)
{
	wxMenu* menu = (wxMenu*)event.GetEventObject();
	//wxMessageBox(menu->GetLabel(event.GetId()));
	wxGetApp().dialog->presets->Select(wxGetApp().dialog->presets->FindString(menu->GetLabel(event.GetId())));
	wxCommandEvent *button = new wxCommandEvent();
	button->SetId(ID_APPLY);
	wxGetApp().daprocess->OnApplyPreset(*button);
}

void
MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	wxGetApp().dialog->Show(true);
}
