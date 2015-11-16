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
            this.btn_terminateAll = new System.Windows.Forms.Button();
            this.tb_name = new System.Windows.Forms.TextBox();
            this.cb_poisedReverse = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btn_startFromFolder = new System.Windows.Forms.Button();
            this.btn_startSingle = new System.Windows.Forms.Button();
            this.lb_test = new System.Windows.Forms.Label();
            this.tb_test = new System.Windows.Forms.TextBox();
            this.btn_terminateSingle = new System.Windows.Forms.Button();
            this.btn_print = new System.Windows.Forms.Button();
            this.tb_message = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // lb_processList
            // 
            this.lb_processList.FormattingEnabled = true;
            this.lb_processList.Location = new System.Drawing.Point(205, 162);
            this.lb_processList.Name = "lb_processList";
            this.lb_processList.Size = new System.Drawing.Size(139, 303);
            this.lb_processList.TabIndex = 0;
            // 
            // btn_terminateAll
            // 
            this.btn_terminateAll.Location = new System.Drawing.Point(205, 58);
            this.btn_terminateAll.Name = "btn_terminateAll";
            this.btn_terminateAll.Size = new System.Drawing.Size(139, 46);
            this.btn_terminateAll.TabIndex = 2;
            this.btn_terminateAll.Text = "Terminate all";
            this.btn_terminateAll.UseVisualStyleBackColor = true;
            this.btn_terminateAll.Click += new System.EventHandler(this.btn_terminateAll_Click);
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
            this.cb_poisedReverse.Location = new System.Drawing.Point(12, 309);
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
            this.btn_startFromFolder.Location = new System.Drawing.Point(12, 58);
            this.btn_startFromFolder.Name = "btn_startFromFolder";
            this.btn_startFromFolder.Size = new System.Drawing.Size(139, 46);
            this.btn_startFromFolder.TabIndex = 6;
            this.btn_startFromFolder.Text = "Start From Folder";
            this.btn_startFromFolder.UseVisualStyleBackColor = true;
            this.btn_startFromFolder.Click += new System.EventHandler(this.btn_startFromFolder_Click);
            // 
            // btn_startSingle
            // 
            this.btn_startSingle.Location = new System.Drawing.Point(12, 110);
            this.btn_startSingle.Name = "btn_startSingle";
            this.btn_startSingle.Size = new System.Drawing.Size(139, 46);
            this.btn_startSingle.TabIndex = 7;
            this.btn_startSingle.Text = "Start Single Process";
            this.btn_startSingle.UseVisualStyleBackColor = true;
            this.btn_startSingle.Click += new System.EventHandler(this.btn_startSingle_Click);
            // 
            // lb_test
            // 
            this.lb_test.AutoSize = true;
            this.lb_test.Location = new System.Drawing.Point(9, 429);
            this.lb_test.Name = "lb_test";
            this.lb_test.Size = new System.Drawing.Size(57, 13);
            this.lb_test.TabIndex = 8;
            this.lb_test.Text = "Test folder";
            // 
            // tb_test
            // 
            this.tb_test.Location = new System.Drawing.Point(12, 445);
            this.tb_test.Name = "tb_test";
            this.tb_test.Size = new System.Drawing.Size(109, 20);
            this.tb_test.TabIndex = 9;
            // 
            // btn_terminateSingle
            // 
            this.btn_terminateSingle.Location = new System.Drawing.Point(205, 110);
            this.btn_terminateSingle.Name = "btn_terminateSingle";
            this.btn_terminateSingle.Size = new System.Drawing.Size(139, 46);
            this.btn_terminateSingle.TabIndex = 10;
            this.btn_terminateSingle.Text = "Terminate single";
            this.btn_terminateSingle.UseVisualStyleBackColor = true;
            this.btn_terminateSingle.Click += new System.EventHandler(this.btn_terminateSingle_Click);
            // 
            // btn_print
            // 
            this.btn_print.Location = new System.Drawing.Point(12, 162);
            this.btn_print.Name = "btn_print";
            this.btn_print.Size = new System.Drawing.Size(139, 44);
            this.btn_print.TabIndex = 11;
            this.btn_print.Text = "Send Message to Selected Router";
            this.btn_print.UseVisualStyleBackColor = true;
            this.btn_print.Click += new System.EventHandler(this.btn_print_Click);
            // 
            // tb_message
            // 
            this.tb_message.Location = new System.Drawing.Point(12, 244);
            this.tb_message.Name = "tb_message";
            this.tb_message.Size = new System.Drawing.Size(100, 20);
            this.tb_message.TabIndex = 12;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 225);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(90, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Message to Send";
            // 
            // starter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(372, 494);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tb_message);
            this.Controls.Add(this.btn_print);
            this.Controls.Add(this.btn_terminateSingle);
            this.Controls.Add(this.tb_test);
            this.Controls.Add(this.lb_test);
            this.Controls.Add(this.btn_startSingle);
            this.Controls.Add(this.btn_startFromFolder);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cb_poisedReverse);
            this.Controls.Add(this.tb_name);
            this.Controls.Add(this.btn_terminateAll);
            this.Controls.Add(this.lb_processList);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "starter";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox lb_processList;
        private System.Windows.Forms.Button btn_terminateAll;
        private System.Windows.Forms.TextBox tb_name;
        private System.Windows.Forms.CheckBox cb_poisedReverse;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btn_startFromFolder;
        private System.Windows.Forms.Button btn_startSingle;
        private System.Windows.Forms.Label lb_test;
        private System.Windows.Forms.TextBox tb_test;
        private System.Windows.Forms.Button btn_terminateSingle;
        private System.Windows.Forms.Button btn_print;
        private System.Windows.Forms.TextBox tb_message;
        private System.Windows.Forms.Label label2;
    }
}

