namespace ComputerToArduino
{
    partial class ArduinoForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonSerialConnect = new System.Windows.Forms.Button();
            this.comboBoxPorts = new System.Windows.Forms.ComboBox();
            this.checkBoxHue1 = new System.Windows.Forms.CheckBox();
            this.checkBoxHue2 = new System.Windows.Forms.CheckBox();
            this.checkBoxHue3 = new System.Windows.Forms.CheckBox();
            this.groupBoxHue = new System.Windows.Forms.GroupBox();
            this.checkBoxHue5 = new System.Windows.Forms.CheckBox();
            this.checkBoxHue4 = new System.Windows.Forms.CheckBox();
            this.groupBoxSerial = new System.Windows.Forms.GroupBox();
            this.trackBarFps = new System.Windows.Forms.TrackBar();
            this.labelFps = new System.Windows.Forms.Label();
            this.checkBoxInvert = new System.Windows.Forms.CheckBox();
            this.checkBoxPause = new System.Windows.Forms.CheckBox();
            this.groupBoxAnalog = new System.Windows.Forms.GroupBox();
            this.labelFade = new System.Windows.Forms.Label();
            this.trackBarFade = new System.Windows.Forms.TrackBar();
            this.groupBoxDigital = new System.Windows.Forms.GroupBox();
            this.buttonPattern = new System.Windows.Forms.Button();
            this.groupBoxHue.SuspendLayout();
            this.groupBoxSerial.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarFps)).BeginInit();
            this.groupBoxAnalog.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarFade)).BeginInit();
            this.groupBoxDigital.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonSerialConnect
            // 
            this.buttonSerialConnect.Location = new System.Drawing.Point(6, 25);
            this.buttonSerialConnect.Margin = new System.Windows.Forms.Padding(2);
            this.buttonSerialConnect.Name = "buttonSerialConnect";
            this.buttonSerialConnect.Size = new System.Drawing.Size(102, 28);
            this.buttonSerialConnect.TabIndex = 0;
            this.buttonSerialConnect.Text = "Connect";
            this.buttonSerialConnect.UseVisualStyleBackColor = true;
            this.buttonSerialConnect.Click += new System.EventHandler(this.ButtonConnectClick);
            // 
            // comboBoxPorts
            // 
            this.comboBoxPorts.FormattingEnabled = true;
            this.comboBoxPorts.Location = new System.Drawing.Point(112, 30);
            this.comboBoxPorts.Margin = new System.Windows.Forms.Padding(2);
            this.comboBoxPorts.Name = "comboBoxPorts";
            this.comboBoxPorts.Size = new System.Drawing.Size(71, 21);
            this.comboBoxPorts.TabIndex = 1;
            // 
            // checkBoxHue1
            // 
            this.checkBoxHue1.AutoSize = true;
            this.checkBoxHue1.Location = new System.Drawing.Point(9, 29);
            this.checkBoxHue1.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxHue1.Name = "checkBoxHue1";
            this.checkBoxHue1.Size = new System.Drawing.Size(32, 17);
            this.checkBoxHue1.TabIndex = 5;
            this.checkBoxHue1.Text = "1";
            this.checkBoxHue1.UseVisualStyleBackColor = true;
            this.checkBoxHue1.CheckedChanged += new System.EventHandler(this.Hue1CheckboxClicked);
            // 
            // checkBoxHue2
            // 
            this.checkBoxHue2.AutoSize = true;
            this.checkBoxHue2.Location = new System.Drawing.Point(45, 29);
            this.checkBoxHue2.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxHue2.Name = "checkBoxHue2";
            this.checkBoxHue2.Size = new System.Drawing.Size(32, 17);
            this.checkBoxHue2.TabIndex = 6;
            this.checkBoxHue2.Text = "2";
            this.checkBoxHue2.UseVisualStyleBackColor = true;
            this.checkBoxHue2.CheckedChanged += new System.EventHandler(this.Hue2CheckboxClicked);
            // 
            // checkBoxHue3
            // 
            this.checkBoxHue3.AutoSize = true;
            this.checkBoxHue3.Location = new System.Drawing.Point(81, 29);
            this.checkBoxHue3.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxHue3.Name = "checkBoxHue3";
            this.checkBoxHue3.Size = new System.Drawing.Size(32, 17);
            this.checkBoxHue3.TabIndex = 7;
            this.checkBoxHue3.Text = "3";
            this.checkBoxHue3.UseVisualStyleBackColor = true;
            this.checkBoxHue3.CheckedChanged += new System.EventHandler(this.Hue3CheckboxClicked);
            // 
            // groupBoxHue
            // 
            this.groupBoxHue.Controls.Add(this.checkBoxHue5);
            this.groupBoxHue.Controls.Add(this.checkBoxHue4);
            this.groupBoxHue.Controls.Add(this.checkBoxHue3);
            this.groupBoxHue.Controls.Add(this.checkBoxHue1);
            this.groupBoxHue.Controls.Add(this.checkBoxHue2);
            this.groupBoxHue.Location = new System.Drawing.Point(17, 24);
            this.groupBoxHue.Margin = new System.Windows.Forms.Padding(2);
            this.groupBoxHue.Name = "groupBoxHue";
            this.groupBoxHue.Padding = new System.Windows.Forms.Padding(2);
            this.groupBoxHue.Size = new System.Drawing.Size(344, 68);
            this.groupBoxHue.TabIndex = 8;
            this.groupBoxHue.TabStop = false;
            this.groupBoxHue.Text = "Hue Control";
            // 
            // checkBoxHue5
            // 
            this.checkBoxHue5.AutoSize = true;
            this.checkBoxHue5.Location = new System.Drawing.Point(153, 29);
            this.checkBoxHue5.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxHue5.Name = "checkBoxHue5";
            this.checkBoxHue5.Size = new System.Drawing.Size(32, 17);
            this.checkBoxHue5.TabIndex = 9;
            this.checkBoxHue5.Text = "5";
            this.checkBoxHue5.UseVisualStyleBackColor = true;
            this.checkBoxHue5.CheckedChanged += new System.EventHandler(this.Hue5CheckboxClicked);
            // 
            // checkBoxHue4
            // 
            this.checkBoxHue4.AutoSize = true;
            this.checkBoxHue4.Location = new System.Drawing.Point(117, 29);
            this.checkBoxHue4.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxHue4.Name = "checkBoxHue4";
            this.checkBoxHue4.Size = new System.Drawing.Size(32, 17);
            this.checkBoxHue4.TabIndex = 8;
            this.checkBoxHue4.Text = "4";
            this.checkBoxHue4.UseVisualStyleBackColor = true;
            this.checkBoxHue4.CheckedChanged += new System.EventHandler(this.Hue4CheckboxClicked);
            // 
            // groupBoxSerial
            // 
            this.groupBoxSerial.Controls.Add(this.comboBoxPorts);
            this.groupBoxSerial.Controls.Add(this.buttonSerialConnect);
            this.groupBoxSerial.Location = new System.Drawing.Point(17, 446);
            this.groupBoxSerial.Margin = new System.Windows.Forms.Padding(2);
            this.groupBoxSerial.Name = "groupBoxSerial";
            this.groupBoxSerial.Padding = new System.Windows.Forms.Padding(2);
            this.groupBoxSerial.Size = new System.Drawing.Size(344, 66);
            this.groupBoxSerial.TabIndex = 9;
            this.groupBoxSerial.TabStop = false;
            this.groupBoxSerial.Text = "Serial Connection";
            // 
            // trackBarFps
            // 
            this.trackBarFps.LargeChange = 256;
            this.trackBarFps.Location = new System.Drawing.Point(6, 125);
            this.trackBarFps.Maximum = 1023;
            this.trackBarFps.Name = "trackBarFps";
            this.trackBarFps.Size = new System.Drawing.Size(332, 45);
            this.trackBarFps.SmallChange = 32;
            this.trackBarFps.TabIndex = 10;
            this.trackBarFps.TickFrequency = 32;
            this.trackBarFps.ValueChanged += new System.EventHandler(this.TrackBarHueValueChanged);
            // 
            // labelFps
            // 
            this.labelFps.AutoSize = true;
            this.labelFps.Location = new System.Drawing.Point(6, 109);
            this.labelFps.Name = "labelFps";
            this.labelFps.Size = new System.Drawing.Size(27, 13);
            this.labelFps.TabIndex = 11;
            this.labelFps.Text = "FPS";
            // 
            // checkBoxInvert
            // 
            this.checkBoxInvert.AutoSize = true;
            this.checkBoxInvert.Location = new System.Drawing.Point(6, 19);
            this.checkBoxInvert.Name = "checkBoxInvert";
            this.checkBoxInvert.Size = new System.Drawing.Size(53, 17);
            this.checkBoxInvert.TabIndex = 12;
            this.checkBoxInvert.Text = "Invert";
            this.checkBoxInvert.UseVisualStyleBackColor = true;
            this.checkBoxInvert.CheckedChanged += new System.EventHandler(this.InvertCheckboxClicked);
            // 
            // checkBoxPause
            // 
            this.checkBoxPause.AutoSize = true;
            this.checkBoxPause.Location = new System.Drawing.Point(6, 42);
            this.checkBoxPause.Name = "checkBoxPause";
            this.checkBoxPause.Size = new System.Drawing.Size(56, 17);
            this.checkBoxPause.TabIndex = 13;
            this.checkBoxPause.Text = "Pause";
            this.checkBoxPause.UseVisualStyleBackColor = true;
            this.checkBoxPause.CheckedChanged += new System.EventHandler(this.PauseCheckboxClicked);
            // 
            // groupBoxAnalog
            // 
            this.groupBoxAnalog.Controls.Add(this.trackBarFps);
            this.groupBoxAnalog.Controls.Add(this.labelFade);
            this.groupBoxAnalog.Controls.Add(this.labelFps);
            this.groupBoxAnalog.Controls.Add(this.trackBarFade);
            this.groupBoxAnalog.Location = new System.Drawing.Point(17, 265);
            this.groupBoxAnalog.Name = "groupBoxAnalog";
            this.groupBoxAnalog.Size = new System.Drawing.Size(344, 176);
            this.groupBoxAnalog.TabIndex = 16;
            this.groupBoxAnalog.TabStop = false;
            this.groupBoxAnalog.Text = "Analog";
            // 
            // labelFade
            // 
            this.labelFade.AutoSize = true;
            this.labelFade.Location = new System.Drawing.Point(6, 45);
            this.labelFade.Name = "labelFade";
            this.labelFade.Size = new System.Drawing.Size(31, 13);
            this.labelFade.TabIndex = 15;
            this.labelFade.Text = "Fade";
            // 
            // trackBarFade
            // 
            this.trackBarFade.LargeChange = 256;
            this.trackBarFade.Location = new System.Drawing.Point(9, 61);
            this.trackBarFade.Maximum = 1023;
            this.trackBarFade.Name = "trackBarFade";
            this.trackBarFade.Size = new System.Drawing.Size(329, 45);
            this.trackBarFade.SmallChange = 32;
            this.trackBarFade.TabIndex = 14;
            this.trackBarFade.TickFrequency = 32;
            this.trackBarFade.ValueChanged += new System.EventHandler(this.TrackBarFadeValueChanged);
            // 
            // groupBoxDigital
            // 
            this.groupBoxDigital.Controls.Add(this.checkBoxInvert);
            this.groupBoxDigital.Controls.Add(this.checkBoxPause);
            this.groupBoxDigital.Location = new System.Drawing.Point(17, 113);
            this.groupBoxDigital.Name = "groupBoxDigital";
            this.groupBoxDigital.Size = new System.Drawing.Size(344, 67);
            this.groupBoxDigital.TabIndex = 17;
            this.groupBoxDigital.TabStop = false;
            this.groupBoxDigital.Text = "Digital";
            // 
            // buttonPattern
            // 
            this.buttonPattern.Location = new System.Drawing.Point(23, 231);
            this.buttonPattern.Name = "buttonPattern";
            this.buttonPattern.Size = new System.Drawing.Size(102, 28);
            this.buttonPattern.TabIndex = 18;
            this.buttonPattern.Text = "Change Pattern";
            this.buttonPattern.UseVisualStyleBackColor = true;
            this.buttonPattern.Click += new System.EventHandler(this.ButtonPatternClicked);
            // 
            // ArduinoForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(380, 523);
            this.Controls.Add(this.buttonPattern);
            this.Controls.Add(this.groupBoxDigital);
            this.Controls.Add(this.groupBoxAnalog);
            this.Controls.Add(this.groupBoxSerial);
            this.Controls.Add(this.groupBoxHue);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "ArduinoForm";
            this.Text = "Computer to Arduino";
            this.groupBoxHue.ResumeLayout(false);
            this.groupBoxHue.PerformLayout();
            this.groupBoxSerial.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trackBarFps)).EndInit();
            this.groupBoxAnalog.ResumeLayout(false);
            this.groupBoxAnalog.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarFade)).EndInit();
            this.groupBoxDigital.ResumeLayout(false);
            this.groupBoxDigital.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button buttonSerialConnect;
        private System.Windows.Forms.ComboBox comboBoxPorts;
        private System.Windows.Forms.CheckBox checkBoxHue1;
        private System.Windows.Forms.CheckBox checkBoxHue2;
        private System.Windows.Forms.CheckBox checkBoxHue3;
        private System.Windows.Forms.GroupBox groupBoxHue;
        private System.Windows.Forms.GroupBox groupBoxSerial;
        private System.Windows.Forms.TrackBar trackBarFps;
        private System.Windows.Forms.CheckBox checkBoxHue5;
        private System.Windows.Forms.CheckBox checkBoxHue4;
        private System.Windows.Forms.Label labelFps;
        private System.Windows.Forms.CheckBox checkBoxInvert;
        private System.Windows.Forms.CheckBox checkBoxPause;
        private System.Windows.Forms.GroupBox groupBoxAnalog;
        private System.Windows.Forms.GroupBox groupBoxDigital;
        private System.Windows.Forms.Label labelFade;
        private System.Windows.Forms.TrackBar trackBarFade;
        private System.Windows.Forms.Button buttonPattern;
    }
}

