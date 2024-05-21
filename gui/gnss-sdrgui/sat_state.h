#pragma once
//#include "gnss-sdrgui.h"

namespace gnsssdrgui
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class sat_state_class: public System::Windows::Forms::Form
	{
	public:
		sat_state_class(void)
		{
			InitializeComponent();
		}

	protected:
		~sat_state_class()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::Label^  lbl_Sat;
	public: System::Windows::Forms::GroupBox^  groupAcq;
	public:
	public: System::Windows::Forms::GroupBox^  groupTracking;
	public: System::Windows::Forms::Label^  lblAcqPeak;
	public: System::Windows::Forms::Label^  lblAcqFreq;
	public: System::Windows::Forms::Label^  lblTrackFreq;
	public: System::Windows::Forms::Label^  lblTrackSNR;
	public: System::Windows::Forms::Label^  lblTrackISumm;
	public: System::Windows::Forms::Label^  lblTrackWeek;
	public: System::Windows::Forms::Label^  lblPreample;
	public: System::Windows::Forms::Label^  lblTrackEPH;
	private: System::Windows::Forms::Button^  btnReset;
	public:





	public:
	protected:

	protected:

	protected:

	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->lbl_Sat = (gcnew System::Windows::Forms::Label());
			this->groupAcq = (gcnew System::Windows::Forms::GroupBox());
			this->lblAcqPeak = (gcnew System::Windows::Forms::Label());
			this->groupTracking = (gcnew System::Windows::Forms::GroupBox());
			this->lblAcqFreq = (gcnew System::Windows::Forms::Label());
			this->lblTrackFreq = (gcnew System::Windows::Forms::Label());
			this->lblTrackSNR = (gcnew System::Windows::Forms::Label());
			this->lblTrackISumm = (gcnew System::Windows::Forms::Label());
			this->lblTrackWeek = (gcnew System::Windows::Forms::Label());
			this->lblPreample = (gcnew System::Windows::Forms::Label());
			this->btnReset = (gcnew System::Windows::Forms::Button());
			this->lblTrackEPH = (gcnew System::Windows::Forms::Label());
			this->groupAcq->SuspendLayout();
			this->groupTracking->SuspendLayout();
			this->SuspendLayout();
			// 
			// lbl_Sat
			// 
			this->lbl_Sat->AutoSize = true;
			this->lbl_Sat->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->lbl_Sat->Location = System::Drawing::Point(10, 9);
			this->lbl_Sat->Name = L"lbl_Sat";
			this->lbl_Sat->Size = System::Drawing::Size(64, 18);
			this->lbl_Sat->TabIndex = 0;
			this->lbl_Sat->Text = L"SAT: n/a";
			// 
			// groupAcq
			// 
			this->groupAcq->Controls->Add(this->lblAcqFreq);
			this->groupAcq->Controls->Add(this->lblAcqPeak);
			this->groupAcq->Location = System::Drawing::Point(9, 32);
			this->groupAcq->Name = L"groupAcq";
			this->groupAcq->Size = System::Drawing::Size(272, 64);
			this->groupAcq->TabIndex = 1;
			this->groupAcq->TabStop = false;
			this->groupAcq->Text = L"Acquisition";
			// 
			// lblAcqPeak
			// 
			this->lblAcqPeak->AutoSize = true;
			this->lblAcqPeak->Location = System::Drawing::Point(9, 20);
			this->lblAcqPeak->Name = L"lblAcqPeak";
			this->lblAcqPeak->Size = System::Drawing::Size(55, 13);
			this->lblAcqPeak->TabIndex = 0;
			this->lblAcqPeak->Text = L"Peak: n/a";
			// 
			// groupTracking
			// 
			this->groupTracking->Controls->Add(this->lblTrackEPH);
			this->groupTracking->Controls->Add(this->lblPreample);
			this->groupTracking->Controls->Add(this->lblTrackWeek);
			this->groupTracking->Controls->Add(this->lblTrackISumm);
			this->groupTracking->Controls->Add(this->lblTrackSNR);
			this->groupTracking->Controls->Add(this->lblTrackFreq);
			this->groupTracking->Enabled = false;
			this->groupTracking->Location = System::Drawing::Point(9, 102);
			this->groupTracking->Name = L"groupTracking";
			this->groupTracking->Size = System::Drawing::Size(272, 322);
			this->groupTracking->TabIndex = 2;
			this->groupTracking->TabStop = false;
			this->groupTracking->Text = L"Tracking";
			// 
			// lblAcqFreq
			// 
			this->lblAcqFreq->AutoSize = true;
			this->lblAcqFreq->Location = System::Drawing::Point(9, 40);
			this->lblAcqFreq->Name = L"lblAcqFreq";
			this->lblAcqFreq->Size = System::Drawing::Size(84, 13);
			this->lblAcqFreq->TabIndex = 1;
			this->lblAcqFreq->Text = L"Found freq.: n/a";
			// 
			// lblTrackFreq
			// 
			this->lblTrackFreq->AutoSize = true;
			this->lblTrackFreq->Location = System::Drawing::Point(13, 196);
			this->lblTrackFreq->Name = L"lblTrackFreq";
			this->lblTrackFreq->Size = System::Drawing::Size(80, 13);
			this->lblTrackFreq->TabIndex = 2;
			this->lblTrackFreq->Text = L"Frequency: n/a";
			// 
			// lblTrackSNR
			// 
			this->lblTrackSNR->AutoSize = true;
			this->lblTrackSNR->Location = System::Drawing::Point(13, 216);
			this->lblTrackSNR->Name = L"lblTrackSNR";
			this->lblTrackSNR->Size = System::Drawing::Size(53, 13);
			this->lblTrackSNR->TabIndex = 3;
			this->lblTrackSNR->Text = L"SNR: n/a";
			// 
			// lblTrackISumm
			// 
			this->lblTrackISumm->AutoSize = true;
			this->lblTrackISumm->Location = System::Drawing::Point(13, 235);
			this->lblTrackISumm->Name = L"lblTrackISumm";
			this->lblTrackISumm->Size = System::Drawing::Size(63, 13);
			this->lblTrackISumm->TabIndex = 4;
			this->lblTrackISumm->Text = L"I-summ: n/a";
			// 
			// lblTrackWeek
			// 
			this->lblTrackWeek->AutoSize = true;
			this->lblTrackWeek->Location = System::Drawing::Point(13, 257);
			this->lblTrackWeek->Name = L"lblTrackWeek";
			this->lblTrackWeek->Size = System::Drawing::Size(59, 13);
			this->lblTrackWeek->TabIndex = 5;
			this->lblTrackWeek->Text = L"Week: n/a";
			// 
			// lblPreample
			// 
			this->lblPreample->AutoSize = true;
			this->lblPreample->Location = System::Drawing::Point(13, 277);
			this->lblPreample->Name = L"lblPreample";
			this->lblPreample->Size = System::Drawing::Size(74, 13);
			this->lblPreample->TabIndex = 6;
			this->lblPreample->Text = L"Preamble: n/a";
			// 
			// btnReset
			// 
			this->btnReset->Location = System::Drawing::Point(12, 430);
			this->btnReset->Name = L"btnReset";
			this->btnReset->Size = System::Drawing::Size(99, 23);
			this->btnReset->TabIndex = 3;
			this->btnReset->Text = L"Reset channel";
			this->btnReset->UseVisualStyleBackColor = true;
			// 
			// lblTrackEPH
			// 
			this->lblTrackEPH->AutoSize = true;
			this->lblTrackEPH->Location = System::Drawing::Point(13, 298);
			this->lblTrackEPH->Name = L"lblTrackEPH";
			this->lblTrackEPH->Size = System::Drawing::Size(70, 13);
			this->lblTrackEPH->TabIndex = 7;
			this->lblTrackEPH->Text = L"EPH cnt: n/a";
			// 
			// sat_state_class
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(293, 461);
			this->Controls->Add(this->btnReset);
			this->Controls->Add(this->groupTracking);
			this->Controls->Add(this->groupAcq);
			this->Controls->Add(this->lbl_Sat);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"sat_state_class";
			this->ShowIcon = false;
			this->Text = L"Satellite state";
			this->groupAcq->ResumeLayout(false);
			this->groupAcq->PerformLayout();
			this->groupTracking->ResumeLayout(false);
			this->groupTracking->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}

#pragma endregion


	};//end of class
} //namespace gnsssdrgui 
