namespace RobotDashboard
{
   partial class ConnectForm
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ConnectForm));
         this.tbSSID = new System.Windows.Forms.TextBox();
         this.tbPswrd = new System.Windows.Forms.TextBox();
         this.btnConnect = new System.Windows.Forms.Button();
         this.lbSSID = new System.Windows.Forms.Label();
         this.lbPswrd = new System.Windows.Forms.Label();
         this.lbWaitInfo = new System.Windows.Forms.Label();
         this.lbTime = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // tbSSID
         // 
         this.tbSSID.Font = new System.Drawing.Font("Segoe UI", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.tbSSID.ForeColor = System.Drawing.Color.Black;
         this.tbSSID.Location = new System.Drawing.Point(16, 35);
         this.tbSSID.Name = "tbSSID";
         this.tbSSID.Size = new System.Drawing.Size(176, 27);
         this.tbSSID.TabIndex = 1;
         this.tbSSID.TextChanged += new System.EventHandler(this.TbSSID_TextChanged);
         // 
         // tbPswrd
         // 
         this.tbPswrd.Font = new System.Drawing.Font("Segoe UI", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.tbPswrd.ForeColor = System.Drawing.Color.Black;
         this.tbPswrd.Location = new System.Drawing.Point(16, 102);
         this.tbPswrd.Name = "tbPswrd";
         this.tbPswrd.PasswordChar = '*';
         this.tbPswrd.Size = new System.Drawing.Size(176, 27);
         this.tbPswrd.TabIndex = 2;
         this.tbPswrd.TextChanged += new System.EventHandler(this.TbPswrd_TextChanged);
         // 
         // btnConnect
         // 
         this.btnConnect.BackColor = System.Drawing.Color.Gainsboro;
         this.btnConnect.Enabled = false;
         this.btnConnect.ForeColor = System.Drawing.Color.Black;
         this.btnConnect.Location = new System.Drawing.Point(16, 146);
         this.btnConnect.Name = "btnConnect";
         this.btnConnect.Size = new System.Drawing.Size(75, 31);
         this.btnConnect.TabIndex = 3;
         this.btnConnect.Text = "Connect";
         this.btnConnect.UseVisualStyleBackColor = false;
         this.btnConnect.Click += new System.EventHandler(this.BtnConnect_Click);
         // 
         // lbSSID
         // 
         this.lbSSID.AutoSize = true;
         this.lbSSID.Font = new System.Drawing.Font("Segoe UI", 12.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.lbSSID.ForeColor = System.Drawing.Color.White;
         this.lbSSID.Location = new System.Drawing.Point(12, 9);
         this.lbSSID.Name = "lbSSID";
         this.lbSSID.Size = new System.Drawing.Size(122, 23);
         this.lbSSID.TabIndex = 6;
         this.lbSSID.Text = "Network name";
         // 
         // lbPswrd
         // 
         this.lbPswrd.AutoSize = true;
         this.lbPswrd.Font = new System.Drawing.Font("Segoe UI", 12.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.lbPswrd.ForeColor = System.Drawing.Color.White;
         this.lbPswrd.Location = new System.Drawing.Point(12, 76);
         this.lbPswrd.Name = "lbPswrd";
         this.lbPswrd.Size = new System.Drawing.Size(80, 23);
         this.lbPswrd.TabIndex = 7;
         this.lbPswrd.Text = "Password";
         // 
         // lbWaitInfo
         // 
         this.lbWaitInfo.AutoSize = true;
         this.lbWaitInfo.Font = new System.Drawing.Font("Segoe UI Semibold", 26F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))));
         this.lbWaitInfo.ForeColor = System.Drawing.Color.White;
         this.lbWaitInfo.Location = new System.Drawing.Point(8, 5);
         this.lbWaitInfo.Name = "lbWaitInfo";
         this.lbWaitInfo.Size = new System.Drawing.Size(292, 94);
         this.lbWaitInfo.TabIndex = 8;
         this.lbWaitInfo.Text = "Trying to connect\r\nto robot ...";
         this.lbWaitInfo.Visible = false;
         // 
         // lbTime
         // 
         this.lbTime.AutoSize = true;
         this.lbTime.Font = new System.Drawing.Font("Segoe UI", 60F);
         this.lbTime.ForeColor = System.Drawing.Color.White;
         this.lbTime.Location = new System.Drawing.Point(189, 71);
         this.lbTime.Name = "lbTime";
         this.lbTime.Size = new System.Drawing.Size(88, 106);
         this.lbTime.TabIndex = 9;
         this.lbTime.Text = "0";
         this.lbTime.Visible = false;
         // 
         // ConnectForm
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 21F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.BackColor = System.Drawing.Color.DeepSkyBlue;
         this.ClientSize = new System.Drawing.Size(313, 191);
         this.Controls.Add(this.lbTime);
         this.Controls.Add(this.lbWaitInfo);
         this.Controls.Add(this.lbPswrd);
         this.Controls.Add(this.lbSSID);
         this.Controls.Add(this.btnConnect);
         this.Controls.Add(this.tbPswrd);
         this.Controls.Add(this.tbSSID);
         this.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
         this.Name = "ConnectForm";
         this.Text = "Connect";
         this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ConnectForm_FormClosing);
         this.Load += new System.EventHandler(this.ConnectForm_Load);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

        #endregion
        private System.Windows.Forms.TextBox tbSSID;
        private System.Windows.Forms.TextBox tbPswrd;
        private System.Windows.Forms.Button btnConnect;
      private System.Windows.Forms.Label lbSSID;
      private System.Windows.Forms.Label lbPswrd;
      private System.Windows.Forms.Label lbWaitInfo;
      private System.Windows.Forms.Label lbTime;
   }
}