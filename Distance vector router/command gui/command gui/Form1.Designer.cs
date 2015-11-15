namespace command_gui
{
    partial class starter
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
            this.lb_processList = new System.Windows.Forms.ListBox();
            this.btn_terminate = new System.Windows.Forms.Button();
            this.tb_name = new System.Windows.Forms.TextBox();
            this.cb_poisedReverse = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btn_startFromFolder = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lb_processList
            // 
            this.lb_processList.FormattingEnabled = true;
            this.lb_processList.Location = new System.Drawing.Point(162, 163);
            this.lb_processList.Name = "lb_processList";
            this.lb_processList.Size = new System.Drawing.Size(182, 303);
            this.lb_processList.TabIndex = 0;
            // 
            // btn_terminate
            // 
            this.btn_terminate.Location = new System.Drawing.Point(205, 54);
            this.btn_terminate.Name = "btn_terminate";
            this.btn_terminate.Size = new System.Drawing.Size(139, 48);
            this.btn_terminate.TabIndex = 2;
            this.btn_terminate.Text = "Terminate";
            this.btn_terminate.UseVisualStyleBackColor = true;
            // 
            // tb_name
            // 
            this.tb_name.Location = new System.Drawing.Point(12, 370);
            this.tb_name.Name = "tb_name";
            this.tb_name.Size = new System.Drawing.Size(109, 20);
            this.tb_name.TabIndex = 3;
            // 
            // cb_poisedReverse
            // 
            this.cb_poisedReverse.AutoSize = true;
            this.cb_poisedReverse.Location = new System.Drawing.Point(12, 396);
            this.cb_poisedReverse.Name = "cb_poisedReverse";
            this.cb_poisedReverse.Size = new System.Drawing.Size(101, 17);
            this.cb_poisedReverse.TabIndex = 4;
            this.cb_poisedReverse.Text = "Poised Reverse";
            this.cb_poisedReverse.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 354);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(76, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Process Name";
            // 
            // btn_startFromFolder
            // 
            this.btn_startFromFolder.Location = new System.Drawing.Point(12, 56);
            this.btn_startFromFolder.Name = "btn_startFromFolder";
            this.btn_startFromFolder.Size = new System.Drawing.Size(139, 46);
            this.btn_startFromFolder.TabIndex = 6;
            this.btn_startFromFolder.Text = "Start From Folder";
            this.btn_startFromFolder.UseVisualStyleBackColor = true;
            this.btn_startFromFolder.Click += new System.EventHandler(this.btn_startFromFolder_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(40, 169);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 7;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // starter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(372, 494);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.btn_startFromFolder);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cb_poisedReverse);
            this.Controls.Add(this.tb_name);
            this.Controls.Add(this.btn_terminate);
            this.Controls.Add(this.lb_processList);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "starter";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox lb_processList;
        private System.Windows.Forms.Button btn_terminate;
        private System.Windows.Forms.TextBox tb_name;
        private System.Windows.Forms.CheckBox cb_poisedReverse;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btn_startFromFolder;
        private System.Windows.Forms.Button button1;
    }
}

