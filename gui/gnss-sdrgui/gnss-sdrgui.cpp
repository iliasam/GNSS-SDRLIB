#include "../../src/sdr.h"
using namespace gnsssdrgui;
using namespace System::Globalization;


/* set sdr initialize struct function */
void setsdrini(bool bsat, int sat, int sys, int ftype, bool L1, bool sbas, bool lex, sdrini_t *ini);
double str2double(String^value, char split);



int satCount = 0;

//maindlg^ form_internal = nullptr;


[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false); 

    Application::Run(gcnew maindlg());
    return 0;
}

int SDR::get_sat_count(System::Void)
{
	return satCount;
}

void SDR::fill_sat_info(int index, System::Object^ obj)
{
	if (index < 0)
		return;
	if (index >= satCount)
		return;

	sat_state_class^form = (sat_state_class^)obj;

	sdrch_t *sat_info_p = &sdrch[index];

	form->lbl_Sat->Text = gcnew String("Sat: ") + gcnew String(sat_info_p->satstr);

	if (!sdrch[index].flagacq)
	{
		//acq running

	}
}

int SDR::get_sat_info(int index, char * info)
{
	if (sdrch[index].no == 0)
		return -1;

	info += sprintf(info, "SAT: %s", sdrch[index].satstr);

	if (!sdrch[index].flagacq)
	{
		info += sprintf(info, " Acquisition ");
		info += sprintf(info, "Peak: %3.1f", sdrch[index].acq.peakr_max_fin);
	}
	else
	{
		//Tracking

		info += sprintf(info, " Tracking ");
		info += sprintf(info, "SNR: %4.1f ", sdrch[index].trk.S[0]);
		double summ_value = sdrch[index].trk.Isum_fin / 1000.0;
		info += sprintf(info, "I-Summ: %.1f K ", summ_value);

		if ((summ_value < TRACK_LOST_SUMM) && (summ_value > 0.0))
		{
			//Tracking lost
			info += sprintf(info, " LOST! ");
		}
		else
		{
			//Tracking stable
			double err_hz = sdrch[index].trk.carrfreq - sdrch[index].f_if - sdrch[index].foffset;
			info += sprintf(info, "Freq.: %i Hz ", (int)err_hz);

			if (sdrch[index].nav.flagtow)
			{
				info += sprintf(info, "Week=%i ", sdrch[index].nav.sdreph.week_gpst);
			}
			else if (sdrch[index].nav.flagsyncf)
			{
				info += sprintf(info, "Preample found ");
			}

			//info += sprintf(info, "T: %f ", sdrch[index].debugT);
			//info += sprintf(info, "T=%lu ", (uint64_t)sdrch[index].nav.firstsf);

			if (sdrch[index].prn > 100)
			{
				double err_hz_ppm = err_hz / sdrch[index].f_cf * 1e6;
				info += sprintf(info, "F. err.: %.02f ppm ", err_hz_ppm);
			}
			else
			{
				//Count bits
				uint8_t eph_state = sdrch[index].nav.sdreph.received_mask;
				uint8_t eph_count = 0;
				for (uint8_t i = 0; i < 5; i++)
				{
					if (eph_state & 0x1)
						eph_count++;
					eph_state = eph_state >> 1;
				}
				info += sprintf(info, "Eph. count=%i ", eph_count);
			}

			int track_time_s = sdrch[index].trk.track_cnt / 1000;//i'm not sure if it is correct for non GPS
			info += sprintf(info, "Trk. time=%i s ", track_time_s);
			
		}
	}
	
	info += sprintf(info, "\n");
	return 1;
}

void gui_debug_print(char* text)
{
	//form->mprintf(strstr);
}

