namespace RobotDashboard
{
   partial class Dashboard
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Dashboard));
         this.lbSpeed = new System.Windows.Forms.Label();
         this.lbDistance = new System.Windows.Forms.Label();
         this.lbRobot = new System.Windows.Forms.Label();
         this.gbStatus = new System.Windows.Forms.GroupBox();
         this.chartSpeed = new LiveCharts.WinForms.CartesianChart();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnReconnect = new System.Windows.Forms.Button();
         this.gbStatus.SuspendLayout();
         this.groupBox1.SuspendLayout();
         this.SuspendLayout();
         // 
         // lbSpeed
         // 
         this.lbSpeed.AutoSize = true;
         this.lbSpeed.Font = new System.Drawing.Font("Verdana", 16F, System.Drawing.FontStyle.Italic);
         this.lbSpeed.ForeColor = System.Drawing.Color.White;
         this.lbSpeed.Location = new System.Drawing.Point(6, 36);
         this.lbSpeed.Name = "lbSpeed";
         this.lbSpeed.Size = new System.Drawing.Size(200, 26);
         this.lbSpeed.TabIndex = 0;
         this.lbSpeed.Text = "Current speed: 0";
         // 
         // lbDistance
         // 
         this.lbDistance.AutoSize = true;
         this.lbDistance.Font = new System.Drawing.Font("Verdana", 16F, System.Drawing.FontStyle.Italic);
         this.lbDistance.ForeColor = System.Drawing.Color.White;
         this.lbDistance.Location = new System.Drawing.Point(6, 62);
         this.lbDistance.Name = "lbDistance";
         this.lbDistance.Size = new System.Drawing.Size(356, 26);
         this.lbDistance.TabIndex = 1;
         this.lbDistance.Text = "Distance to nearest obstacle: 0";
         // 
         // lbRobot
         // 
         this.lbRobot.AutoSize = true;
         this.lbRobot.BackColor = System.Drawing.Color.Transparent;
         this.lbRobot.Font = new System.Drawing.Font("Verdana", 50F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))));
         this.lbRobot.ForeColor = System.Drawing.Color.White;
         this.lbRobot.Location = new System.Drawing.Point(-1, 123);
         this.lbRobot.Name = "lbRobot";
         this.lbRobot.Size = new System.Drawing.Size(441, 160);
         this.lbRobot.TabIndex = 2;
         this.lbRobot.Text = "Robot \r\nDashboard";
         // 
         // gbStatus
         // 
         this.gbStatus.Controls.Add(this.lbDistance);
         this.gbStatus.Controls.Add(this.lbSpeed);
         this.gbStatus.Font = new System.Drawing.Font("Segoe UI Semibold", 18F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))));
         this.gbStatus.ForeColor = System.Drawing.Color.White;
         this.gbStatus.Location = new System.Drawing.Point(12, 333);
         this.gbStatus.Name = "gbStatus";
         this.gbStatus.Size = new System.Drawing.Size(428, 105);
         this.gbStatus.TabIndex = 5;
         this.gbStatus.TabStop = false;
         this.gbStatus.Text = "Status";
         // 
         // chartSpeed
         // 
         this.chartSpeed.BackColor = System.Drawing.Color.White;
         this.chartSpeed.Location = new System.Drawing.Point(6, 46);
         this.chartSpeed.Name = "chartSpeed";
         this.chartSpeed.Size = new System.Drawing.Size(593, 338);
         this.chartSpeed.TabIndex = 6;
         this.chartSpeed.Text = "cartesianChart1";
         // 
         // groupBox1
         // 
         this.groupBox1.Controls.Add(this.chartSpeed);
         this.groupBox1.Font = new System.Drawing.Font("Segoe UI Semibold", 22F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))));
         this.groupBox1.ForeColor = System.Drawing.Color.White;
         this.groupBox1.Location = new System.Drawing.Point(446, 48);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(605, 390);
         this.groupBox1.TabIndex = 6;
         this.groupBox1.TabStop = false;
         this.groupBox1.Text = "Chart of average speed";
         // 
         // btnReconnect
         // 
         this.btnReconnect.BackColor = System.Drawing.Color.CornflowerBlue;
         this.btnReconnect.ForeColor = System.Drawing.Color.White;
         this.btnReconnect.Location = new System.Drawing.Point(961, 12);
         this.btnReconnect.Name = "btnReconnect";
         this.btnReconnect.Size = new System.Drawing.Size(90, 41);
         this.btnReconnect.TabIndex = 7;
         this.btnReconnect.Text = "Reconnect";
         this.btnReconnect.UseVisualStyleBackColor = false;
         this.btnReconnect.Click += new System.EventHandler(this.BtnReconnect_Click);
         // 
         // Dashboard
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 21F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.BackColor = System.Drawing.Color.DodgerBlue;
         this.ClientSize = new System.Drawing.Size(1063, 448);
         this.Controls.Add(this.btnReconnect);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.gbStatus);
         this.Controls.Add(this.lbRobot);
         this.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
         this.Name = "Dashboard";
         this.Text = "Dashboard";
         this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Dashboard_FormClosing);
         this.Load += new System.EventHandler(this.Dashboard_Load);
         this.gbStatus.ResumeLayout(false);
         this.gbStatus.PerformLayout();
         this.groupBox1.ResumeLayout(false);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label lbSpeed;
      private System.Windows.Forms.Label lbDistance;
      private System.Windows.Forms.Label lbRobot;
      private System.Windows.Forms.GroupBox gbStatus;
        private LiveCharts.WinForms.CartesianChart chartSpeed;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnReconnect;
    }
}

