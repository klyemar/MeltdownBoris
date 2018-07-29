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
            this.button1 = new System.Windows.Forms.Button();
            this.buttonPattern = new System.Windows.Forms.Button();
            this.groupBoxHue.SuspendLayout();
            this.groupBoxSerial.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarFps)).BeginInit();
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
            this.groupBoxSerial.Location = new System.Drawing.Point(9, 446);
            this.groupBoxSerial.Margin = new System.Windows.Forms.Padding(2);
            this.groupBoxSerial.Name = "groupBoxSerial";
            this.groupBoxSerial.Padding = new System.Windows.Forms.Padding(2);
            this.groupBoxSerial.Size = new System.Drawing.Size(352, 66);
            this.groupBoxSerial.TabIndex = 9;
            this.groupBoxSerial.TabStop = false;
            this.groupBoxSerial.Text = "Serial Connection";
            // 
            // trackBarFps
            // 
            this.trackBarFps.LargeChange = 256;
            this.trackBarFps.Location = new System.Drawing.Point(11, 127);
            this.trackBarFps.Maximum = 1023;
            this.trackBarFps.Name = "trackBarFps";
            this.trackBarFps.Size = new System.Drawing.Size(352, 45);
            this.trackBarFps.SmallChange = 32;
            this.trackBarFps.TabIndex = 10;
            this.trackBarFps.TickFrequency = 32;
            this.trackBarFps.ValueChanged += new System.EventHandler(this.TrackBarHueValueChanged);
            // 
            // labelFps
            // 
            this.labelFps.AutoSize = true;
            this.labelFps.Location = new System.Drawing.Point(23, 111);
            this.labelFps.Name = "labelFps";
            this.labelFps.Size = new System.Drawing.Size(27, 13);
            this.labelFps.TabIndex = 11;
            this.labelFps.Text = "FPS";
            // 
            // checkBoxInvert
            // 
            this.checkBoxInvert.AutoSize = true;
            this.checkBoxInvert.Location = new System.Drawing.Point(26, 170);
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
            this.checkBoxPause.Location = new System.Drawing.Point(26, 193);
            this.checkBoxPause.Name = "checkBoxPause";
            this.checkBoxPause.Size = new System.Drawing.Size(56, 17);
            this.checkBoxPause.TabIndex = 13;
            this.checkBoxPause.Text = "Pause";
            this.checkBoxPause.UseVisualStyleBackColor = true;
            this.checkBoxPause.CheckedChanged += new System.EventHandler(this.PauseCheckboxClicked);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(0, 0);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 14;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // buttonPattern
            // 
            this.buttonPattern.Location = new System.Drawing.Point(15, 404);
            this.buttonPattern.Margin = new System.Windows.Forms.Padding(2);
            this.buttonPattern.Name = "buttonPattern";
            this.buttonPattern.Size = new System.Drawing.Size(102, 28);
            this.buttonPattern.TabIndex = 2;
            this.buttonPattern.Text = "Change Pattern";
            this.buttonPattern.UseVisualStyleBackColor = true;
            this.buttonPattern.Click += new System.EventHandler(this.PatternButtonClicked);
            // 
            // ArduinoForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(380, 523);
            this.Controls.Add(this.buttonPattern);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.checkBoxPause);
            this.Controls.Add(this.checkBoxInvert);
            this.Controls.Add(this.labelFps);
            this.Controls.Add(this.trackBarFps);
            this.Controls.Add(this.groupBoxSerial);
            this.Controls.Add(this.groupBoxHue);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "ArduinoForm";
            this.Text = "Computer to Arduino";
            this.groupBoxHue.ResumeLayout(false);
            this.groupBoxHue.PerformLayout();
            this.groupBoxSerial.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trackBarFps)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

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
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button buttonPattern;
    }
}