/* sdr start function */
System::Void SDR::start(System::Object^ obj)
{
    int i=0;
    sdrini_t sdrini={0};
    maindlg^form=(maindlg^)obj;
    char *str,split;
    struct lconv *lc;
    
    setlocale( LC_ALL, ".ACP" ); //?????
    lc=localeconv();
    split=lc->decimal_point[0];

    /* reciver setting */
    str=(char*)(void*)Marshal::StringToHGlobalAnsi(form->tb_input1->Text);
    memcpy(sdrini.file1,str,strlen(str));
    str=(char*)(void*)Marshal::StringToHGlobalAnsi(form->tb_input2->Text);
    memcpy(sdrini.file2,str,strlen(str));
    sdrini.fend=form->cmb_input->SelectedIndex + 1;
    sdrini.f_sf[0]=str2double(form->tb_f1sf->Text,split)*1e6;
    sdrini.f_sf[1]=str2double(form->tb_f2sf->Text,split)*1e6;
    sdrini.f_if[0]=str2double(form->tb_f1if->Text,split)*1e6;
    sdrini.f_if[1]=str2double(form->tb_f2if->Text,split)*1e6;
    sdrini.useif1=1;
    sdrini.useif2=(int)form->chk_input2->Checked;
    sdrini.dtype[0]=(int)form->rb_f1IQ->Checked+1;
    sdrini.dtype[1]=(int)form->rb_f2IQ->Checked+1;

    if (form->cb_cf1->SelectedIndex>=0) {
        array<String^,1>^str1=form->cb_cf1->Items[form->cb_cf1->SelectedIndex]->ToString()->Split(' ',0);
        sdrini.f_cf[0]=str2double(str1[0],split)*1e6;
    } else {
        sdrini.f_cf[0]=0.0;
    }

    if (form->cb_cf2->SelectedIndex>=0) {
        array<String^,1>^str2=form->cb_cf2->Items[form->cb_cf2->SelectedIndex]->ToString()->Split(' ',0);
        sdrini.f_cf[1]=str2double(str2[0],split)*1e6;
    } else {
        sdrini.f_cf[1]=0.0;
    }
    sdrini.rtlsdrppmerr=Convert::ToInt32(form->tb_clk->Text);
	sdrini.acq_threshold = Convert::ToDouble(form->nud_acq_threshold->Value);

    /* tracking setting */
    sdrini.trkcorrn=Convert::ToInt32(form->config->tb_corrn);
    sdrini.trkcorrd=Convert::ToInt32(form->config->tb_corrd);
    sdrini.trkcorrp=Convert::ToInt32(form->config->tb_corrp);
    sdrini.trkdllb[0]=str2double(form->config->tb_dll1,split);
    sdrini.trkdllb[1]=str2double(form->config->tb_dll2,split);
    sdrini.trkpllb[0]=str2double(form->config->tb_pll1,split);
    sdrini.trkpllb[1]=str2double(form->config->tb_pll2,split);
    sdrini.trkfllb[0]=str2double(form->config->tb_fll1,split);
    sdrini.trkfllb[1]=str2double(form->config->tb_fll2,split);

    /* channel setting */ /* GPS */
	
    setsdrini(form->chk_G01->Checked, 1,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G02->Checked, 2,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G03->Checked, 3,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G04->Checked, 4,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G05->Checked, 5,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G06->Checked, 6,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G07->Checked, 7,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G08->Checked, 8,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G09->Checked, 9,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G10->Checked,10,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G11->Checked,11,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G12->Checked,12,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G13->Checked,13,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G14->Checked,14,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G15->Checked,15,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G16->Checked,16,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G17->Checked,17,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G18->Checked,18,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G19->Checked,19,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G20->Checked,20,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G21->Checked,21,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G22->Checked,22,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G23->Checked,23,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G24->Checked,24,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G25->Checked,25,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G26->Checked,26,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G27->Checked,27,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G28->Checked,28,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G29->Checked,29,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G30->Checked,30,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G31->Checked,31,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
    setsdrini(form->chk_G32->Checked,32,SYS_GPS,form->rb_G_FE2->Checked,form->chk_TYPE_L1CA->Checked,false,false,&sdrini);
	

    /* channel setting */ /* GLONASS */
    setsdrini(form->chk_R_7->Checked,-7,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R_6->Checked,-6,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R_5->Checked,-5,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R_4->Checked,-4,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R_3->Checked,-3,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R_2->Checked,-2,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R_1->Checked,-1,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R0->Checked, 0,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R1->Checked, 1,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R2->Checked, 2,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R3->Checked, 3,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R4->Checked, 4,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R5->Checked, 5,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);
    setsdrini(form->chk_R6->Checked, 6,SYS_GLO,form->rb_R_FE2->Checked,form->chk_TYPE_G1->Checked,false,false,&sdrini);

    /* channel setting */ /* GAL */
	for each (Object^ var_obj in form->gb_gal->Controls)
	{
		if (var_obj->GetType() == System::Windows::Forms::CheckBox::typeid)
		{
			System::Windows::Forms::CheckBox^ chkObj = (System::Windows::Forms::CheckBox^)var_obj;
			if (chkObj->Name->Contains("chk_E") && !chkObj->Name->Contains("chk_EALL"))
			{
				String^ chk_name = chkObj->Name;
				chk_name = chk_name->Remove(0, 5);
				int prn_num = Convert::ToInt16(chk_name);
				setsdrini(chkObj->Checked, prn_num, SYS_GAL, 
					form->rb_E_FE2->Checked, form->chk_TYPE_E1B->Checked, false, false, &sdrini);
			}
		}
	}

    /* channel setting */ /* BeiDou */
    setsdrini(form->chk_C01->Checked, 1,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C02->Checked, 2,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C03->Checked, 3,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C04->Checked, 4,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C05->Checked, 5,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C06->Checked, 6,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C07->Checked, 7,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C08->Checked, 8,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C09->Checked, 9,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C10->Checked,10,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C11->Checked,11,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C12->Checked,12,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C13->Checked,13,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);
    setsdrini(form->chk_C14->Checked,14,SYS_CMP,form->rb_C_FE2->Checked,form->chk_TYPE_B1I->Checked,false,false,&sdrini);

    /* channel setting */ /* QZS */
    setsdrini(form->chk_Q01->Checked,193,SYS_QZS,form->rb_Q_FE2->Checked,form->chk_TYPE_L1CAQZS->Checked,false,false,&sdrini);
    setsdrini(form->chk_Q01->Checked,183,SYS_QZS,form->rb_Q_FE2->Checked,false,form->chk_TYPE_SAIF->Checked,false,&sdrini);
    setsdrini(form->chk_Q01->Checked,193,SYS_QZS,form->rb_Q_FE2->Checked,false,false,form->chk_TYPE_LEX->Checked,&sdrini);

    /* channel setting */ /* SBAS */
    setsdrini(form->chk_S120->Checked,120,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S121->Checked,121,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S122->Checked,122,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S123->Checked,123,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S124->Checked,124,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S125->Checked,125,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S126->Checked,126,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S127->Checked,127,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S128->Checked,128,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S129->Checked,129,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S130->Checked,130,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S131->Checked,131,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S132->Checked,132,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S133->Checked,133,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S134->Checked,134,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S135->Checked,135,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S136->Checked,136,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S137->Checked,137,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);
    setsdrini(form->chk_S138->Checked,138,SYS_SBS,form->rb_S_FE2->Checked,true,form->chk_TYPE_SBASL1->Checked,false,&sdrini);

    /* output setting */
    sdrini.pltacq=(int)form->chk_plotacq->Checked;
    sdrini.plttrk=(int)form->chk_plottrk->Checked;

    /* output setting */
    switch(form->cmb_outint->SelectedIndex) 
	{
		case 0: sdrini.outms=1000; break;
		case 1: sdrini.outms=200; break;
		case 2: sdrini.outms=100; break;
		default:
			sdrini.outms = 1000;
    }
    sdrini.rinex=(int)form->chk_rinex->Checked;
    sdrini.rtcm=(int)form->chk_rtcm->Checked;
    sdrini.lex=(int)form->chk_lex->Checked;
    sdrini.sbas=(int)form->chk_saif->Checked;
    sdrini.log=(int)form->chk_log->Checked;
	sdrini.use_restore_acq = (int)form->chk_RestoreAcq->Checked;
	sdrini.dispay_track_cycles = (int)form->chk_DispTrackCycles->Checked;
    sprintf(sdrini.rinexpath,"%s",form->tb_rinex_dir->Text);
    if (form->tb_rtcm_port->Text!="") sdrini.rtcmport=Convert::ToInt32(form->tb_rtcm_port->Text);
    if (form->tb_lex_port->Text!="") sdrini.lexport=Convert::ToInt32(form->tb_lex_port->Text);
    if (form->tb_saif_port->Text!="") sdrini.sbasport=Convert::ToInt32(form->tb_saif_port->Text);

	satCount = sdrini.nch;

    /* sdr initialization */
    initsdrgui(form,&sdrini);

    /* create sdr thread*/
    hmainthread=(HANDLE)_beginthread(startsdr,0,NULL);
}



/* set sdr initialize struct function */
void setsdrini(bool bsat, int prn, int sys, int ftype, bool L1, bool sbas, bool lex, sdrini_t *ini)
{
    if (!bsat) return;
    if (sys==SYS_GPS) {
        if (L1) {
            ini->prn[ini->nch]=prn;
            ini->sys[ini->nch]=sys;
            ini->ctype[ini->nch]=CTYPE_L1CA;
            ini->ftype[ini->nch]=ftype+1;
            ini->nch++;
        }
    }
    if (sys==SYS_GLO) {
        if (L1) {
            ini->prn[ini->nch]=prn;
            ini->sys[ini->nch]=sys;
            ini->ctype[ini->nch]=CTYPE_G1;
            ini->ftype[ini->nch]=ftype+1;
            ini->nch++;
        }
    }
    if (sys==SYS_GAL) {
        if (L1) {
            ini->prn[ini->nch]=prn;
            ini->sys[ini->nch]=sys;
            ini->ctype[ini->nch]=CTYPE_E1B;
            ini->ftype[ini->nch]=ftype+1;
            ini->nch++;
        }
    }
    if (sys==SYS_CMP) {
        if (L1) {
            ini->prn[ini->nch]=prn;
            ini->sys[ini->nch]=sys;
            ini->ctype[ini->nch]=CTYPE_B1I;
            ini->ftype[ini->nch]=ftype+1;
            ini->nch++;
        }
    }
    if (sys==SYS_QZS) {
        if (L1) {
            ini->prn[ini->nch]=prn;
            ini->sys[ini->nch]=sys;
            ini->ctype[ini->nch]=CTYPE_L1CA;
            ini->ftype[ini->nch]=ftype+1;
            ini->nch++;
        }
        if (sbas) { /* L1-SAIF */
            ini->prn[ini->nch]=prn;
            ini->sys[ini->nch]=sys;
            ini->ctype[ini->nch]=CTYPE_L1SAIF;
            ini->ftype[ini->nch]=ftype+1;
            ini->nch++;
        }
        if (lex) { /* LEX */
            ini->prn[ini->nch]=prn;
            ini->sys[ini->nch]=sys;
            ini->ctype[ini->nch]=CTYPE_LEXS;
            ini->ftype[ini->nch]=ftype+1;
            ini->nch++;
        }
    }
    if (sys==SYS_SBS) {
        if (L1) {
            ini->prn[ini->nch]=prn;
            ini->sys[ini->nch]=sys;
            ini->ctype[ini->nch]=CTYPE_L1SBAS;
            ini->ftype[ini->nch]=ftype+1;
            ini->nch++;
        }
    }
}

double str2double(String^value, char split)
{
    return Convert::ToDouble(value->Replace('.', split));
}

/* sdr stop function */
System::Void SDR::stop(System::Object^ obj)
{
    maindlg^form=(maindlg^)obj;
    sdrstat.stopflag=ON;
}
/* sdr spectrum start */
System::Void SDR::startspectrum(System::Object^ obj)
{
    maindlg^form=(maindlg^)obj;
    sdrspec_t sdrspec={0};

    if (form->rb_spec1->Checked) {
        sdrspec.dtype=(int)form->rb_f1IQ->Checked+1;
        sdrspec.ftype=1;

		System::String ^text = form->tb_f1sf->Text;

		text = text->Replace(',', '.');
        sdrspec.f_sf=Convert::ToDouble(text, CultureInfo::InvariantCulture)*1e6;
        sdrspec.nsamp=(int)(sdrspec.f_sf/1000);
    }
    if (form->rb_spec2->Checked) {
        sdrspec.dtype=(int)form->rb_f2IQ->Checked+1;
        sdrspec.ftype=2;
        sdrspec.f_sf=Convert::ToDouble(form->tb_f2sf->Text)*1e6;
        sdrspec.nsamp=(int)(sdrspec.f_sf/1000);
    }
    sdrstat.specflag=OFF;	
    initsdrspecgui(&sdrspec);
}
/* sdr spectrum stop */
System::Void SDR::stopspectrum(System::Object^ obj)
{
    maindlg^form=(maindlg^)obj;
    sdrstat.specflag=ON;	
}

System::Void SDR::settrkprm(System::Object^ obj1,System::Object^ obj2)
{
    trkprmdlg^trk=(trkprmdlg^)obj1;
    maindlg^ main=(maindlg^)obj2;

    main->config->tb_corrn=trk->tb_corrn->Text;
    main->config->tb_corrd=trk->tb_corrd->Text;
    main->config->tb_corrp=trk->tb_corrp->Text;
    main->config->tb_dll1=trk->tb_dll1->Text;
    main->config->tb_pll1=trk->tb_pll1->Text;
    main->config->tb_fll1=trk->tb_fll1->Text;
    main->config->tb_dll2=trk->tb_dll2->Text;
    main->config->tb_pll2=trk->tb_pll2->Text;
    main->config->tb_fll2=trk->tb_fll2->Text;
}
