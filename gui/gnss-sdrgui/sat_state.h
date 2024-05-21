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
			this->groupTracking = (gcnew System::Windows::Forms::GroupBox());
			this->SuspendLayout();
			// 
			// lbl_Sat
			// 
			this->lbl_Sat->AutoSize = true;
			this->lbl_Sat->Location = System::Drawing::Point(10, 9);
			this->lbl_Sat->Name = L"lbl_Sat";
			this->lbl_Sat->Size = System::Drawing::Size(51, 13);
			this->lbl_Sat->TabIndex = 0;
			this->lbl_Sat->Text = L"SAT: n/a";
			// 
			// groupAcq
			// 
			this->groupAcq->Location = System::Drawing::Point(9, 25);
			this->groupAcq->Name = L"groupAcq";
			this->groupAcq->Size = System::Drawing::Size(272, 64);
			this->groupAcq->TabIndex = 1;
			this->groupAcq->TabStop = false;
			this->groupAcq->Text = L"Acquisition";
			// 
			// groupTracking
			// 
			this->groupTracking->Enabled = false;
			this->groupTracking->Location = System::Drawing::Point(9, 95);
			this->groupTracking->Name = L"groupTracking";
			this->groupTracking->Size = System::Drawing::Size(272, 231);
			this->groupTracking->TabIndex = 2;
			this->groupTracking->TabStop = false;
			this->groupTracking->Text = L"Tracking";
			// 
			// sat_state_class
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(293, 338);
			this->Controls->Add(this->groupTracking);
			this->Controls->Add(this->groupAcq);
			this->Controls->Add(this->lbl_Sat);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"sat_state_class";
			this->ShowIcon = false;
			this->Text = L"Satellite state";
			this->ResumeLayout(false);
			this->PerformLayout();

		}

#pragma endregion


	};//end of class
} //namespace gnsssdrgui 
